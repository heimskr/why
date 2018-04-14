#!/usr/bin/env node
let fs = require("fs"),
	chalk = require("chalk"),
	minimist = require("minimist"),
	Long = require("long"),
	WASMC = require("./wasmc.js"),
	Parser = require("../wvm/parser.js");

/**
 * @module wasm
 */

require("../util.js");
require("string.prototype.padstart").shim();
require("string.prototype.padend").shim();

/**
 * Represents a `wld` instance.
 */
class Linker {
	constructor(options, filename, libs, out) {
		this.options = options;
		this.filename = filename;
		this.libraryNames = libs;
		this.libraries = { };
		this.out = out;
		this.codeOffset = 0;
		this.labels = { };
	}

	link() {
		this.loadMain();
		this.concatenateData();
		const expanded = this.readjustCode();
		this.mergeLabels();
		this.compiler.processHandlers();
		this.compiler.processCode(this.compiler.expandLabels(expanded));
		this.concatenateCode();

		fs.writeFileSync(this.options.out, WASMC.longs2strs(this.finalizeOutput()).join("\n"));

		console.log(`${chalk.green("\u2714")} Successfully linked ${chalk.bold(this.filename)} and saved the output to ${chalk.bold(this.options.out)}.`);
	}

	loadMain() {
		let wasmc = this.compiler = new WASMC({ library: false }, this.filename);
		
		wasmc.parse();
		wasmc.processMetadata();
		wasmc.processData();

		this.dataOffset = wasmc.meta[1].toInt();
		this.codeOffset = wasmc.meta[2].toInt();
		this.data = wasmc.data;

		return this;
	}

	concatenateData() {
		if (!(this.libraryNames instanceof Array)) {
			return false;
		}

		this.libraryNames.forEach((lib) => {
			let parser = Parser.open(lib);
			let {$data, $code} = parser.parsed.offsets;
			this.libraries[lib] = parser;
			parser.dataStart = this.dataOffset + this.data.length * 8;
			this.data = this.data.concat(parser.raw.slice($data / 8, $code / 8));
		});

		this.codeOffset = this.dataOffset + this.data.length * 8;
		return this;
	}

	readjustCode() {
		let expanded = this.compiler.expandCode();
		let codeStart = this.codeOffset + expanded.length * 8;

		this.libraryNames.forEach((lib) => {
			const library = this.libraries[lib];
			const labels = library.data.labels;
			const {$data, $code} = library.parsed.offsets;

			const getType = (addr) => {
				if ($data <= addr && addr < $code) {
					return "data";
				} else if ($code <= addr) {
					return "code";
				} else {
					throw new Error(`Unknown address type for ${addr}`);
				}
			};

			const adjust = (addr) => {
				const type = getType(addr);
				if (type == "data") {
					return addr - $data + library.dataStart;
				} else if (type == "code") {
					return addr - $code + codeStart;
				}
			};

			library.code = [];
			library.parsed.code.forEach((instruction) => {
				if (instruction.flags == 1) {
					let addr;

					if (instruction.type == "j") {
						addr = instruction.addr;
					} else if (instruction.type == "i") {
						addr = instruction.imm;
					} else {
						this.warn("Flag detected for R-type instruction:", instruction);
					}

					instruction[instruction.type == "j"? "addr" : "imm"] = adjust(addr);
					instruction.flags = 0;
				}

				library.code.push(this.compiler.unparseInstruction(instruction));
			});

			Object.keys(labels).forEach((label) => {
				if (typeof this.labels[label] != "undefined") {
					this.warn("Redefining label:", label);
				}

				this.labels[label] = adjust(labels[label]);
				this.log(`${chalk.cyan("Adjusting")} ${label} from ${chalk.bold(labels[label])} to ${chalk.bold(this.labels[label])}.`);
			});

			codeStart += library.code.length * 8;
		});

		return expanded;
	}

	// Adjusts the labels from the compiler and merges them with the adjusted labels gathered from the libraries.
	mergeLabels() {
		const $code = this.compiler.meta[2].toInt();
		Object.keys(this.compiler.offsets).forEach((name) => {
			const offset = this.compiler.offsets[name];
			if ($code <= offset) {
				this.compiler.offsets[name] = offset - $code + this.codeOffset;
			}
		});

		Object.keys(this.labels).forEach((name) => {
			if (typeof this.compiler.offsets[name] != "undefined") {
				this.warn("Label merge conflict:", name);
			}

			this.compiler.offsets[name] = this.labels[name];
		});
	}

	concatenateCode() {
		this.libraryNames.forEach((lib) => this.compiler.code = this.compiler.code.concat(this.libraries[lib].code));
	}

	finalizeOutput() {
		const compiler = this.compiler;
		compiler.meta[2] = Long.fromInt(this.codeOffset, true);
		compiler.meta[3] = Long.fromInt([compiler.meta, compiler.handlers, this.data, compiler.code].reduce((a, b) => a + 8*b.length, 0), true);
		return compiler.meta.concat(compiler.handlers).concat(this.data).concat(compiler.code);
	}

	warn(...args) {
		console.warn(...args);
	}

	log(...args) {
		if (this.options.debug) {
			console.log(...args);
		}
	}
}

module.exports = Linker;

if (require.main === module) {
	const options = minimist(process.argv.slice(2), {
			alias: {
				o: "out",
				d: "debug",
			},
			boolean: ["debug"],
			default: {
				debug: false 
			}
		}), filename = options._[0];

	if (!filename || options._.length < 2 || !options.out) {
		console.log("Usage: node wld.js [main.wasm] ...[library.wlib] -o out");
		process.exit(0);
	}

	new Linker(options, options._[0], options._.slice(1), options.out).link();
}
