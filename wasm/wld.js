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
	constructor(options, objects, out) {
		this.options = options;
		this.objectFilenames = objects;
		this.outputFilename = out;
	}

	link() {
		const main = new Parser();
		main.open(this.objectFilenames[0]);

		const {raw, parsed} = main;
		const codeLength = main.getCodeLength();
		const dataLength = main.getDataLength();
		const mainSymbols = main.getSymbols();

		// fs.writeFileSync(this.options.out, WASMC.longs2strs(this.finalizeOutput()).join("\n"));
		// console.log(`${chalk.green("\u2714")} Successfully linked ${chalk.bold(this.filename)} and saved the output to ${chalk.bold(this.options.out)}.`);
	}

	/**
	 * Converts the imm/addr values of the I-/J-type instructions in a list of Longs to their symbol representations
	 * @param {Long} longs An array of compiled code.
	 * @param {Object<string, Array<number, Long>>} symbolTable An object mapping a symbol name to tuple of its ID and its address.
	 */
	desymbolize(longs, symbolTable) {
		for (let i = 0; i < longs.length; i++) {
			
		}
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
	});

	if (options._.length == 0 || !options.out) {
		console.log("Usage: node wld.js ...[compiled.why] -o out");
		process.exit(0);
	}

	try {
		new Linker(options, options._, options.out).link();
	} catch(e) {
		if (typeof e == "string") {
			console.error(chalk.red(e));
		} else {
			process.stdout.write("\x1b[31m");
			console.error(e);
			process.stdout.write("\x1b[0m");
		}

		process.exit(1);
	}
}
