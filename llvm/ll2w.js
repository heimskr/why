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
			trees.forEach((tree) => console.log(JSON.stringify(trees[tree], null, 4)));
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
			this.ast.forEach((entry) => fn(...entry));
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

		this.iterateTree("source_filename", (name) => this.sourceFilename = name);
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

		graph.sorted().forEach(({ id: name }) => graph.getNode(name).out.forEach((dependency) => {
			this.metadata[name].items = _.unionWith(this.metadata[name].items, this.metadata[dependency].items, _.isEqual);
		}));

		metas.filter(([, name]) => !_.isNumber(name)).forEach(([, name, distinct, ...items]) => {
			this.metadata[name] = {
				recursive: false,
				distinct, 
				items: _.uniqWith(_.flatten(items.map((i) => this.metadata[i].items)), _.isEqual)
			};
		});
	}

	/**
	 * Finds and extracts global constant defintions from the AST.
	 */
	extractGlobalConstants() {
		const constants = {};
		this.iterateTree("global constant", (item) => constants[item.name] = _.omit(item, "name"));
		return constants;
	}

	extractFunctions() {
		const functions = {};
		const allBlocks = {};

		this.iterateTree("function", (meta, instructions) => {
			const basicBlocks = [];
			let currentBasicBlock = ["start", {preds: [], in: [], out: []}, []];

			for (const instruction of instructions) {
				const [name, ...args] = instruction;
				if (name == "label_c") {
					basicBlocks.push(currentBasicBlock);
					currentBasicBlock = [args[0], {preds: args[1], in: [], out: []}, []];
				} else {
					currentBasicBlock[2].push(instruction);
				}
			}

			if (currentBasicBlock[2].length) {
				basicBlocks.push(currentBasicBlock);
			}

			functions[meta.name] = basicBlocks.map(this.extractBasicBlockVariables);
			basicBlocks.forEach(([name, ...etc]) => allBlocks[meta.name + ":" + name] = etc);
		});

		return [functions, allBlocks];
	}

	connectBlocks(functions, basicBlocks) {
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
					const destName = `${iname}:start`;
					if (LL2W.builtins.includes(iname)) {
						// Because builtins are single machine instructions and not real functions,
						// we don't include them in the control flow graph.
						continue;
					}

					_.push(meta.out, destName);
					if (!(destName in basicBlocks)) {
						console.warn(`Warning: couldn't find a basic block called ${destName}.`);
					} else {
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
					}
				}
			}
		}

		Object.keys(basicBlocks).forEach(k => console.log("\n" + chalk.bold(k), "in:", basicBlocks[k][0].in, "out:", basicBlocks[k][0].out));
		// console.log(functions);
	}

	extractBasicBlockVariables(basicBlock) {
		let read = [], written = [], assigners = {};
		for (const instruction of basicBlock[2]) {
			const [, type, meta] = instruction;
			if (type == "phi" || type == "alloca") {
				// TODO: do phi instructions count as reads?
				written.push(meta.destination[1]);
				assigners[meta.destination[1]] = instruction;
			} else if (["conversion"].includes(type)) {
				written.push(meta.destination[1]);
				assigners[meta.destination[1]] = instruction;

				if (meta.sourceValue[0] == "variable") {
					read.push(meta.sourceValue[1]);
				}
			} else if (type == "call") {
				if (meta.assign) {
					written.push(meta.assign[1]);
					assigners[meta.assign[1]] = instruction;
				}

				for (const arg of meta.args) {
					if (arg[1] && arg[1][0] == "variable") {
						read.push(parseInt(arg[1][1]));
					}
				}
			} else if (type == "store") {
				if (meta.storeValue[0] == "variable") {
					read.push(meta.storeValue[1]);
				}
				
				if (meta.destinationValue[0] == "variable") {
					written.push(meta.destinationValue[1]);
					assigners[meta.destinationValue[1]] = instruction;
				}
			} else if (type == "load") {
				written.push(meta.destination[1]);
				assigners[meta.destination[1]] = instruction;
				
				if (meta.pointerValue[0] == "variable") {
					read.push(meta.pointerValue[1]);
				}
			} else if (type == "binary" || type == "icmp") {
				written.push(meta.destination[1]);
				assigners[meta.destination[1]] = instruction;

				[meta.op1, meta.op2].forEach((o) => o[0] == "variable" && read.push(o[1]));
			} else if (type == "br_conditional") {
				// TODO: do branch targets count as reads?
				read.push(meta.cond[1]);
			} else if (type == "ret") {
				if (meta.value && meta.value[0] == "variable") {
					read.push(meta.value[1]);
				}
			} else if (type == "getelementptr") {
				written.push(meta.destination[1]);
				assigners[meta.destination[1]] = instruction;

				if (meta.pointerValue[0] == "variable") {
					read.push(meta.pointerValue[1]);
				}
				
				meta.indices.filter(i => i[1][0] == "variable").forEach(i => read.push(i[1][1]));
			}
		}
		
		basicBlock[1].read = _.uniq(read);
		basicBlock[1].written = _.uniq(written);
		basicBlock[1].assigners = assigners;
		return basicBlock;
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
		
		const resolved = filenames.map((f) => path.resolve(f));
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
		alias: { d: "debug" },
		boolean: ["debug"],
		default: { d: false }
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

	compiler.extractInformation();
	compiler.extractAttributes();
	compiler.extractStructs();
	compiler.extractMetadata();
	compiler.globalConstants = compiler.extractGlobalConstants();
	[compiler.functions, compiler.allBlocks] = compiler.extractFunctions();
	compiler.connectBlocks(compiler.functions, compiler.allBlocks);

	0&&compiler.debug(() => jsome({
		sourceFilename: compiler.sourceFilename,
		targets: compiler.targets,
		attributes: compiler.attributes,
		structs: compiler.structs,
		metadata: compiler.metadata,
		constants: compiler.globalConstants,
	}));

	// console.log(compiler.ast);
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
