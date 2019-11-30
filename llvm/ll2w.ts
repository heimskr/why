#!/usr/bin/env ts-node
import minimist = require("minimist");
import fs = require("fs");
const chalk = require("chalk");
import getline = require("get-line-from-pos");
import {Grammar, Parser, CompiledRules} from "nearley";
import Graph, {BothMap, Node, NodeID} from "../graph";
import util = require("util");
import child_process = require("child_process");
import rimraf from "rimraf";
import shell_escape from "shell-escape";
import * as path from "path";
import jsome = require("jsome");
const exec = util.promisify(child_process.exec);

import _, {displayIOError, isNumeric, pushAll} from "../util";
import {BUILTINS} from "./constants";

import {StringMap, MetadataType, DeclarationType, IRMetadata, ASTFunction, IRFunction, BasicBlock, FunctionExtractions,
	VariableName, BasicBlockExtra, ASTDeclaration, IRFunctionMeta, Instruction, ComputedOperands, IRFunctionBlock,
	CachingCFG, UsefulLivenessData, isLabelComment, LabelComment, isLabel, BlockName, Label, isASTVariable, isPhi, LLVMFunctionMeta, isSwitch, isSelect, isConversion, isCall, isStore, isLoad, isBinary, isICMP, isBrCond, isRet, isGetElementPtr} from "./types";

export type LL2WOptions = {debug?: boolean, cfg?: boolean, dev?: boolean};
export type LivenessMap = {[varName: string]: {[blockName: string]: [boolean, boolean]}};
export type CFG = Graph<{enter: number, exit: number, unreachable: number[]}>;
export function isCFG(x: Graph<any>): x is CFG {
	return "enter" in x.data && "exit" in x.data && "unreachable" in x.data;
}

