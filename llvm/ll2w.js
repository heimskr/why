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
	 * @return {Object} The extracted `attributes` data.
	 */
	extractAttributes() {
		/**
		 * A map of attribute definitions.
		 * @type {Object<string, Array>}
		 */
		this.attributes = {};

		this.iterateTree("attributes", (name, attrs) => this.attributes[name] = attrs);
		return this.attributes;
	}

	/**
	 * Finds and extracts `struct` entries from the AST.
	 * @return {Object} The extracted `struct` data.
	 */
	extractStructs() {
		/**
		 * A map of struct definitions.
		 * @type {Object<string, Array>}
		 */
		this.structs = {};

		this.iterateTree("struct", (name, types) => this.structs[name] = types);
		return this.structs;
	}

	/**
	 * Finds and extracts `metadata` entries from the AST.
	 * @return {Object<string, object>} The extracted metadata.
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

		return this.metadata;
	}

	/**
	 * Finds and extracts global constant defintions from the AST.
	 * @return {Object} The extracted constant definitions.
	 */
	extractGlobalConstants() {
		/**
		 * A map of global constants.
		 * @type {Object<string, Object>}
		 */
		this.globalConstants = {};

		this.iterateTree("global constant", item => this.globalConstants[item.name] = _.omit(item, "name"));
		return this.globalConstants;
	}

	/**
	 * Finds and extracts function declarations from the AST.
	 * @return {Object<string, IRDeclaration>} The extracted function declarations.
	 */
	extractDeclarations() {
		/**
		 * A map of function declarations.
		 * @type {Object<string, Object>}
		 */
		this.declarations = {};
		
		this.iterateTree("declaration", dec => {
			const [, meta] = dec;
			const {name, type, types, arity, unnamedAddr} = meta;
			if (name in this.declarations) {
				warn("Duplicate declaration for", chalk.bold(name) + ".");
			}

			this.declarations[name] = {type, types, arity, isLocalUnnamed: unnamedAddr == "local_unnamed_addr"};
		});

		return this.declarations;
	}

	/**
	 * Finds and extracts function declarations from the AST.
	 * @return {Object<string, IRFunction>} The extracted function declarations.
	 */
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
				const blockVars = LL2W.computeBasicBlockVariables(block);
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

	static connectBlocks(functions, basicBlocks, declarations={}) {
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
	}

	static computeOperands(instruction) { // -> {read: var[], written: var[], assigner: ?var }
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

	static computeBasicBlockVariables(basicBlock) {
		let read = [], written = [], assigners = {};

		basicBlock[2].forEach(instruction => {
			const result = LL2W.computeOperands(instruction);
			read = read.concat(result.read);
			written = written.concat(result.written);
			if (result.assigner) assigners[result.assigner] = instruction;
		});
		
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

	/**
	 * Produces the control flow graph (CFG) for a given function.
	 * @param {IRFunction} fn A function AST.
	 * @return {Graph} A CFG with additional `enter`, `exit` and `unreachable` properties.
	 */
	static computeCFG(fn) {
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

	/**
	 * Computes the interference graph for a function.
	 * @param {IRFunction} fn       An LLVM IR function as formed by {@link extractFunctions}.
	 * @param {Graph}      cfg      A control flow graph as computed by {@link computeCFG}.
	 * @param {Object}     liveness A liveness map as computed by {@link computeLivenessForFunction}.
	 * @return {Graph} A graph with an extra `indexMap` property that maps instruction indices to the
	 *                 corresponding {@link Node} objects.
	 */
	static computeInterference(fn, cfg, liveness) {
		// An array that contains all the IRInstructions in the function.
		const instructions = _.flatten(fn.map(block => block[2].map(instr => { instr.block = block; return instr; })));
		
		// Create an array of all phi instructions. It contains tuples of the instruction index and the phi pairs
		// (in the form [variable name, source block name]).
		const phis = [];
		for (let i = instructions.length - 1; 0 <= i; --i) {
			const [, type, meta] = instructions[i];
			if (type != "phi") continue;
			phis.unshift([i, meta.pairs.map(pair => pair.map(([, varName]) => varName))]);
		}

		jsome(phis);

		return null;
	}

	/**
	 * Computes an object that indicates in which blocks of a function a given variable used in that function
	 * is live-in/live-out.
	 * @param {IRFunction} fn An LLVM IR function as formed by {@link extractFunctions}.
	 * @return {Object} A map of variable names to maps of block names to tuples of whether the variable is live-in in
	 *                  the block and whether the variable is live-out in the block.
	 */
	static computeLivenessForFunction(fn) {
		// An array of all variable names used in the function.
		const {vars} = fn;
		
		// An array of the labels of all basic blocks in the function.
		const blockNames = fn.map(([blockName]) => blockName);
		const blockPairs = fn.map(([blockName], blockIndex) => [blockName, blockIndex]);

		const out = {};
		const cfg = LL2W.computeCFG(fn);

		for (const varName of vars) {
			out[varName] = {};
			for (const [blockName, blockIndex] of blockPairs) {
				out[varName][blockName] = [
					LL2W.isLiveInUsingMergeSet (fn, cfg, fn[blockIndex], varName),
					LL2W.isLiveOutUsingMergeSet(fn, cfg, fn[blockIndex], varName)
				];
			}
		}

		return out;
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
			ms = cfg._mergeSets = _.mapKeys(_.mapValues(Graph.mergeSets(dj, cfg.enter, cfg.exit),
			                                            value => value.map(id => dj.nodes[id].data.label)),
			                                (value, key) => dj.nodes[key].data.label);
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
		if (block === undefined) {
			debugger;
		}

		const n = block[0];
		const blockDebug = n == fn.meta.arity? chalk.magenta("block " + n) : "block " + chalk.bold(n);
		debug(`Computing live-in for ${chalk.bold(varName)} in ${blockDebug}.`);

		const originalMergeSet = mergeSets[n];
		const modifiedMergeSet = _.uniq([...originalMergeSet, varName].map(x => x.toString()));
		const readers = LL2W.getReaders(fn, varName);
		const writers = LL2W.getWriters(fn, varName);
		let definition = null;
		if (writers.length == 1) {
			definition = writers[0][0];
		} else if (isNumeric(varName) && parseInt(varName) <= fn.meta.arity) {
			// Function arguments exist without any explicit defining instruction.
			// We'll assume they're implicitly defined in the first block.
			definition = fn.first;
		} else {
			throw new Error(`Variable %${varName} has ${writers.length} definitions; expected 1.`);
		}

		// for t ∈ uses(a)
		for (let [t] of readers) {
			// while t ≠ def(a)
			while (t != definition) {
				// Return true if t ∩ M^r(n)
				if (modifiedMergeSet.includes(t)) {
					debug(`Finished computing live-in  for ${chalk.bold(varName)} in ${blockDebug}:`,
					      `${chalk.bold("true")} because t ∩ M^r(n).`);
					return true;
				}

				// t = dom-parent(t)
				const parent = dTree.nodes[dTree.findSingle(node => node.data.label == t).in[0]].data.label;
				if (t != parent) {
					t = parent;
					continue;
				}

				if (t.toString() == fn.meta.arity.toString()) {
					// TODO: Is it correct to return false here?
					debug(chalk.yellow("Warning:"), `parent of ${t} is ${parent}. Returning ${chalk.red("false")}.`);
					return false;
				} else {
					debug(chalk.yellow("Warning:"), `parent of ${t} is ${parent}.`, chalk.red("This shouldn't happen."),
					      `Returning ${chalk.red("false")} to prevent an infinite loop.`);
					return false;
				}
			}
		}

		debug(`Finished computing live-in  for ${chalk.bold(varName)} in ${blockDebug}:`,
		      `${chalk.bold("false")} by default.`);
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
		const blockDebug = n == fn.meta.arity? chalk.cyan("block " + n) : "block " + chalk.bold(n);
		debug(`Computing live-out for ${chalk.bold(varName)} in ${blockDebug}.`);
		const {dTree, mergeSets} = LL2W.getUsefulLivenessData(cfg, {dj: true});
		const originalMergeSet = mergeSets[n];
		const modifiedMergeSet = _.uniq([...originalMergeSet, varName].map(x => x.toString()));
		const readers = LL2W.getReaders(fn, varName);
		const writers = LL2W.getWriters(fn, varName);
		const succ = n => cfg.findSingle(n).out;
		// const succ = n => cfg.getNode(n).out.map(s => {
			// console.log([s]); return s.toString().substr(s.indexOf(":") + 1)});
		// const rename = n => `${fnname}:${n}`;
		let definition = null;
		if (writers.length == 1) {
			definition = writers[0][0];
		} else if (isNumeric(varName) && parseInt(varName) <= fn.meta.arity) {
			// Function arguments exist without any explicit defining instruction.
			// We'll assume they're implicitly defined in the first block.
			definition = fn.first;
		} else {
			throw new Error(`Variable %${varName} has ${writers.length} definitions; expected 1.`);
		}

		// console.log(`def(a) = n  ~  ${definition} = ${n}  ~  ${definition == n}`);

		if (definition == n) {
			// console.log(`return _.without([...${readers.length}], ${n}) ≠ ∅ ->`, !!_.without(readers, n).length);
			// I'm assuming "φ" in the paper refers to the empty set.
			// return uses(a)\def(a) ≠ ∅
			const out = !!_.without(readers, n).length
			debug(`Finished computing live-out for ${chalk.bold(varName)} in ${blockDebug}:`,
			      `def(a) = n → (uses(a)\\def(a) ≠ ∅) == ${chalk.bold(out)}.`);
			return out;
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
					debug(`Finished computing live-out for ${chalk.bold(varName)} in ${blockDebug}:`
					      `${chalk.bold("true")} because t ∩ M_s(n).`);
					return true;
				}

				// t = dom-parent(t)
				const parent = dTree.nodes[dTree.findSingle(node => node.data.label == t).in[0]].data.label;
				if (t != parent) {
					t = parent;
					continue;
				}

				if (t.toString() == fn.meta.arity.toString()) {
					// TODO: Is it correct to return false here?
					debug(chalk.yellow("Warning:"), `parent of ${t} is ${parent}. Returning ${chalk.red("false")}.`);
					return false;
				} else {
					debug(chalk.yellow("Warning:"), `parent of ${t} is ${parent}.`, chalk.red("This shouldn't happen."),
					      `Returning ${chalk.red("false")} to prevent an infinite loop.`);
					return false;
				}
			}
		}

		debug(`Finished computing live-out for ${chalk.bold(varName)} in ${blockDebug}:`,
		      `${chalk.bold("false")} by default.`);
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

let debug = () => {};

if (require.main === module) {
	const options = minimist(process.argv.slice(2), {
		alias: {d: "debug"},
		boolean: ["debug"],
		default: {debug: false}
	}), infile = options._[0];

	if (options.debug) {
		debug = (...a) => console.log("🐞", ...a);
	}

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

	console.log();

	compiler.extractInformation();
	compiler.extractAttributes();
	compiler.extractStructs();
	compiler.extractMetadata();
	const declarations = compiler.extractDeclarations();
	compiler.extractGlobalConstants();
	const {functions, allBlocks, blockOrder, functionOrder} = compiler.extractFunctions();
	LL2W.connectBlocks(functions, allBlocks, declarations);

	testInterference(functions, allBlocks, declarations);
}

function testInterference(functions, allBlocks, declarations) {
	// Expecting disassemble.ll (dtest).
	const fn = functions.wvm_disassemble_r_alt_op;
	
	const cfg = LL2W.computeCFG(fn, declarations);
	const liveness = LL2W.computeLivenessForFunction(fn);

	console.log("Interference:");
	console.log(LL2W.computeInterference(fn, cfg, liveness));
}

function testLiveness(functions, declarations) {
	const fn = functions.liveness;
	// console.log(Object.keys(functions).map(key => [key, functions[key].length]));
	// LL2W.computeCFG(fn, declarations).display({width: 1000, height: 500}).then(() => console.log());
	// LL2W.computeCFG(fn, declarations).display({width: 4000*1, height: 1000*1}).then(() => console.log());
	const cfg = LL2W.computeCFG(fn, declarations);
	const dj = cfg.djGraph(cfg.enter);
	const ms = Graph.mergeSets(dj, cfg.enter, cfg.exit);
	const blockID = "8";
	const varName = "w";
	const block = fn.filter(([l]) => l == blockID)[0];
	// console.log(`isLiveOut(${blockID}, ${varName}):`, LL2W.isLiveOutUsingMergeSet(fn, cfg, block, varName));
	// console.log("liveness set:", compiler.computeLivenessSet(functions, allBlocks));
	console.log("liveness sets:");
	console.log(LL2W.computeLivenessForFunction(fn));
	return;
}

function testReversePost() {
	const cfg = new Graph(6).arcString("AB BC BD BF CE DE EB");
	
	cfg = new Graph(7);
	cfg.arc(0, 5); cfg.arc(0, 1); cfg.arc(0, 2);
	cfg.arc(3, 2); cfg.arc(3, 4); cfg.arc(3, 5); cfg.arc(3, 6);
	cfg.arc(1, 4); cfg.arc(5, 2);
	cfg.arc(6, 4); cfg.arc(6, 0);
	cfg.forEach(n => n.data={label: (n.id + 1)+""});
	cfg.forEach(n => n.data={label: (n.id)+""});

	console.log(cfg.reversePost());
	console.log(cfg.sortedDFS().map(n => n.id));
	return;
}

function test254Gap() {
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
}

function testMisc1() {
	// cfg = new Graph(8).arcString("AB BC BD CE DE EF FG GF GH");
	const cfg = new Graph(13);
	const q = "RABCDEFGHIJKL".split("");
	for (const pair of "RA RB RC AB AD BA BD BE BR CF CG DL EH FI GI GJ HE HK IK JI KI KR LH".split(" ")) {
		cfg.arc(q.indexOf(pair[0]), q.indexOf(pair[1]));
	}

	// console.log(cfg.dominance(5).map((x, i) => i+1 + " => " + (Number(x)==x?Number(x)+1:x)));
	// console.log(cfg.dominance(5).map((x, i) => i+1 + " => " + (Number(x)==x?Number(x)+1:x)));
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
