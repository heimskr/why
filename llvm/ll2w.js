#!/usr/bin/env node
let minimist = require("minimist"),
	fs = require("fs"),
	chalk = require("chalk"),
	getline = require("get-line-from-pos"),
	nearley = require("nearley"),
	Graph = require("../graph.js"),
	_ = require("lodash"),
	util = require("util"),
	exec = util.promisify(require("child_process").exec),
	rimraf = require("rimraf"),
	shell_escape = require("shell-escape"),
	path = require("path"),
	jsome = require("jsome");

const {displayIOError, mixins} = require("../util.js");
mixins(_);

const warn = (...a) => console.log(chalk.dim("[") + chalk.bold.yellow("!") + chalk.dim("]"), ...a);

const {BUILTINS} = require("./constants.js");

/**
 * `ll2w` is an LLVM intermediate representation to WVM compiler (thus
 * <code><b>ll</b>vm<b>2w</b>vm</code>) written from scratch.
 * 
 * @module llvm/ll2w
 */

/**
 * Class representing an instance of the ll2w compiler. It contains all the methods comprising the
 * compilation process and various fields representing the internal state of the compiler.
 */
class LL2W {
	/**
	 * Creates a new compiler instance.
	 * @param {Object} options - An object containing various configuration options.
	 * @param {boolean} options.debug - Whether to output debug information.
	 */
	constructor(options) {
		/**
		 * The initial options supplied.
		 * @type {Object}
		 * @name module:llvm~LL2W#options
		 */
		this.options = options;
	}

	/**
	 * Sets up the compiler to use the LLVM parser.
	 */
	initialize() {
		try {
			/**
			 * The grammar rules for the LLVM IR parser.
			 * @type {grammar}
			 */
			this.grammar = require("./llvm.js");
		} catch (e) {
			console.error(`Couldn't read ${chalk.bold("llvm.js")}.`);
			if (this.options.debug) {
				console.error(e);
			}

			process.exit(1);
		}

		/**
		 * The LLVM IR parser.
		 * @type {nearley.Parser}
		 */
		this.parser = new nearley.Parser(this.grammar.ParserRules, this.grammar.ParserStart);
	}

	/**
	 * Loads LLVM intermediate representation source code (the typical file extension is `.ll`)
	 * and parses it.
	 * @param {string} text - The source code for the compiler to use.
	 */
	feed(text) {
		this.debug(() => console.time("parse"));
		this.initialize();

		/**
		 * The source code of the program as passed to feed() with one newline appended.
		 * @type {string}
		 */
		this.source = text + "\n";

		let trees;
		try {
			trees = this.parser.feed(this.source).results;
		} catch (e) {
			console.error(chalk.red("Syntax error"), "at", chalk.white(`${getline(this.source, e.offset)}:${e.offset - this.source.split(/\n/).slice(0, getline(this.source, e.offset) - 1).join("\n").length}`) + ":");
			if (this.options.dev) {
				console.log(e.message.replace(/\(@(\d+):([^)]+)\)/g, ($0, $1, $2) => `(@${getline(this.source, e.offset)}:${e.offset - this.source.split(/\n/).slice(0, getline(this.source, e.offset)).join("\n").length + $2})`));
			}

			process.exit(1);
		}

		if (trees.length > 1) {
			trees.forEach(tree => console.log(JSON.stringify(trees[tree], null, 4)));
			console.error(chalk.red.italic(`\nAmbiguous grammar (${trees.length}).\n`));
			process.exit(1);
		} else if (trees.length == 0) {
			console.error(chalk.red.italic("Nothing was parsed."));
			process.exit(1);
		}

		/**
		 * The abstract syntax tree of the LLVM IR.
		 * @type {Array}
		 */
		this.ast = trees[0];

