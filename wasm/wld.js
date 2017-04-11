#!/usr/bin/env node
let fs = require("fs"),
	chalk = require("chalk"),
	minimist = require("minimist"),
	Long = require("long"),
	_ = require("lodash"),
	WASMC = require("./wasmc.js"),
	Parser = require("../wvm/parser.js"),
	colorize = require("json-colorizer");

require("../util.js");
require("string.prototype.padstart").shim();
require("string.prototype.padend").shim();

const { EXCEPTIONS, R_TYPES, I_TYPES, J_TYPES, OPCODES, FUNCTS, REGISTER_OFFSETS, MAX_ARGS, FLAGS } = require("./constants.js");
const isLabelRef = (x) => x instanceof Array && x.length == 2 && x[0] == "label";
 
class Linker {
	constructor(options, filename, libs, out) {
		this.options = options;
		this.filename = filename;
		this.libraryNames = libs;
		this.libraries = { };
		this.out = out;
		this.codeOffset = 0;
		this.labels = { };

		this.data = [];
	};

	link() {
		this.loadMain();
		this.concatenateData();
		this.concatenateCode();
	};

	loadMain() {
		let wasmc = this.compiler = new WASMC({ library: false }, this.filename);
		
		wasmc.parse();
		wasmc.processMetadata();
		wasmc.processData();

		this.dataOffset = wasmc.meta[1].toInt();
		this.codeOffset = wasmc.meta[2].toInt();
		this.data = this.data.concat(wasmc.data);

		return this;
	};

	concatenateData() {
		if (!(this.libraryNames instanceof Array)) {
			return false;
		};

		this.libraryNames.forEach((lib) => {
			let parser = Parser.open(lib);
			let { $handlers, $data, $code, $end } = parser.parsed.offsets;
			this.libraries[lib] = parser;
			parser.dataStart = this.dataOffset + this.data.length;
			this.data = this.data.concat(parser.raw.slice($data, $code));
		});

		this.codeOffset = this.dataOffset + this.data.length
		return this;
	};

	concatenateCode() {
		let expanded = this.compiler.expandCode();
		let codeStart = this.codeOffset + expanded.length;
		let dataStart;

		this.libraryNames.forEach((lib) => {
			console.log(`\n${chalk.magenta.bold(lib)} (${chalk.bold(codeStart)})\n`);
			const library = { dataStart } = this.libraries[lib];
			const libcode = library.parsed.code;
			const labels = library.data.labels;
			let { $handlers, $data, $code, $end } = library.parsed.offsets;

			const getType = (addr) => {
				if ($data <= addr && addr < $code) {
					return "data";
				} else if ($code <= addr) {
					return "code";
				} else {
					return "?";
				};
			};

			const getName = (addr) => Object.keys(labels).filter((key) => labels[key] == addr)[0] || "???";

			libcode.forEach((instruction, i) => {
				if (instruction.flags != 1) {
					return;
				};

				console.log(chalk.yellow("instruction:"), instruction);

				let addr = instruction.type == "j"? instruction.addr : instruction.imm;
				console.log(chalk.green("addr:"), chalk.red(addr), chalk.green("name:"), chalk.red(getName(addr)), chalk.green("type:"), chalk.red(getType(addr)), "\n");
			});

			codeStart += libcode.length;
		});
	};
};

module.exports = Linker;

if (require.main === module) {
	const options = minimist(process.argv.slice(2), {
		alias: {
			o: "out",
		},
		boolean: [],
		default: { }
	}), filename = options._[0];

	if (!filename || options._.length < 2 || !options.out) {
		console.log("Usage: node wld.js [main.wasm] ...[library.wlib] -o out");
		process.exit(0);
	};

	new Linker(options, options._[0], options._.slice(1), options.out).link();
};
