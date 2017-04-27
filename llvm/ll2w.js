#!/usr/bin/env node
let minimist = require("minimist"),
	fs = require("fs"),
	chalk = require("chalk"),
	getline = require("get-line-from-pos"),
	nearley = require("nearley");

const { displayIOError } = require("../util.js");

/**
 * `ll2w` is an LLVM intermediate representation to WVM compiler (thus
 * <code><b>ll</b>vm<b>2w</b>vm</code>) written from scratch.
 * 
 * @module llvm
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
		 */
		this.options = options;
	};

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
			};

			process.exit(1);
		};

		/**
		 * The LLVM IR parser.
		 * @type {nearley.Parser}
		 */
		this.parser = new nearley.Parser(this.grammar.ParserRules, this.grammar.ParserStart);
	};

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
			};

			process.exit(1);
		};

		if (trees.length > 1) {
			trees.forEach((tree) => console.log(JSON.stringify(trees[tree], null, 4)));
			console.error(chalk.red.italic(`\nAmbiguous grammar (${trees.length}).\n`));
			process.exit(1);
		} else if (trees.length == 0) {
			console.error(chalk.red.italic("Nothing was parsed."));
			process.exit(1);
		};

		/**
		 * The abstract syntax tree of the LLVM IR.
		 * @type {Array}
		 */
		this.ast = trees[0];

		this.debug(() => console.timeEnd("parse"));
		if (typeof this.ast != "object") {
			LL2W.die("Error: parser output isn't an object.");
		};
	};

	/**
	 * Iterates the AST and executes a function for every top-level entry of a given type.
	 * @param {?string} type - The type of entry to iterate for. Entries not of this type will be
	 * ignored. If null, all entries will be iterated for.
	 * @param {function} fn - The function to call on each iteration.
	 */
	iterateTree(type, fn) {
		if (!this.ast) {
			throw new Error("AST not yet generated");
		};

		if (type == null) {
			this.ast.forEach((entry) => fn(...entry));
		} else {
			this.ast.forEach(([name, ...args]) => name == type && fn(...args));
		};
	};

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
		 * @type {Object.<string, string>}
		 */
		this.targets = { };

		this.iterateTree("source_filename", (name) => this.sourceFilename = name);
		this.iterateTree("target", (key, value) => this.targets[key] = value);
	};

	/**
	 * Finds and extracts `attributes` entries from the AST.
	 */
	extractAttributes() {
		/**
		 * A map of attribute definitions.
		 * @type {Object.<string, Array>}
		 */
		this.attributes = { };

		this.iterateTree("attributes", (name, attrs) => this.attributes[name] = attrs);
	};

	/**
	 * Finds and extracts `struct` entries from the AST.
	 */
	extractStructs() {
		/**
		 * A map of struct definitions.
		 * @type {Object.<string, Array>}
		 */
		this.structs = { };

		this.iterateTree("struct", (name, types) => this.structs[name] = types);
	};

	/**
	 * Finds and extracts `metadata` entries from the AST.
	 */
	extractMetadata() {
		/**
		 * A map of metadata entries.
		 * @type {Object.<string, Object>}
		 */
		this.metadata = { };

		this.ast.filter(([type, name]) => type == "metadata" && typeof name == "number")
			.sort((a, b) => b[1] < a[1])
			.forEach(([, name, distinct, ...items]) => {
				let recursive = false;
				let toAdd = [];
				console.log({items});
				items.forEach((item) => {
					if (item == name) {
						recursive = true;
					} else if (typeof item == "number") {
						const meta = this.metadata[item];
						if (typeof meta == "undefined") {
							console.warn(`Nonexistent metadata entry "${item}" was referenced by "${name}".`);
						} else {
							toAdd = toAdd.concat(meta.items);
						};
					} else {
						toAdd.push(item);
					};
				});

				this.metadata[name] = { recursive, items };
			});
	};

	/**
	 * Executes a function if options.debug is true. Does nothing otherwise.
	 * @param {function} fn - The function to execute.
	 **/
	debug(fn) {
		if (this.options.debug) {
			fn(this);
		};
	};

	/**
	 * Prints a message to stderr and exits the process with return code 1.
	 * @param {...*} args - The arguments to pass to `console.error`.
	 */
	static die(...args) {
		console.error(...args);
		process.exit(1);
	};
};

module.exports = LL2W;

if (require.main === module) {
	const options = minimist(process.argv.slice(2), {
		alias: { d: "debug" },
		boolean: ["debug"],
		default: { d: false }
	}), infile = options._[0];
	let outfile = options._[1] || infile.replace(/\.ll$/, "") + ".why";
	const compiler = new LL2W(options);

	if (!outfile) {
		outfile = infile.replace(/\.ll$/, "") + ".why";
	};

	try {
		compiler.feed(fs.readFileSync(infile, "utf8"));
	} catch(e) {
		console.log(e);
		displayIOError(e, infile);
		process.exit(1);
	};

	compiler.extractInformation();
	compiler.extractAttributes();
	compiler.extractStructs();
	compiler.extractMetadata();

	compiler.debug(() => require("jsome")({
		sourceFilename: compiler.sourceFilename,
		targets: compiler.targets,
		attributes: compiler.attributes,
		structs: compiler.structs,
		metadata: compiler.metadata
	}));
};