		this.debug(() => console.timeEnd("parse"));
		if (typeof this.ast != "object") {
			LL2W.die("Error: parser output isn't an object.");
		}
	}

	/**
	 * Iterates the AST and executes a function for every top-level entry of a given type.
	 * @param {?string} type - The type of entry to iterate for. Entries not of this type will be
	 * ignored. If null, all entries will be iterated for.
	 * @param {function} fn - The function to call on each iteration.
	 */
	iterateTree(type, fn) {
		if (!this.ast) {
			throw new Error("AST not yet generated");
		}

		if (type == null) {
			this.ast.forEach(entry => fn(...entry));
		} else {
			this.ast.forEach(([name, ...args]) => name == type && fn(...args));
		}
	}

	/**
	 * Finds and extracts `source_filename` and `target` entries from the AST.
	 */
	extractInformation() {
		/**
		 * The value of the source_filename entry in the source.
		 * Equal to `null` if the source doesn't contain a source_filename entry.
		 * @type {?string}
		 */
		this.sourceFilename = null;

		/**
		 * A map of target definitions.
		 * @type {Object<string, string>}
		 */
		this.targets = {};

		this.iterateTree("source_filename", name => this.sourceFilename = name);
		this.iterateTree("target", (key, value) => this.targets[key] = value);
	}

	/**
	 * Finds and extracts `attributes` entries from the AST.
	 */
	extractAttributes() {
		/**
		 * A map of attribute definitions.
		 * @type {Object<string, Array>}
		 */
		this.attributes = {};

		this.iterateTree("attributes", (name, attrs) => this.attributes[name] = attrs);
	}

	/**
	 * Finds and extracts `struct` entries from the AST.
	 */
	extractStructs() {
		/**
		 * A map of struct definitions.
		 * @type {Object<string, Array>}
		 */
		this.structs = {};

		this.iterateTree("struct", (name, types) => this.structs[name] = types);
	}

	/**
	 * Finds and extracts `metadata` entries from the AST.
	 */
	extractMetadata() {
		/**
		 * A map of metadata entries.
		 * @type {Object<string, Object>}
		 */
		this.metadata = {};

		const metas = this.ast.filter(([type]) => type == "metadata");
		const graph = new Graph(metas.length);

		metas.filter(([, name]) => _.isNumber(name)).forEach(([, name, distinct, ...items]) => {
			let recursive = false, toAdd = [];

			items.forEach((item, i) => {
				if (item == name) {
					recursive = true;
				} else if (typeof item == "number") {
					graph.arc(name, item);
				} else {
					toAdd.push(item);
				}
			});

			this.metadata[name] = {recursive, distinct, items: toAdd};
		});

		graph.sorted().forEach(({id: name}) => graph.getNode(name).out.forEach(dependency => {
			this.metadata[name].items = _.unionWith(this.metadata[name].items, this.metadata[dependency].items, _.isEqual);
		}));

		metas.filter(([, name]) => !_.isNumber(name)).forEach(([, name, distinct, ...items]) => {
			this.metadata[name] = {
				recursive: false,
				distinct, 
				items: _.uniqWith(_.flatten(items.map(i => this.metadata[i].items)), _.isEqual)
			};
		});
	}

	/**
	 * Finds and extracts global constant defintions from the AST.
	 */
	extractGlobalConstants() {
		const constants = {};
		this.iterateTree("global constant", item => constants[item.name] = _.omit(item, "name"));
		return constants;
	}

	extractDeclarations() {
		const decs = {};
		this.iterateTree("declaration", dec => {
			const [, meta] = dec;
			const {name, type, types, arity, unnamedAddr} = meta;
			if (name in decs) {
				warn("Duplicate declaration for", chalk.bold(name) + ".");
			}

			decs[name] = {type, types, arity, isLocalUnnamed: unnamedAddr == "local_unnamed_addr"};
		});

		return decs;
	}

	extractFunctions() {
		const functions = {};
		const allBlocks = {};
		const blockOrder = [];
		const functionOrder = [];

		this.iterateTree("function", (meta, instructions) => {
			// LLVM helpfully indicates basic blocks with comments and labels.
			// If we cheat, we can exploit this to avoid having to implement an actual basic block scanning algorithm.
			// (Even though it wouldn't be very difficult to implement such an algorithm.)
			const basicBlocks = [];

			functionOrder.push(meta.name);
			
			// The first basic block is implicitly given a label whose name is equal to the function's arity.
			let currentBasicBlock = [meta.arity, {preds: [], in: [], out: []}, []];

			for (const instruction of instructions) {
				const [name, ...args] = instruction;
				if (name == "label_c") {
					basicBlocks.push(currentBasicBlock);
					currentBasicBlock = [args[0], {preds: args[1], in: [], out: []}, []];
				} else if (name == "label") {
					basicBlocks.push(currentBasicBlock);
					currentBasicBlock = [args[0], {preds: [], in: [], out: []}, []];
				} else {
					currentBasicBlock[2].push(instruction);
				}
			}

			if (currentBasicBlock[2].length) {
				basicBlocks.push(currentBasicBlock);
			}

			let allVars = [];
			functions[meta.name] = basicBlocks.map(block => {
				const blockVars = this.extractBasicBlockVariables(block);
				allVars = [...allVars, ...blockVars[1].read, ...blockVars[1].written];
				return blockVars;
			});
			basicBlocks.forEach(([name, ...etc]) => {
				allBlocks[meta.name + ":" + name] = etc;
				blockOrder.push([meta.name, name]);
			});
			functions[meta.name].meta = meta;
			functions[meta.name].vars = _.uniq(allVars);
			functions[meta.name].first = basicBlocks[0][0];
		});

		return {functions, allBlocks, blockOrder, functionOrder};
	}

	connectBlocks(functions, basicBlocks, declarations={}) {
		for (const [fullName, block] of Object.entries(basicBlocks)) {
			const [funcName, name] = fullName.split(":");
			const [meta, instructions] = block;
			const last = _.last(instructions);
			
			if (last[1] == "br_unconditional") {
				const destName = `${funcName}:${last[2].dest[1]}`;
				if (!(destName in basicBlocks)) {
					throw new Error(`Couldn't find a basic block called ${destName}.`);
				}

				const destBlock = basicBlocks[destName];
				_.push(meta.out, destName);
				_.push(destBlock[0].in, fullName);
			} else if (last[1] == "br_conditional") {
				// I'm assuming that conditional branches go only to other basic blocks within the same function.
				const {iftrue: [, iftrue], iffalse: [, iffalse]} = last[2];
				let trueName  = `${funcName}:${iftrue}`,
					falseName = `${funcName}:${iffalse}`;
				_.push(basicBlocks[trueName][0].in, fullName);
				_.push(basicBlocks[falseName][0].in, fullName);
				_.push(meta.out, trueName);
				_.push(meta.out, falseName);
			} else {
				// console.log(last[1]);
			}

			for (const instruction of instructions) {
				const [type, name, imeta] = instruction;
				if (type != "instruction") {
					continue;
				}

				if (name == "call") {
					const iname = imeta.name;
					const arity = LL2W.getArity(functions, iname, declarations);
					const destName = `${iname}:${arity}`;
					if (LL2W.builtins.includes(iname)) {
						// Because builtins are single machine instructions and not real functions,
						// we don't include them in the control flow graph.
						continue;
					}

					_.push(meta.out, destName);
					if (destName in basicBlocks) {
						// First, make a link from this block to the start of the called function.
						const destBlock = basicBlocks[destName];
						_.push(destBlock[0].in, fullName);
						
						// Next, we link each return statement from the called function back to this block.
						// Depending on the exact circumstances, some of the returns (but not all) might not ever return
						// control to this block, but I imagine it would be extremely tricky to determine which do, so
						// just have to assume that all of them do. This might end up decreasing performance due to
						// overly cautious register allocation, but it's probably not a huge concern.
						for (const [calledName, {out: calledOut}, calledInstructions] of functions[iname]) {
							const [lastType, lastName] = _.last(calledInstructions);
							if (lastType == "instruction" && lastName == "ret") {
								_.push(calledOut, fullName);
								_.push(meta.in, `${iname}:${calledName}`);
							}
						}
					} else if (typeof iname == "number" || (typeof iname == "string" && iname.match(/^\d+$/))) {
						// If the function name is a number, that means the function is actually a function pointer.
						// As those could point anywhere, it's not possible to do anything with them, so we do nothing.
					} else if (!(iname in declarations)) {
						warn(`Couldn't find a basic block called ${chalk.bold(iname + chalk.dim(":" + arity))}.`);
					}
				}
			}
		}

		// Object.keys(basicBlocks).forEach(k => console.log(chalk.bold(k), "in:", basicBlocks[k][0].in, "out:", basicBlocks[k][0].out));
		// console.log(functions);
	}

	static extractOperands(instruction) { // -> {read: var[], written: var[], assigner: ?var }
		const [, type, meta] = instruction;
		let read = [], written = [], assigner = null;

		const isVar = x => x instanceof Array && x[0] == "variable";
		const tryRead = x => isVar(x) && read.push(x[1]);

		if (["phi", "alloca", "conversion", "load", "binary", "icmp", "getelementptr"].includes(type)) {
			written.push(assigner = meta.destination[1]);
		}

		if (type == "phi") {
			// TODO: do phi instructions count as reads?
			meta.pairs.forEach(([v, src]) => tryRead(v));
		} else if (type == "switch") {
			tryRead(meta.operand);
		} else if (type == "select") {
			written.push(assigner = meta.destination[1]);
			read.push(meta.condition[1]);
			[meta.leftValue, meta.rightValue].forEach(tryRead);
		} else if (type == "conversion") {
			tryRead(meta.sourceValue)
		} else if (type == "call") {
			if (meta.assign) {
				written.push(assigner = meta.assign[1]);
			}

			if (typeof meta.name == "number") {
				// This call is to a function pointer.
				read.push(meta.name);
			}

			for (const arg of meta.args) {
				tryRead(arg[1]);
			}
		} else if (type == "store") {
			tryRead(meta.storeValue);
			
			if (isVar(meta.destinationValue)) {
				// Yes, the store writes a value, but it doesn't write it to a variable.
				// Instead, it writes it to memoryat the position contained in the destination pointer.
				// That means it has two reads and no writes!
				read.push(assigner = meta.destinationValue[1]);
			}
		} else if (type == "load") {
			tryRead(meta.pointerValue);
		} else if (type == "binary" || type == "icmp") {
			[meta.op1, meta.op2].forEach(o => isVar(o) && read.push(o[1]));
		} else if (type == "br_conditional") {
			// TODO: do branch targets count as reads?
			if (typeof meta.cond != "number") {
				read.push(meta.cond[1]);
			}
		} else if (type == "ret") {
			tryRead(meta.value);
		} else if (type == "getelementptr") {
			tryRead(meta.pointerValue);
			meta.indices.filter(i => isVar(i[1])).forEach(i => read.push(i[1][1]));
		}

		return {read, written, assigner};
	}

	extractBasicBlockVariables(basicBlock) {
		let read = [], written = [], assigners = {};

		const add = instruction => {
			const result = LL2W.extractOperands(instruction);
			read = read.concat(result.read);
			written = written.concat(result.written);
			if (result.assigner) assigners[result.assigner] = instruction;
		};

		basicBlock[2].forEach(add);
		
		basicBlock[1].read = _.uniq(read);
		basicBlock[1].written = _.uniq(written);
		basicBlock[1].assigners = assigners;
		return basicBlock;
	}

	static getAllVariables(fn) {
		const {arity} = fn.meta;
		const out = [];
		
		for (const [k, {read, written}] of fn) {
			_.pushAll(out, [...read, ...written].filter(x => typeof x != "number" || arity < x));
		}
		
		return out;
	}

	static fromSameFunction(...blockNames) {
		if (blockNames.length < 2) {
			return blockNames.length == 1;
		}

		const firstName = blockNames[0].substr(0, blockNames[0].indexOf(":"));
		for (const nextName of blockNames.slice(1)) {
			if (firstName != nextName)
				return false;
		}

		return true;
	}

	static computeCFG(fns, blocks, blockOrder) {
		const n = Object.keys(blocks).length;
		const g = new Graph(n + 2);

		// Entry node: 0
		// Exit node: 1 (first need to figure out how to determine the exit node...)

		if ("_main" in fns) {
			// If a _main function exists, its first block will be chosen as the entry node.
			g[0].data = {fn: "_main", block: fns._main.first};
			g[1].data = {fn: "_main", block: _.last(fns._main)[0]};
		} else {
			// Otherwise, choose the very first block in the program. Who knows what the exit node should be.
			g[0].data = {fn: blockOrder[0][0], block: blockOrder[0][1]};
		}

		// Assign all blocks to the rest of the nodes.
		blockOrder.forEach(([fnName, blockIndex], i) => {
			g[i + 2].data = {fn: fnName, block: blockIndex};
		});

		blockOrder.forEach(([fnName, blockIndex], i) => {
			// Add an arc from the block to each of its outblocks.
			const block = blocks[fnName + ":" + blockIndex];
			if (!block) {
				throw `Block "${fnName}:${blockIndex} not found.`;
			}

			for (const id of block[0].out) {
				const targets = g.filter(({data}, j) => 2 <= j && `${data.fn}:${data.block}` == id);
				
				if (!targets || targets.length === undefined) {
					throw `Couldn't find target ${id}`;
				}

				if (targets.length != 1) {
					throw `Found ${targets.length} targets for ${id}`;
				}

				g[i + 2].arc(targets[0]);
			}
		});
	}

	computeLivenessSet(fns, blocks) {
		// Based on Algorithm 9.9 ("Compute liveness sets per variable using def-use chains")
		// and Algorithm 9.10 ("Optimized path exploration using a stack-like data structure")
		// from http://ssabook.gforge.inria.fr/latest/book-full.pdf
		
		console.log();
		
		const liveIn  = _.objectify(_.keys(blocks), () => []);
		const liveOut = _.objectify(_.keys(blocks), () => []);

		// For each pair in each phi instruction in a set of instructions, take the first element of the pair
		// (the source variable) and take its second element (the name of the variable).
		// This results in an array containing every variable in the function that's used in the block's phi functions.
		const phiUses = instrs => _.uniq(_.flatten(instrs.filter(i => i[1] == "phi").map(i => i[2].pairs.map(p => p[0][1]))));

		const _b = chalk.bold;

		const _same = LL2W.fromSameFunction;
		const s = 0;

		const upAndMark = (blocks, name, fullName, v) => {
			/* Algorithm 9.10: Optimized path exploration using a stack-like data structure.
				Function Up_and_Mark_Stack(B, v) {
					// Killed in the block, stop
					if def(v) ∈ B (φ excluded)
						return

					// Propagation already done, stop
					if top(LiveIn(B)) = v
						return
					
					push(LiveIn(B), v)
					
					// Do not propagate φ definitions
					if v ∈ PhiDefs(B)
						return

					// Propagate backwards
					foreach P ∈ CFG_preds(B) {
						if top(LiveOut(P)) ≠ v
							push(LiveOut(P), v)
						
						Up_and_Mark_Stack(P, v)
					}
				}
			*/

			// console.log(name, blocks[name]);

			const def = blocks[name].assigners[v];

			// Killed in the block.
			if (def && def[1] != "phi") {
				s||console.log(chalk.red(`${_b(v)} killed in ${_b(fullName)}.`));
				return;
			}

			// Propagation already done.
			if (_.last(liveIn[fullName]) == v) {
				s||console.log(chalk.yellow(`Propagation already done for ${_b(v)} in ${_b(fullName)}.`));
				return;
			}

			s||console.log(`liveIn[${_b(fullName)}].push(${_b(v)})`);
			liveIn[fullName].push(v);

			// Do not propagate φ definitions.
			if (def && def[1] == "phi") {
				s||console.log(chalk.cyan(`Not propagating φ definition for ${_b(v)} in ${_b(fullName)}.`));
				return;
			}

			// console.log({in:blocks[name].in});

			// Propagate backwards.
			for (const p of blocks[name].in) {
				if (!_same(fullName, p)) continue;

				if (_.last(liveOut[p]) != v) {
					s||console.log(chalk.green(`Yep, top(liveOut[${_b(p)}]) ≠ ${_b(v)}`) + ` (${_b(_.last(liveOut[p]))})`);
					liveOut[p].push(v);
				} else s||console.log(chalk.magenta(`Nope, top(liveOut[${_b(p)}]) = ${_b(v)}`));

				upAndMark(blocks, p.substr(1 + p.indexOf(":")), p, v);
			}
		};

		/* Algorithm 9.9: Compute liveness sets per variable using def-use chains
			Function Compute_LiveSets_SSA_ByVar(CFG) {
				foreach variable v {
					foreach block B where v is used {
						// Used in the φ of a successor block
						if v ∈ PhiUses(B)
							LiveOut(B) = LiveOut(B) ∪ {v}

						Up_and_Mark(B, v)
					}
				}
			}
		*/

		for (const [fname, fn] of Object.entries(fns)) {
			for (const v of LL2W.getAllVariables(fn)) {
				// Loop through each block where v is used.
				fn.filter(b => b[1].read.includes(v)).map(block => {
					const b = `${fname}:${block[0]}`;
					const blocks = _.fromPairs(fn.map(([id, meta]) => [id, meta]));

					if (phiUses(block[2]).includes(v)) {
						s||console.log(chalk.dim(`${_b(v)} ∈ PhiUses(${_b(b)})`));
						_.push(liveOut[b], v);
					}
	
					upAndMark(blocks, block[0], b, v);
				});
			}
		}

		return {liveIn, liveOut};

		// for (const v of LL2W.getAllVariables(fns, blocks)) {
		// 	// Loop through each block where v is used.
		// 	_.keys(blocks).filter(b => blocks[b][0].read.includes(v)).map(b => {
		// 		if (phiUses(b).includes(v)) {
		// 			s||console.log(chalk.dim(`${_b(v)} ∈ PhiUses(${_b(b)})`));
		// 			_.push(liveOut[b], v);
		// 		}

		// 		upAndMark(b, v);
		// 	});
		// }

		// s||console.log();
		// return {liveIn, liveOut};
	}

	computeLiveRanges(fn) {
		// Is this actually how it's done? In some examples, it's done backwards.
		// I'd imagine live variable analysis is less complicated in SSA because variables are assigned once,
		// which should mean that each variable's live range shouldn't be made of multiple separate components.

		// The arguments of the function are stored in %0, %1, ...
		const numArgs = fn.meta.arity;

		const instructions = fn.reduce((a, b) => a.concat(b[2]), []);
		const assigners = fn.reduce((a, b) => ({...a, ...b[1].assigners}), {});

		let allVars = _.range(0, numArgs);
		let i = 0;
		for (const instruction of instructions) {
			const {read, written, assigner} = LL2W.extractOperands(instruction);
			allVars = [...allVars, ...read, ...written];
			process.stdout.write((i+++":").padEnd(5, " "));
			console.log(chalk.bold((instruction[1] + ":").padEnd("br_unconditional:  ".length, " ")), (read.join(", ") || chalk.dim("  .   ")).padEnd(6, " "), chalk.red(" → "), (written.join(", ") || chalk.dim(". ")).padEnd(2, " "), chalk.red(" /"), chalk.cyan.dim(assigner));
		}

		console.log();
		allVars = _.uniq(allVars);
		const ranges = _.fromPairs(allVars.map(v => [v, [v < numArgs? 0 : null, null]]));
		const _warned = [];

		for (const v of allVars) {
			let range = ranges[v]; // [position first written, position last read]
			instructions.forEach((instruction, i) => {
				const {read, written} = LL2W.extractOperands(instruction);

				if (written.includes(v)) {
					if (range[0] == null) {
						// If we haven't already set the starting position and this instruction writes to the variable,
						// set this instruction's position as the starting point of the variable's live range.
						range[0] = i;
					} else if (!_warned.includes(v)) {
						warn(`Variable ${chalk.bold(`%${v}`)} is assigned multiple times.\n`);
						_warned.push(v);
					}
				}

				if (read.includes(v)) {
					// If this instruction reads from the variable, set it as the end position.
					// It doesn't matter if the end position has already been set.
					range[1] = i;
				}
			});

			if (range[0] == null && range[1] != null && typeof v == "number" && numArgs < v) {
				warn(`Variable ${chalk.bold(`%${v}`)} is read but never assigned in ${chalk.bold(fn.meta.name)}.`);
			} else if (range[0] != null && range[1] == null) {
				// Sometimes, the return value of a call is stored in a variable that isn't ever read.
				range[1] = range[0];
				if (numArgs <= range[0] && !(assigners[v] instanceof Array && assigners[v][1] == "call")) {
					// Don't warn if the function arguments aren't read. The compiler has already complained about it.
					warn(`Variable ${chalk.bold(`%${v}`)} is assigned but never read in ${chalk.bold(fn.meta.name)}.`);
				}
			}
		}

		console.log();
		console.log(ranges);
	}

	static getArity(functions, functionName, declarations={}) {
		if (functionName in functions) {
			return functions[functionName].meta.arity;
		}

		if (functionName in BUILTINS) {
			return BUILTINS[functionName].arity;
		}

		if (functionName in declarations) {
			return declarations[functionName].arity;
		}

		return -1;
	}

	/**
	 * Executes a function if options.debug is true. Does nothing otherwise.
	 * @param {function} fn - The function to execute.
	 **/
	debug(fn) {
		this.options.debug && fn(this);
	}

	/**
	 * Prints a message to stderr and exits the process with return code 1.
	 * @param {...*} args - The arguments to pass to `console.error`.
	 */
	static die(...args) {
		console.error(...args);
		process.exit(1);
	}

	static async produceIR(dirName, ...filenames) {
		let deleteDir = false;
		if (!dirName) {
			let {err, stdout, stderr} = await exec("mktemp -d");
			if (err) throw err;
			dirName = stdout.trim();
			deleteDir = true;
		}

		console.log(`Using ${chalk.bold(dirName)}.`);
		
		const resolved = filenames.map(f => path.resolve(f));
		const cmd = shell_escape(["clang", "-S", "-emit-llvm", ...resolved]);

		process.chdir(dirName);

		let {err, stdout, stderr} = await exec(cmd);
		if (err) throw err;
		
		console.log({err, stdout, stderr});

		if (deleteDir) {
			rimraf.sync(dirName);
		}
	}
}