const warn = (...a: any[]) => console.log(chalk.dim("[") + chalk.bold.yellow("!") + chalk.dim("]"), ...a);

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
	options: LL2WOptions;
	grammar?: nearley.CompiledRules;
	parser?: nearley.Parser;
	source: string | null = null;
	ast?: Object[][];
	sourceFilename: string | null = null;
	targets?: StringMap<string>;
	attributes?: StringMap<Object>;
	structs?: StringMap<Object>;
	metadata?: StringMap<MetadataType>;
	globalConstants?: StringMap<Object>;
	declarations?: StringMap<DeclarationType>;
	static builtins = ["_int", "_rit", "_time", "_gettime", "_halt", "_prc", "_prd", "_prx"];


	/**
	 * Creates a new compiler instance.
	 * @param {Object} options - An object containing various configuration options.
	 * @param {boolean} options.debug - Whether to output debug information.
	 */
	constructor(options: LL2WOptions = {}) {
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
	initialize(): void {
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
			throw e;
		}

		if (!this.grammar) {
			throw new Error("Loading grammar failed.");
		}

		/**
		 * The LLVM IR parser.
		 * @type {nearley.Parser}
		 */
		this.parser = new Parser(Grammar.fromCompiled(this.grammar));
	}

	/**
	 * Loads LLVM intermediate representation source code (the typical file extension is `.ll`)
	 * and parses it.
	 * @param {string} text - The source code for the compiler to use.
	 */
	feed(text: string): void {
		this.debug(() => console.time("parse"));
		this.initialize();

		/**
		 * The source code of the program as passed to feed() with one newline appended.
		 * @type {string}
		 */
		const source: string = this.source = text + "\n";

		let trees;
		try {
			trees = this.parser.feed(this.source).results;
		} catch (e) {
			console.error(chalk.red("Syntax error"), "at", chalk.white(getline(source, e.offset) + ":" +
				(e.offset - source.split(/\n/).slice(0, getline(source, e.offset) - 1).join("\n").length) + "):"));

			if (this.options.dev) {
				console.log(e.message.replace(/\(@(\d+):([^)]+)\)/g, ($0: never, $1: never, $2: string) =>
					`(@${getline(source, e.offset)}:` + (e.offset - source.split(/\n/).slice(0,
					getline(source, e.offset)).join("\n").length) + $2 + ")"));
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
	iterateTree(type: string | null, fn: Function): void {
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
	extractInformation(): void {
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
		const targets: StringMap<string> = this.targets = {};

		this.iterateTree("source_filename", (name: string) => this.sourceFilename = name);
		this.iterateTree("target", (key: string, value: string) => targets[key] = value);
	}

	/**
	 * Finds and extracts `attributes` entries from the AST.
	 * @return {Object} The extracted `attributes` data.
	 */
	extractAttributes(): StringMap<Object> {
		/**
		 * A map of attribute definitions.
		 * @type {Object<string, Array>}
		 */
		const attributes: StringMap<Object> = this.attributes = {};

		this.iterateTree("attributes", (name: string, attrs: Object) => attributes[name] = attrs);
		return this.attributes;
	}

	/**
	 * Finds and extracts `struct` entries from the AST.
	 * @return {Object} The extracted `struct` data.
	 */
	extractStructs(): StringMap<Object> {
		/**
		 * A map of struct definitions.
		 * @type {Object<string, Array>}
		 */
		const structs: StringMap<Object> = this.structs = {};

		this.iterateTree("struct", (name: string, types: Object) => structs[name] = types);
		return this.structs;
	}

	/**
	 * Finds and extracts `metadata` entries from the AST.
	 * @return {Object<string, object>} The extracted metadata.
	 */
	extractMetadata(): StringMap<Object> {
		/**
		 * A map of metadata entries.
		 * @type {Object<string, Object>}
		 */
		const metadata: StringMap<MetadataType> = this.metadata = {};

		if (!this.ast) throw new Error("AST not yet generated");

		const metas = this.ast.filter(([type]) => type == "metadata") as IRMetadata[];
		const graph: Graph<null> = new Graph<null>(metas.length, null);

		metas.filter(([, id]) => _.isNumber(id)).forEach(([, id, distinct, ...items]: IRMetadata) => {
			let recursive = false;
			let toAdd: any[] = [];

			items.forEach((item, i) => {
				if (item == id) {
					recursive = true;
				} else if (typeof item == "number") {
					graph.arc(id, item);
				} else {
					toAdd.push(item);
				}
			});

			metadata[id] = {recursive, distinct, items: toAdd};
		});

		graph.sortedDFS().forEach(({id}) => graph.getNodeSafe(id).out.forEach(dependency => {
			metadata[id].items = _.unionWith(metadata[id].items, metadata[dependency].items, _.isEqual);
		}));

		metas.filter(([, id]) => !_.isNumber(id)).forEach(([, id, distinct, ...items]: IRMetadata) => {
			metadata[id] = {
				recursive: false,
				distinct, 
				items: _.uniqWith(_.flatten(items.map(i => metadata[i].items)), _.isEqual)
			};
		});

		return metadata;
	}

	/**
	 * Finds and extracts global constant defintions from the AST.
	 * @return {Object} The extracted constant definitions.
	 */
	extractGlobalConstants(): StringMap<Object> {
		/**
		 * A map of global constants.
		 * @type {Object<string, Object>}
		 */
		const globalConstants: StringMap<Object> = this.globalConstants = {};

		this.iterateTree("global constant", (item: {name:string}) => globalConstants[item.name] = _.omit(item, "name"));
		return this.globalConstants;
	}

	/**
	 * Finds and extracts function declarations from the AST.
	 * @return {Object<string, IRDeclaration>} The extracted function declarations.
	 */
	extractDeclarations(): StringMap<DeclarationType> {
		/**
		 * A map of function declarations.
		 * @type {Object<string, Object>}
		 */
		const declarations: StringMap<DeclarationType> = this.declarations = {};
		
		this.iterateTree("declaration", (dec: ASTFunction) => {
			const [, {name, type, types, arity, unnamedAddr}] = dec;
			if (name in declarations) {
				warn("Duplicate declaration for", chalk.bold(name) + ".");
			}

			declarations[name] = {type, types, arity, isLocalUnnamed: unnamedAddr == "local_unnamed_addr"};
		});

		return declarations;
	}

	/**
	 * Finds and extracts function declarations from the AST.
	 * @return {Object<string, IRFunction>} The extracted function declarations.
	 */
	extractFunctions(): FunctionExtractions {
		const functions: StringMap<IRFunction> = {};
		const allBlocks: StringMap<BasicBlock> = {};
		const blockOrder: [string, BlockName][] = [];
		const functionOrder: string[] = [];

		this.iterateTree("function", (meta: LLVMFunctionMeta, instructions: (Instruction | LabelComment | Label)[]) => {
			// LLVM helpfully indicates basic blocks with comments and labels.
			// If we cheat, we can exploit this to avoid having to implement an actual basic block scanning algorithm.
			// (Even though it wouldn't be very difficult to implement such an algorithm.)
			const basicBlocks: BasicBlock[] = [];

			let exitBlock: BlockName | null = null;

			functionOrder.push(meta.name);
			
			// The first basic block is implicitly given a label whose name is equal to the function's arity.
			let currentBasicBlock: BasicBlock = [meta.arity, {preds: [], in: [], out: []}, []];

			for (const instruction of instructions) {
				if (isLabelComment(instruction)) {
					const [, name, preds] = instruction;
					basicBlocks.push(currentBasicBlock);
					currentBasicBlock = [name, {preds, in: [], out: []}, []];
				} else if (isLabel(instruction)) {
					const [, name] = instruction;
					basicBlocks.push(currentBasicBlock);
					currentBasicBlock = [name, {preds: [], in: [], out: []}, []];
				} else {
					exitBlock = currentBasicBlock[0];
					currentBasicBlock[2].push(instruction);
				}
			}

			if (currentBasicBlock[2].length) {
				basicBlocks.push(currentBasicBlock);
			}

			basicBlocks.forEach(block => {
				allBlocks[meta.name + ":" + block[0]] = block;
				blockOrder.push([meta.name, block[0]]);
			});
			
			let allVars: VariableName[] = [];

			functions[meta.name] = Object.assign(basicBlocks.map(block => {
				const computed = LL2W.computeBasicBlockVariables(block);
				allVars = [...allVars, ...computed[1].read, ...computed[1].written];
				return computed;
			}), {
				meta,
				vars: _.uniq(allVars),
				first: basicBlocks[0][0],
				exit: exitBlock
			});
		});

		return {functions, allBlocks, blockOrder, functionOrder};
	}

	static connectBlocks(functions: StringMap<IRFunction>, basicBlocks: BothMap<BasicBlock>, declarations = {}):
		void {

		for (const [fullName, block] of Object.entries(basicBlocks)) {
			const [funcName] = fullName.split(":");
			const [, oldMeta, instructions] = block;
			const last: Instruction = _.last(instructions);

			const meta = Object.assign(oldMeta, {unreachable: false});
			
			if (last[1] == "br_unconditional") {
				const destName = `${funcName}:${last[2].dest[1]}`;
				if (!(destName in basicBlocks)) {
					throw new Error(`Couldn't find a basic block called ${destName}.`);
				}

				const destBlock = basicBlocks[destName];
				meta.out.push(destName);
				destBlock[1].in.push(fullName);
			} else if (last[1] == "br_conditional") {
				// I'm assuming that conditional branches go only to other basic blocks within the same function.
				const {iftrue: [, iftrue], iffalse: [, iffalse]} = last[2];
				let trueName  = `${funcName}:${iftrue}`,
					falseName = `${funcName}:${iffalse}`;
				basicBlocks[trueName][1].in.push(fullName);
				basicBlocks[falseName][1].in.push(fullName);
				meta.out.push(trueName);
				meta.out.push(falseName);
			} else if (last[1] == "switch") {
				const imeta = last[2];
				const targets = [imeta.default, ...imeta.table.map(([,, v]) => v)];
				for (const [, target] of targets) {
					const tname = `${funcName}:${target}`;
					basicBlocks[tname][1].in.push(fullName);
					meta.out.push(tname);
				}
			} else if (last[1] == "unreachable") {
				meta.unreachable = true;
			}

			for (const instruction of instructions) {
				if (instruction[0] != "instruction") {
					continue;
				}

				if (instruction[1] == "call") {
					const iname = instruction[2].name;
					const arity = LL2W.getArity(functions, iname, declarations);
					const destName = `${iname}:${arity}`;
					if (LL2W.builtins.includes(iname)) {
						// Because builtins are single machine instructions and not real functions,
						// we don't include them in the control flow graph.
						continue;
					}

					meta.out.push(destName);
					if (destName in basicBlocks) {
						// First, make a link from this block to the start of the called function.
						const destBlock = basicBlocks[destName];
						destBlock[1].in.push(fullName);
						
						// Next, we link each return statement from the called function back to this block.
						// Depending on the exact circumstances, some of the returns (but not all) might not ever return
						// control to this block, but I imagine it would be extremely tricky to determine which do, so
						// just have to assume that all of them do. This might end up decreasing performance due to
						// overly cautious register allocation, but it's probably not a huge concern.
						const fn: IRFunction = functions[iname];
						for (const instr of fn) {
							const [calledName, {out: calledOut}, calledInstructions] = instr;

							const [lastType, lastName] = _.last(calledInstructions);
							if (lastType == "instruction" && lastName == "ret") {
								calledOut.push(fullName);
								meta.in.push(`${iname}:${calledName}`);
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

	static computeOperands(instruction: Instruction): ComputedOperands {
		const type: any = instruction[1]; // TODO
		const meta: any = instruction[2];
		let read: VariableName[] = [], written: VariableName[] = [], assigner: any = null; // TODO

		const tryRead = (x: any) => isASTVariable(x) && read.push(x[1]);

		if (["phi", "alloca", "conversion", "load", "binary", "icmp", "getelementptr"].includes(type)) {
			written.push(assigner = meta.destination[1]);
		}

		if (isPhi(instruction)) {
			// TODO: do phi instructions count as reads?
			meta.pairs.forEach(([v, src]) => tryRead(v));
		} else if (isSwitch(instruction)) {
			meta.table.forEach(([,, dest]) => tryRead(dest));
			tryRead(meta.operand);
			tryRead(meta.default);
		} else if (isSelect(instruction)) {
			written.push(assigner = meta.destination[1]);
			read.push(meta.condition[1]);
			[meta.leftValue, meta.rightValue].forEach(tryRead);
		} else if (isConversion(instruction)) {
			tryRead(meta.sourceValue)
		} else if (isCall(instruction)) {
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
		} else if (isStore(instruction)) {
			tryRead(meta.storeValue);
			
			if (isASTVariable(meta.destinationValue)) {
				// Yes, the store writes a value, but it doesn't write it to a variable.
				// Instead, it writes it to memoryat the position contained in the destination pointer.
				// That means it has two reads and no writes!
				read.push(assigner = meta.destinationValue[1]);
			}
		} else if (isLoad(instruction)) {
			tryRead(meta.pointerValue);
		} else if (isBinary(instruction) || isICMP(instruction)) {
			[meta.op1, meta.op2].forEach(o => isASTVariable(o) && read.push(o[1]));
		} else if (isBrCond(instruction)) {
			// TODO: do branch targets count as reads?
			if (typeof meta.cond != "number") {
				read.push(meta.cond[1]);
			}
		} else if (isRet(instruction)) {
			tryRead(meta.value);
		} else if (isGetElementPtr(instruction)) {
			tryRead(meta.pointerValue);
			meta.indices.filter(i => isASTVariable(i[1])).forEach(i => read.push(i[1][1]));
		}

		return {read, written, assigner};
	}

	static computeBasicBlockVariables(basicBlock: BasicBlock): BasicBlockExtra {
		let read: VariableName[] = [], written: VariableName[] = [], assigners: BothMap<Instruction> = {};

		// /*
		basicBlock[2].forEach(instruction => {
			const result = LL2W.computeOperands(instruction);
			read = read.concat(result.read);
			written = written.concat(result.written);
			if (result.assigner) assigners[result.assigner] = instruction;
		});

		return [basicBlock[0], Object.assign(basicBlock[1], {
			read: _.uniq(read),
			written: _.uniq(written),
			assigners: assigners,
			unreachable: null
		}), basicBlock[2]];
		// */
		// return basicBlock;
	}

	static getAllVariables(fn: IRFunction): VariableName[] {
		const {arity} = fn.meta;
		const out: VariableName[] = [];
		
		for (const [, {read, written}] of fn) {
			pushAll(out, [...read, ...written].filter(x => typeof x != "number" || arity < x));
		}
		
		return out;
	}

	static fromSameFunction(...blockNames: string[]): boolean {
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
	static computeCFG(fn: IRFunction): CFG {
		const out: CFG = new Graph(fn.length, {enter: -1, exit: -1, unreachable: []});
		const name = fn.meta.name;

		// Assign all blocks to the rest of the nodes.
		fn.forEach((block, i) => {
			out[i].data = {label: block[0]};

			if (block[0] == fn.first) {
				out.data.enter = i;
			}

			if (block[0] == fn.exit) {
				out.data.exit = i;
			}

			if (block[1].unreachable) {
				out.data.unreachable.push(i);
			}
		});

		// Add an arc from the block to each of its outblocks.
		fn.forEach((block: BasicBlockExtra, i: number) => {
			// Ignore outblocks that aren't within the same function.
			block[1].out
				.filter(s => s.toString().substr(0, s.toString().indexOf(":")) == name)
				.map(s => s.toString().substr(s.toString().indexOf(":") + 1))
				.map(s => out.findSingle((b: Node<{label: string}>) => b.data.label == s))
				.forEach(n => out[i].arc(n));
		});

		return out;
	}

	/**
	 * Computes the interference graph for a function.
	 * @param {IRFunction}  fn       An LLVM IR function as formed by {@link extractFunctions}.
	 * @param {CFG}         cfg      A control flow graph as computed by {@link computeCFG}.
	 * @param {LivenessMap} liveness A liveness map as computed by {@link computeLivenessForFunction}.
	 * @return {Graph} A graph with an extra `indexMap` property that maps instruction indices to the
	 *                 corresponding {@link Node} objects.
	 */
	static computeInterference(fn: IRFunction, cfg: CFG, liveness: LivenessMap): Graph<never> {
		// An array that contains all the IRInstructions in the function.
		// for (const block of fn) {
		// 	for (const instr of block[2]) {
		// 		instr[2].block = block;
		// 	}
		// }

		// const instructions = _.flatten(fn);
		
		// Create an empty interference graph, then add all the instructions to the graph while making a note of all
		// phi instructions in an array. The array contains tuples of the instruction node and the phi pairs, which are
		// in the form [variable name, source block name].
		// The graph node data is in the form [instruction index, instruction type, instruction metadata].
	// 	const graph = new Graph(fn.length);
	// 	const phis = [];
	// 	for (let i = fn.length - 1; 0 <= i; --i) {
	// 		const instruction = fn[i];
	// 		const [type, meta] = instruction;
			
	// 		const node = graph.add([i, type, meta]);

	// 		if (type == "phi") {
	// 			phis.unshift([node, meta.pairs.map(pair => pair.map(([, varName]) => varName))]);
	// 		}
	// 	}

		return null;
	}

	/**
	 * Computes an object that indicates in which blocks of a function a given variable used in that function
	 * is live-in/live-out.
	 * @param {IRFunction} fn An LLVM IR function as formed by {@link extractFunctions}.
	 * @return {Object} A map of variable names to maps of block names to tuples of whether the variable is live-in in
	 *                  the block and whether the variable is live-out in the block.
	 */
	static computeLivenessForFunction(fn: IRFunction): LivenessMap {
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

	static getReaders(fn: IRFunction, varName: VariableName): IRFunctionBlock[] {
		const varString: string = varName.toString();
		return fn.filter(([id, {read}]) => read.map(x => x.toString()).includes(varString));
	}

	static getWriters(fn: IRFunction, varName: VariableName): IRFunctionBlock[] {
		const varString: string = varName.toString();
		return fn.filter(([id, {written}]) => written.map(x => x.toString()).includes(varString));
	}

	static getUsefulLivenessData(cfg: CachingCFG,
		ignore: {dt?: boolean, dj?: boolean, ms?: boolean} = {dt: false, dj: false, ms: false}): UsefulLivenessData {

		let dj = cfg._djGraph;
		let dt = cfg._dTree;
		let ms = cfg._mergeSets;
		const forceAll = !ms && !ignore.ms;

		if (forceAll || !dt && !ignore.dt) {
			dt = cfg._dTree = cfg.dTree(cfg.data.enter);
		}
		
		if (forceAll || !dj && !ignore.dj) {
			dj = cfg._djGraph = cfg.djGraph(dt);
		}
		
		if (forceAll) {
			ms = cfg._mergeSets = _.mapKeys(_.mapValues(Graph.mergeSets(dj, cfg.data.enter, cfg.data.exit),
			                                            value => value.map(id => dj.nodes[id].data.label)),
			                                (value, key) => dj.nodes[key].data.label);
		}

		return {djGraph: dj, dTree: dt, mergeSets: ms};
	}

	static invalidateUsefulLivenessData(cfg: CachingCFG): void {
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
	static isLiveInUsingMergeSet(fn: IRFunction, cfg: CachingCFG, block: IRFunctionBlock, varName: VariableName): boolean {
		varName = varName.toString();
		const {dTree, mergeSets} = LL2W.getUsefulLivenessData(cfg, {dj: true});
		if (block === undefined) {
			debugger;
		}

		const n = block[0];
		const blockDebug = n == fn.meta.arity.toString()? chalk.magenta("block " + n) : "block " + chalk.bold(n);
		debug(`Computing live-in for ${chalk.bold(varName)} in ${blockDebug}.`);

		const originalMergeSet = mergeSets[n];
		const modifiedMergeSet: NodeID[] = _.uniq([...originalMergeSet, varName].map(x => x.toString()));
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
				const parent = dTree.nodes[dTree.findSingle(node =>
					("label" in (node.data as any)) && (node.data as {label: string}).label == t).in[0]].data.label;
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
					debug(`Finished computing live-out for ${chalk.bold(varName)} in ${blockDebug}:`,
					      `${chalk.bold("true")} because t ∩ M_s(n).`);
					return true;
				}

				// t = dom-parent(t)
				const parent = dTree.nodes[dTree.findSingle(node =>
					("label" in (node.data as any)) && (node.data as {label: string}).label == t).in[0]].data.label;
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

	static getArity(functions: {[key: string]: IRFunction}, functionName: string,
	                declarations: {[key: string]: DeclarationType} = {}): number {
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
	debug(fn: Function): void {
		this.options.debug && fn(this);
	}

	/**
	 * Prints a message to stderr and exits the process with return code 1.
	 * @param {...*} args - The arguments to pass to `console.error`.
	 */
	static die(...args: any[]): void {
		console.error(...args);
		process.exit(1);
	}

	static async produceIR(dirName: string, ...filenames: string[]): Promise<void> {
		let deleteDir = false;
		if (!dirName) {
			let {stdout, stderr} = await exec("mktemp -d");
			dirName = stdout.trim();
			deleteDir = true;
		}

		console.log(`Using ${chalk.bold(dirName)}.`);
		
		const resolved = filenames.map(f => path.resolve(f));
		const cmd = shell_escape(["clang", "-S", "-emit-llvm", ...resolved]);

		process.chdir(dirName);

		let {stdout, stderr} = await exec(cmd);
		
		// console.log({stdout, stderr});

		if (deleteDir) {
			rimraf.sync(dirName);
		}
	}
}

// LL2W.builtins = ["_int", "_rit", "_time", "_gettime", "_halt", "_prc", "_prd", "_prx"];

module.exports = LL2W;

let debug = (...a: any[]): void => {};

if (require.main === module) {
	(() => {
		const options = minimist(process.argv.slice(2), {
			alias: {d: "debug", c: "cfg"},
			boolean: ["debug"],
			default: {debug: false}
		}), infile = options._[0];

		if (options.debug) {
			debug = (...a: any[]) => console.log("🐞", ...a);
		}

		if (!infile) {
			console.log("Usage: ./ll2w.js <filename> [out]");
			process.exit(0);
		}

		let outfile = options._[1] || infile.replace(/\.ll$/, "") + ".why";
		const compiler = new LL2W(options as LL2WOptions);

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

		compiler.extractInformation();
		compiler.extractAttributes();
		compiler.extractStructs();
		compiler.extractMetadata();
		const declarations = compiler.extractDeclarations();
		compiler.extractGlobalConstants();
		const {functions, allBlocks, blockOrder, functionOrder} = compiler.extractFunctions();
		LL2W.connectBlocks(functions, allBlocks, declarations);

		if (options.cfg) {
			if (!(options.cfg in functions)) {
				console.error(`Can't print CFG: couldn't find function ${chalk.bold(options.cfg)}.`);
				process.exit(1);
			}

			return LL2W.computeCFG(functions[options.cfg]).display();
		}

		console.log();

		// testInterference(functions, allBlocks, declarations);
		if ("liveness" in functions) {
			testLiveness(functions.liveness, declarations);
		}
	})();
}

function testInterference(functions, allBlocks, declarations) {
	// Expecting disassemble.ll (dtest).
	const fn = functions.wvm_disassemble_r_alt_op;
	
	const cfg = LL2W.computeCFG(fn);
	const liveness = LL2W.computeLivenessForFunction(fn);

	console.log("Interference:");
	console.log(LL2W.computeInterference(fn, cfg, liveness));
}

function testLiveness(fn, declarations) {
	// console.log(Object.keys(functions).map(key => [key, functions[key].length]));
	// LL2W.computeCFG(fn, declarations).display({width: 1000, height: 500}).then(() => console.log());
	// LL2W.computeCFG(fn, declarations).display({width: 4000*1, height: 1000*1}).then(() => console.log());
	const cfg = LL2W.computeCFG(fn);
	const dj = cfg.djGraph(cfg.data.enter);
	const ms = Graph.mergeSets(dj, cfg.data.enter, cfg.data.exit);
	const blockID = "8";
	const varName = "w";
	const block = fn.filter(([l]) => l == blockID)[0];
	// console.log(`isLiveOut(${blockID}, ${varName}):`, LL2W.isLiveOutUsingMergeSet(fn, cfg, block, varName));
	// console.log("liveness set:", compiler.computeLivenessSet(functions, allBlocks));
	// console.log("liveness sets:");
	// console.log(LL2W.computeLivenessForFunction(fn));
	return;
}

function testReversePost() {
	let cfg;
	// cfg = new Graph(6).arcString("AB BC BD BF CE DE EB");
	
	cfg = new Graph(7, null);
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
	const dj254gap = new Graph(24, null);
	const pairs254gapLtR = "01 02 23 34 35 3-23 38 j45 56 57 j23-5 j23-8 j67 j75 j78 89 8-10 8-14 j9-10 14-15 14-16 j15-16 10-11 11-12 12-13 j13-1 16-22 j22-10 16-17 17-21 j21-22 17-18 18-19 18-20 j19-20 j20-18 j20-21".split(" ");
	const pairs254gapSorted = "01 02 23 34 35 38 3-23 j45 56 57 j23-5 j23-8 j67 j75 j78 89 8-10 8-14 j9-10 14-15 14-16 j15-16 10-11 11-12 12-13 j13-1 16-17 j22-10 16-22 17-18 j21-22 17-21 18-19 18-20 j19-20 j20-18 j20-21".split(" ");
	const pairs254gap = pairs254gapLtR;

	const jpairs254gap = pairs254gap.filter(s => s[0] == "j").map(s => s.substr(1));
	const jedges254gap = jpairs254gap.map(s => s.includes("-")? s.split(/-/).map(n => parseInt(n)) : [parseInt(s[0]), parseInt(s[1])]);
	const str254gap = pairs254gap.filter(s => s[0] != "j").join(" ");
	// console.log({jpairs254gap, jedges254gap, str254gap});
	dj254gap.arcString(str254gap);
	dj254gap.data.jEdges = jedges254gap;
	// console.log("dj254gap:", dj254gap);
	// console.log("dj254gap:\n" + dj254gap.toString());

	// dj254gap.display();
	
	console.log(chalk.dim("Calculating merge sets."));
	// const ms = cfg.mergeSets();
	const ms = Graph.mergeSets(dj254gap, 0, 1);
	console.log(chalk.dim("\nResults:"));
	console.log(_.keys(ms).filter(k => ms[k].length).map(k => `${chalk.green(k.padStart(3, " "))}: ${[...ms[k]].sort().map(x => chalk.yellow(x)).join(", ")}`).join("\n"));
	// console.log(cfg.mergeSets());
}

function testMisc1() {
	// cfg = new Graph(8).arcString("AB BC BD CE DE EF FG GF GH");
	const cfg = new Graph(13, null);
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
