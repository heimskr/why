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

const {displayIOError, mixins, isNumeric} = require("../util.js");
mixins(_);

const warn = (...a) => console.log(chalk.dim("[") + chalk.bold.yellow("!") + chalk.dim("]"), ...a);

const {BUILTINS} = require("./constants.js");

/**
 * `ll2w` is an LLVM intermediate representation to WVM compiler (hence <code><b>ll</b>vm<b>2w</b>vm</code>)
 * written from scratch.
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

		metas.filter(([, id]) => _.isNumber(id)).forEach(([, id, distinct, ...items]) => {
			let recursive = false, toAdd = [];

			items.forEach((item, i) => {
				if (item == id) {
					recursive = true;
				} else if (typeof item == "number") {
					graph.arc(id, item);
				} else {
					toAdd.push(item);
				}
			});

			this.metadata[id] = {recursive, distinct, items: toAdd};
		});

		graph.sortedDFS().forEach(({id}) => graph.getNode(id).out.forEach(dependency => {
			this.metadata[id].items = _.unionWith(this.metadata[id].items, this.metadata[dependency].items, _.isEqual);
		}));

		metas.filter(([, id]) => !_.isNumber(id)).forEach(([, id, distinct, ...items]) => {
			this.metadata[id] = {
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

			let exitBlock = null;

			functionOrder.push(meta.name);
			
			// The first basic block is implicitly given a label whose name is equal to the function's arity.
			let currentBasicBlock = [meta.arity.toString(), {preds: [], in: [], out: []}, []];

			for (const instruction of instructions) {
				const [name, ...args] = instruction;
				if (name == "label_c") {
					basicBlocks.push(currentBasicBlock);
					currentBasicBlock = [args[0], {preds: args[1], in: [], out: []}, []];
				} else if (name == "label") {
					basicBlocks.push(currentBasicBlock);
					currentBasicBlock = [args[0], {preds: [], in: [], out: []}, []];
				} else {
					exitBlock = currentBasicBlock[0];
					currentBasicBlock[2].push(instruction);
				}
			}

			if (currentBasicBlock[2].length) {
				basicBlocks.push(currentBasicBlock);
			}

			basicBlocks.forEach(([name, ...etc]) => {
				allBlocks[meta.name + ":" + name] = etc;
				blockOrder.push([meta.name, name]);
			});
			
			let allVars = [];
			const fn = functions[meta.name] = basicBlocks.map(block => {
				const blockVars = this.extractBasicBlockVariables(block);
				allVars = [...allVars, ...blockVars[1].read, ...blockVars[1].written];
				return blockVars;
			});

			fn.meta = meta;
			fn.vars = _.uniq(allVars);
			fn.first = basicBlocks[0][0];
			fn.exit = exitBlock;
		});

		return {functions, allBlocks, blockOrder, functionOrder};
	}

	connectBlocks(functions, basicBlocks, declarations={}) {
		for (const [fullName, block] of Object.entries(basicBlocks)) {
			const [funcName, name] = fullName.split(":");
			const [meta, instructions] = block;
			const last = _.last(instructions);

			meta.unreachable = false;
			
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
			} else if (last[1] == "switch") {
				const imeta = last[2];
				const targets = [imeta.default, ...imeta.table.map(([,, v]) => v)];
				for (const [, target] of targets) {
					const tname = `${funcName}:${target}`;
					_.push(basicBlocks[tname][0].in, fullName);
					_.push(meta.out, tname);
				}
			} else if (last[1] == "unreachable") {
				meta.unreachable = true;
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
			meta.table.forEach(([,, dest]) => tryRead(dest));
			tryRead(meta.operand);
			tryRead(meta.default);
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

	static computeCFG(fn, declarations) {
		const g = new Graph(fn.length);
		const name = fn.meta.name;

		g.unreachable = [];

		// Assign all blocks to the rest of the nodes.
		fn.forEach((block, i) => {
			g[i].data = {label: block[0]};

			if (block[0] == fn.first) {
				g.enter = i;
			}

			if (block[0] == fn.exit) {
				g.exit = i;
			}

			if (block[1].unreachable) {
				g.unreachable.push(i);
			}
		});

		// Add an arc from the block to each of its outblocks.
		fn.forEach((block, i) => {
			const label = block[0];

			// Ignore outblocks that aren't within the same function.
			block[1].out
				.filter(s => s.substr(0, s.indexOf(":")) == name)
				.map(s => s.substr(s.indexOf(":") + 1))
				.map(s => g.findSingle(b => b.data.label == s))
				.forEach(n => g[i].arc(n));
		});

		return g;
	}

	computeLivenessForFunction(fn) {
		const {vars} = fn;
		const blockNames = fn.map(([blockName]) => blockName);

		const assigners = fn.reduce((a, b) => ({
			...a,
			..._.mapValues(b[1].assigners, x => {
				x[2].block = b[0];
				return x;
			})
		}), {});

		// A list of pairs of block IDs and the name of a variable that is read during some instruction in the block.
		// For example, if there's an instruction in block 5 that reads %y, one of the pairs will be ["5", "y"].
		const reads = fn.reduce((a, b) => [...a, ...b[1].read.map(v => [b[0], v])], []);
		const writes = fn.reduce((a, b) => [...a, ...b[1].written.map(v => [b[0], v])], []);
		console.log("Reads:", reads);

		const emptyLive = () => _.fromPairs(vars.map(v => [v, []]));
		const emptyProcessed = () =>
			_.fromPairs(blockNames.map(block => [block, _.fromPairs(vars.map(v => [v, false]))]));

		const liveIn = emptyLive(), liveOut = emptyLive();
		const processedIn = emptyProcessed(), processedOut = emptyProcessed();

		for (const varName of vars) {
			const varReads = reads.filter(([, v]) => v == varName).map(([b, ]) => b);
			const varWrites = writes.filter(([, v]) => v == varName).map(([b, ]) => b);
			console.log(varName, {reads: varReads, writes: varWrites});
		}
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
		const phiUses = instructions =>
			_.uniq(_.flatten(instructions.filter(([, instructionName]) => instructionName == "phi")
										 .map(([,, {pairs}]) => pairs.map(pair => pair[0][1]))));

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

	computeLiveRanges_old(fn) {
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

	static getReaders(fn, varName) {
		varName = varName.toString();
		return fn.filter(([id, {read}]) => read.map(x => x.toString()).includes(varName));
	}

	static getWriters(fn, varName) {
		varName = varName.toString();
		return fn.filter(([id, {written}]) => written.map(x => x.toString()).includes(varName));
	}

	static getUsefulLivenessData(cfg, ignore={dt: false, dj: false, ms: false}) {
		let dj = cfg._djGraph;
		let dt = cfg._dTree;
		let ms = cfg._mergeSets;
		const forceAll = !ms && !ignore.ms;

		if (forceAll || !dt && !ignore.dt) {
			dt = cfg._dTree = cfg.dTree(cfg.enter);
		}
		
		if (forceAll || !dj && !ignore.dj) {
			dj = cfg._djGraph = cfg.djGraph(dt);
		}
		
		if (forceAll) {
			ms = cfg._mergeSets = Graph.mergeSets(dj, cfg.enter, cfg.exit);
		}

		return {djTree: dj, dTree: dt, mergeSets: ms};
	}

	static invalidateUsefulLivenessData(cfg) {
		cfg._djGraph = cfg._dTree = cfg._mergeSets = undefined;
	}

	/**
	 * Determines whether a variable is live-in at a certain block.
	 * @param {IRFunction} fn An LLVM IR function as formed by {@link extractFunctions}.
	 * @param {Graph} cfg The control flow graph for the function.
	 * @param {IRBlock} block A basic block as formed in {@link extractFunctions}.
	 * @param {number|string} varName The name of the variable to check.
	 * @return {boolean} Whether the variable is live-in.
	 */
	static isLiveInUsingMergeSet(fn, cfg, block, varName) {
		varName = varName.toString();
		const {dTree, mergeSets} = LL2W.getUsefulLivenessData(cfg, {dj: true});
		const originalMergeSet = mergeSets[block[0]];
		const modifiedMergeSet = _.uniq([...originalMergeSet, varName].map(x => x.toString()));
		const readers = LL2W.getReaders(fn, varName);
		const writers = LL2W.getWriters(fn, varName);
		let definition = null;
		if (writers.length == 1) {
			definition = writers[0][0];
		} else {
			throw new Error(`Variable ${varName} has ${writers.length} definitions; expected 1.`);
		}

		// for t ∈ uses(a)
		for (let [t] of readers) {
			// while t ≠ def(a)
			while (t != definition) {
				// Return true if t ∩ M^r(n)
				if (modifiedMergeSet.includes(t)) {
					return true;
				}

				// t = dom-parent(t)
				t = dTree.nodes[dTree.findSingle(node => node.data.label == t).in[0]].data.label;
			}
		}

		return false;
	}

	/**
	 * Determines whether a variable is live-out at a certain block.
	 * @param {IRFunction} fn An LLVM IR function as formed by {@link extractFunctions}.
	 * @param {Graph} cfg The control flow graph for the function.
	 * @param {IRBlock} block A basic block as formed in {@link extractFunctions}.
	 * @param {number|string} varName The name of the variable to check.
	 * @return {boolean} Whether the variable is live-out.
	 */
	static isLiveOutUsingMergeSet(fn, cfg, block, varName) {
		varName = varName.toString();
		const fnname = fn.meta.name;
		const n = block[0];
		const {dTree, mergeSets} = LL2W.getUsefulLivenessData(cfg, {dj: true});
		const originalMergeSet = mergeSets[n];
		const modifiedMergeSet = _.uniq([...originalMergeSet, varName].map(x => x.toString()));
		const readers = LL2W.getReaders(fn, varName);
		const writers = LL2W.getWriters(fn, varName);
		const succ = n => cfg.getNode(n).out;
		// const succ = n => cfg.getNode(n).out.map(s => {
			// console.log([s]); return s.toString().substr(s.indexOf(":") + 1)});
		// const rename = n => `${fnname}:${n}`;
		let definition = null;
		if (writers.length == 1) {
			definition = writers[0][0];
		} else {
			throw new Error(`Variable ${varName} has ${writers.length} definitions; expected 1.`);
		}

		console.log(`def(a) = n  ~  ${definition} = ${n}  ~  ${definition == n}`);

		if (definition == n) {
			console.log(`return _.without([...${readers.length}], ${n}) ≠ ∅ ->`, !!_.without(readers, n).length);
			// I'm assuming "φ" in the paper refers to the empty set.
			// return uses(a)\def(a) ≠ ∅
			return !!_.without(readers, n).length;
		}

		// M_s(n) = ∅
		// Copy the merge sets and set the node's merge set to the empty set.
		const Ms = {..._.cloneDeep(modifiedMergeSet), [n]: []};

		// for w ∈ succ(n)
		for (const w of succ(n)) {
			// Ms(n) = Ms(n) ∪ Mr(w)
			// Add the merge set of the successor to the merge set of the node.
			Ms[n] = _.union(Ms[n], modifiedMergeSet[w]);
		}

		// for t ∈ uses(a)
		for (let [t] of readers) {
			// while t ≠ def(a) 
			while (t != definition) {
				// if t ∩ M_s(n)
				// Return true if the node's merge set includes the reader.
				if (Ms[n].includes(t)) {
					return true;
				}

				// t = dom-parent(t)
				t = dTree.nodes[dTree.findSingle(node => node.data.label == t).in[0]].data.label;
			}
		}

		return false;
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

	let cfg, ms, fn, dj;

	// jsome(_.cloneDeep(functions.wvm_check_condition || {}).map(x => {
	// 	const y = x.slice(0, 2);
	// 	delete y[1].assigners;
	// 	return y;
	// }));

	// const cfg = LL2W.computeCFG(functions, allBlocks, blockOrder, declarations);
	// let cfg = LL2W.computeCFG(functions.wvm_get_string, declarations);
	// const fn = functions.wvm_print_memory;
	// const fn = functions.wvm_get_string;
	// const fn = functions.wvm_check_condition;
	fn = functions.liveness;
	// console.log(Object.keys(functions).map(key => [key, functions[key].length]));
	// LL2W.computeCFG(fn, declarations).display({width: 1000, height: 500}).then(() => console.log());
	// LL2W.computeCFG(fn, declarations).display({width: 4000*1, height: 1000*1}).then(() => console.log());
	cfg = LL2W.computeCFG(fn, declarations);
	dj = cfg.djGraph(cfg.enter);
	ms = Graph.mergeSets(dj, cfg.enter, cfg.exit);
	const blockID = "8";
	const varName = "w";
	const block = fn.filter(([l]) => l == blockID)[0];
	// console.log(`isLiveOut(${blockID}, ${varName}):`, LL2W.isLiveOutUsingMergeSet(fn, cfg, block, varName));
	// console.log("liveness set:", compiler.computeLivenessSet(functions, allBlocks));
	console.log("liveness set:", compiler.computeLivenessForFunction(fn));
	return;

	// console.log(cfg.toString((i, n) => n.data.label, o => cfg[o].data.label));
	// console.log(cfg.toString());

	// compiler.debug(() => jsome({
		// sourceFilename: compiler.sourceFilename,
		// targets: compiler.targets,
		// attributes: compiler.attributes,
		// structs: compiler.structs,
		// metadata: compiler.metadata,
		// constants: compiler.globalConstants,
	// }));

	// return;

	// console.log(cfg.reversePost());

	// const g = new Graph(4).arcString("AB AC BD CD");
	// const g = new Graph(6).arcString("AB BC BD CE DE EB EF");
	// const g = new Graph(7).arcString("AB AD BC CB CG DE EF FD FG");

	
	// console.log(g.reversePost.map(x => String.fromCharCode("A".charCodeAt(0) + x)));
	// console.log(g.reversePost.map(x => x + 1));

	// Why does pred of _main:5 contain retvar:0?

	// console.log(cfg.dominance(0).map((x, i) => cfg[i].data.label + " => " + (x === null? "~" : cfg[x].data.label)));
	// console.log(cfg.toString((i, n) => n.data.label, o => cfg[o].data.label));

	
	// cfg = new Graph(6).arcString("AB BA BC CB DB DC EA FE FD"); // 0 2 1 3 4 5
	// cfg.forEach(n => n.data={label: (n.id + 1)+""});
	
	// console.log("RPO:", cfg.reversePost(5));
	
	// console.log("DFS:", cfg.dfs(5));
	// return;

	// cfg = new Graph(6).arcString("AB BC BD BF CE DE EB");
	
	// cfg = new Graph(7);
	// cfg.arc(0, 5); cfg.arc(0, 1); cfg.arc(0, 2);
	// cfg.arc(3, 2); cfg.arc(3, 4); cfg.arc(3, 5); cfg.arc(3, 6);
	// cfg.arc(1, 4); cfg.arc(5, 2);
	// cfg.arc(6, 4); cfg.arc(6, 0);
	// cfg.forEach(n => n.data={label: (n.id + 1)+""});
	// cfg.forEach(n => n.data={label: (n.id)+""});

	// console.log(cfg.reversePost());
	// console.log(cfg.sortedDFS().map(n => n.id));
	// return;

	// cfg = new Graph(8).arcString("AB BC BD CE DE EF FG GF GH");
	cfg = new Graph(13);
	const q = "RABCDEFGHIJKL".split("");
	for (const pair of "RA RB RC AB AD BA BD BE BR CF CG DL EH FI GI GJ HE HK IK JI KI KR LH".split(" ")) {
		cfg.arc(q.indexOf(pair[0]), q.indexOf(pair[1]));
	}

	// console.log(cfg.dominance(5).map((x, i) => i+1 + " => " + (Number(x)==x?Number(x)+1:x)));
	// console.log(cfg.dominance(5).map((x, i) => i+1 + " => " + (Number(x)==x?Number(x)+1:x)));
	
	cfg = new Graph(11).arcString("AB BK BC CD CH DE EF FE FG GB HI IF IJ JH");

	// cfg.renameNode(4, "e");
	// cfg.renameNode(1, "b");
	
	const ts = x => typeof x == "number"? x+1 : x;

	// console.log(chalk.dim("CFG:"));
	// console.log(cfg.toString(ts));
	// console.log("");
	// console.log(cfg.toString(x=>q[x], x=>q[x]));
	// cfg.lengauerTarjan(0);

	// Object.entries(cfg.lengauerTarjan(0)).forEach(([a, b]) => console.log(`idom[${+a+1}] = ${b+1}`));
	
	// console.log(cfg.dTree(0).toString(x=>+x+1, x=>+x+1));
	// console.log(cfg.dTree(0).toString(x=>q[x], x=>q[x]));
	// console.log(cfg.djGraph(0).toString(x=>q[x], x=>q[x]));
	// console.log(chalk.green(cfg.djGraph(0).jEdges.toString(ts)));
	// console.log(cfg.bfs().map(n => ts(n.id)));
	// console.log("\x1b[32m");
	// console.log(cfg.djGraph(0).jEdges);
	
	const dj254gap = new Graph(24);
	const pairs254gapLtR = "01 02 23 34 35 3-23 38 j45 56 57 j23-5 j23-8 j67 j75 j78 89 8-10 8-14 j9-10 14-15 14-16 j15-16 10-11 11-12 12-13 j13-1 16-22 j22-10 16-17 17-21 j21-22 17-18 18-19 18-20 j19-20 j20-18 j20-21".split(" ");
	const pairs254gapSorted = "01 02 23 34 35 38 3-23 j45 56 57 j23-5 j23-8 j67 j75 j78 89 8-10 8-14 j9-10 14-15 14-16 j15-16 10-11 11-12 12-13 j13-1 16-17 j22-10 16-22 17-18 j21-22 17-21 18-19 18-20 j19-20 j20-18 j20-21".split(" ");
	const pairs254gap = pairs254gapLtR;

	const jpairs254gap = pairs254gap.filter(s => s[0] == "j").map(s => s.substr(1));
	const jedges254gap = jpairs254gap.map(s => s.includes("-")? s.split(/-/).map(n => parseInt(n)) : [parseInt(s[0]), parseInt(s[1])]);
	const str254gap = pairs254gap.filter(s => s[0] != "j").join(" ");
	// console.log({jpairs254gap, jedges254gap, str254gap});
	dj254gap.arcString(str254gap);
	dj254gap.jEdges = jedges254gap;
	// console.log("dj254gap:", dj254gap);
	// console.log("dj254gap:\n" + dj254gap.toString());

	// dj254gap.display();
	
	console.log(chalk.dim("Calculating merge sets."));
	// const ms = cfg.mergeSets();
	ms = Graph.mergeSets(dj254gap, 0, 1);
	console.log(chalk.dim("\nResults:"));
	console.log(_.keys(ms).filter(k => ms[k].length).map(k => `${chalk.green(k.padStart(3, " "))}: ${[...ms[k]].sort().map(x => chalk.yellow(x)).join(", ")}`).join("\n"));
	// console.log(cfg.mergeSets());
	


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

	https://patchwork.kernel.org/patch/10295821/
	https://github.com/lucvoo/sparse-dev
*/