LL2W.builtins = ["_int", "_rit", "_time", "_gettime", "_halt", "_prc", "_prd", "_prx"];

module.exports = LL2W;

if (require.main === module) {
	const options = minimist(process.argv.slice(2), {
		alias: {d: "debug"},
		boolean: ["debug"],
		default: {d: false}
	}), infile = options._[0];

	if (!infile) {
		console.log("Usage: ./ll2w.js <filename> [out]");
		process.exit(0);
	}

	let outfile = options._[1] || infile.replace(/\.ll$/, "") + ".why";
	const compiler = new LL2W(options);

	if (!outfile) {
		outfile = infile.replace(/\.ll$/, "") + ".why";
	}

	try {
		compiler.feed(fs.readFileSync(infile, "utf8"));
	} catch(e) {
		console.log(e);
		displayIOError(e, infile);
		process.exit(1);
	}

	// jsome(compiler.ast);
	console.log();

	compiler.extractInformation();
	compiler.extractAttributes();
	compiler.extractStructs();
	compiler.extractMetadata();
	const declarations = compiler.extractDeclarations();
	compiler.globalConstants = compiler.extractGlobalConstants();
	const {functions, allBlocks, blockOrder, functionOrder} = compiler.extractFunctions();
	compiler.connectBlocks(functions, allBlocks, declarations);
	LL2W.computeCFG(functions, allBlocks, blockOrder);

	0&&compiler.debug(() => jsome({
		sourceFilename: compiler.sourceFilename,
		targets: compiler.targets,
		attributes: compiler.attributes,
		structs: compiler.structs,
		metadata: compiler.metadata,
		constants: compiler.globalConstants,
	}));

	// console.log(compiler.computeLivenessSet(compiler.functions, compiler.allBlocks));

	// for (const fn in compiler.functions) {
		// console.log(chalk.underline("\n\n\nLive range for " + fn) + ":\n");
		// compiler.computeLiveRanges(compiler.functions[fn]);
	// }

	// jsome(compiler.ast);
}

/*
Potentially useful links:
	http://stackoverflow.com/questions/36087319/llvm-openmp-what-is-the-meaning-of-internal-thread-local-unnamed-addr-global/36094052

	LLVM backends:
		http://llvm.org/docs/WritingAnLLVMBackend.html
		https://jonathan2251.github.io/lbd/TutorialLLVMBackendCpu0.pdf
		https://opus4.kobv.de/opus4-fau/files/1108/tricore_llvm.pdf

	Register allocation:
		https://en.wikipedia.org/wiki/Register_allocation#Iterated_Register_Coalescing
		https://www.cs.cmu.edu/~fp/courses/15411-f09/lectures/03-regalloc.pdf
*/
