#!/usr/bin/env node
let fs = require("fs"),
	chalk = require("chalk"),
	minimist = require("minimist"),
	Long = require("long"),
	WASMC = require("./wasmc.js"),
	Parser = require("../wvm/parser.js"),
	_ = require("lodash");

const {FLAGS} = require("./constants.js");

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

		this.parser = null;
		this.symbolTable = {};
	}

	link() {
		if (this.objectFilenames.length < 2) {
			console.error(`${chalk.yellow("?")} Multiple input files are needed.`);
			process.exit(1);
		}

		// Step 1
		this.parser = new Parser();
		this.parser.open(this.objectFilenames[0]);

		const {raw, parsed} = this.parser;
		
		// Step 2
		const codeLength = this.parser.getCodeLength();
		const dataLength = this.parser.getDataLength();
		const mainSymbols = this.parser.getSymbols();
		this.symbolTable = _.cloneDeep(mainSymbols);
		
		// Step 3
		this.desymbolize(this.parser.rawCode, mainSymbols);

		// Steps 4–6
		let extraSymbolLength = this.symbolTable.length;
		let extraCodeLength = codeLength;
		let extraDataLength = dataLength;
	}

	/**
	 * Converts the imm/addr values of the I-/J-type instructions in a list of Longs to their symbol representations
	 * @param {Long} longs An array of compiled code.
	 * @param {Object<string, Array<number, Long>>} symbolTable An object mapping a symbol name to tuple of its ID and its address.
	 */
	desymbolize(longs, symbolTable) {
		for (let i = 0; i < longs.length; i++) {
			const parsedInstruction = Parser.parseInstruction(longs[i]);
			const {opcode, type, flags, rs, rd, imm, addr} = parsedInstruction;
			if (flags == FLAGS.KNOWN_SYMBOL) {
				if (type != "i" && type != "j") {
					throw `Found an instruction not of type I or J with \x1b[1mKNOWN_SYMBOL\x1b[22m set at \x1b[1m0x${i*8 + this.parser.offsets.$code}\x1b[22m.`;
				}

				const name = this.findSymbolFromAddress(type == "i"? imm : addr);
				if (!name || !symbolTable[name]) {
					throw `Couldn't find a symbol corresponding to \x1b[0m\x1b[1m${imm}\x1b[0m.`;
				}

				const id = symbolTable[name][0];
				// We disable warnings because the encoded IDs are somehow negative (even if I add a Math.abs() call in WASMC.encodeSymbol, strangely).
				if (type == "i") {
					longs[i] = WASMC.iType(opcode, rs, rd, id, FLAGS.SYMBOL_ID, false);
				} else {
					longs[i] = WASMC.jType(opcode, rs, id, FLAGS.SYMBOL_ID, false);
				}
			}
		}
	}

	/**
	 * Finds a symbol name based on its ID.
	 * @param  {number} id A numeric ID.
	 * @return {?string} A symbol name if one was found; `null` otherwise.
	 */
	findSymbolFromID(id) {
		for (let name of Object.keys(this.symbolTable)) {
			if (this.symbolTable[name][0] == id) {
				return name;
			}
		}

		return null;
	}

	/**
	 * Finds a symbol name based on its address.
	 * @param  {number} addr An address.
	 * @return {?string} A symbol name if one was found; `null` otherwise.
	 */
	findSymbolFromAddress(addr) {
		for (let name of Object.keys(this.symbolTable)) {
			if (this.symbolTable[name][1].eq(addr)) {
				return name;
			}
		}

		if (addr == this.parser.offsets.$end) {
			return ".end";
		}

		return null;
	}

	/**
	 * Writes the output of the linking process to a file.
	 * @param {Long[]} longs The final linked output as an array of Longs.
	 * @param {string} [outfile] A filename (`options.out` if not specified).
	 */
	writeOutput(longs, outfile=this.options.out) {
		fs.writeFileSync(outfile, WASMC.longs2strs(longs).join("\n"));
	}

	/**
	 * Prints a message to the console that indicates the linking was successful.
	 * @param {string} infile The input filename.
	 * @param {string} outfile The output filename.
	 */
	printSuccess(infile, outfile) {
		console.log(`${chalk.green("\u2714")} Successfully linked ${chalk.bold(infile)} and saved the output to ${chalk.bold(outfile)}.`);
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
