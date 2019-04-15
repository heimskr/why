#!/usr/bin/env ts-node
import * as fs from "fs";
import {Grammar, Parser, CompiledRules} from "nearley";
import * as Long from "long";
import _ from "../util";
import {createHash} from "crypto";

const minimist = require("minimist");
const chalk = require("chalk");
const getline = require("get-line-from-pos");

/**
 * `wasm` is the assembly language for Why.js. The `wasmc` utility parses it and compiles it to `wvm` bytecode.
 * 
 * @module wasm
 */

require("string.prototype.padstart").shim();
require("string.prototype.padend").shim();

import {EXCEPTIONS, R_TYPES, I_TYPES, J_TYPES, OPCODES, FUNCTS, REGISTER_OFFSETS,
       MAX_ARGS, FLAGS, EXTS, CONDITIONS, SYMBOL_TYPES, SymbolEnum} from "./constants";
const isLabelRef = (x: any) => x instanceof Array && x.length == 2 && x[0] == "label";



export type Register = ["register", "zero" | "return" | "stack" | "lo" | "hi" | "g" | "st", 0]
					 | ["register", "s" | "k" | "e" | "m" | "r" | "a" | "t", number];
export type SymbolTable = {[key: string]: [number, Long, SymbolEnum]};
export type Condition = "p" | "n" | "z" | "nz";
export type ASMInstruction = [string, string, ...any[]] & ASMInstructionMeta; // [label, op, ...args]
export type ASMInstructionType = "r" | "i" | "j";
export type ASMInstructionMeta = {
	opcode?: number,
	rs?: number,
	rd?: number,
	flags?: number,
	type?: ASMInstructionType,
	conditions?: Condition,
	op?: string,
	
	rt?: number,
	funct?: number,
	
	imm?: number,
	
	addr?: number,
	link?: boolean,
};
export type DataPair = ["int" | "float", number] | ["string", string] | ["bytes", number] | ["var", string];
export type ASMParsedInstruction = [string, string, ...any[]] & {inSubroutine?: boolean};
export type ASMParsed = {
	code: ASMParsedInstruction[],
	meta: {[key: string]: any},
	data: {[key: string]: DataPair},
	includes?: string[]
};
export type SymbolType = "data" | "code" | "other";

/**
 * Represents a `wasmc` instance.
 */
export default class WASMC {
	options: {[key: string]: any};
	ignoreFlags: boolean;
	parsed: ASMParsed;
	offsets: {[key: string]: number};
	meta: Long[];
	data: Long[];
	code: Long[];
	symbolTable: Long[];
	unknownSymbols: string[];
	dataOffsets: {[key: string]: number};
	dataVariables: {[key: string]: string};
	assembled: Long[];

	/**
	 * Creates a new wasmc instance.
	 * @param options An object containing options for the compiler (from the command line, for example).
	 */
	constructor(options: {[key: string]: any} = {}) {
		/**
		 * An object containing options for the compiler (from the command line, for example).
		 * @type {Object.<string, *>}
		 * @name module:wasm~WASMC#options
		 */
		this.options = options;

		/**
		 * Whether to prevent adding linker flags to the bytecode.
		 * @type {boolean}
		 * @name module:wasm~WASMC#ignoreFlags
		 * @default
		 */
		this.ignoreFlags = "ignoreFlags" in this.options? this.options.ignoreFlags : false;

		/**
		 * Contains the abstract syntax tree once {@link module:wasm~WASMC#parse WASMC.parse()} is called.
		 * @type {Object}
		 * @name module:wasm~WASMC#parsed
	 	 */
		this.parsed = {meta: {}, data: {}, code: []};

		/**
		 * Contains a list of offsets/labels.
		 * @type {Object.<string, number>}
		 * @name module:wasm~WASMC#offsets
		 */
		this.offsets = {};

		/**
		 * Contains the program's metadata as an array of Longs. See the ISA documentation for the layout.
		 * @type {Long[]}
		 * @name module:wasm~WASMC#meta
		 */
		this.meta = [];

		/**
		 * Contains the program's data as an array of Longs.
		 * @type {Long[]}
		 * @name module:wasm~WASMC#data
		 */
		this.data = [];

		/**
		 * Contains the program's bytecode as an array of Longs.
		 * @type {Long[]}
		 * @name module:wasm~WASMC#code
		 */
		this.code = [];

		/**
		 * Contains the encoded symbol table.
		 * @type {Long[]}
		 * @name module:wasm~WASMC#symbolTable
		 */
		this.symbolTable = [];
		
		/**
		 * An array of unknown symbols while compiling.
		 * @type {string[]}
		 * @name module:wasm~WASMC#unknownSymbols
		 */
		this.unknownSymbols = [];

		/**
		 * An array of offsets of data values relative to the start of the data section.
		 * @type {Object.<string, number>}
		 * @name module:wasm~WASMC#dataOffsets
		 */
		this.dataOffsets = {};

		/**
		 * An object mapping names of data variables to the names of variables whose addresses they store.
		 * @type {Object.<string, string>}
		 * @name module:wasm~WASMC#dataVariables
		 */
		 this.dataVariables = {};

		 this.assembled = [];
	}

	/**
	 * Loads the Nearley grammar, parses a source string and stores the AST in {@link module:wasm~WASMC#parsed parsed}.
	 * @param {string} source Source code of a wasm program.
	 */
	parse(source: string): void {
		let compiled: CompiledRules;
		try {
			compiled = require("./wasm.js");
		} catch (e) {
			console.error("Couldn't read wasm.js.");
			if (this.options.debug) {
				console.error(e);
			}

			process.exit(1);
			throw e;
		}

		const grammar: Grammar = Grammar.fromCompiled(compiled);
		grammar.start = compiled.ParserStart;
		const parser: Parser = new Parser(grammar);
		source += "\n";

		let trees: any[];
		try {
			trees = parser.feed(source).results;
		} catch (e) {
			console.error(chalk.red("Syntax error"), `at ${chalk.white(getline(source, e.offset))}: ${e.offset -
			              source.split(/\n/).slice(0, getline(source, e.offset) - 1).join("\n").length}:`);
			if (this.options.dev) {
				console.log(e.message.replace(/\(@(\d+):([^)]+)\)/g, ($0: never, $1: never, $2: string) => {
					const _line = getline(source, e.offset);
					return `(@${_line}:${e.offset - source.split(/\n/).slice(0, _line).join("\n").length + $2})`;
				}));
			}

			process.exit(1);
			throw e;
		}

		if (trees.length > 1) {
			trees.forEach(tree => console.log(JSON.stringify(tree, null, 4) + "\n" + chalk.bold("".padStart(64, "-"))));
			console.error(chalk.red.italic(`\nAmbiguous grammar (${trees.length}).\n`));
			process.exit(1);
		} else if (trees.length === 0) {
			console.warn(chalk.red.italic("Nothing parsed."));
			process.exit(1);
		}

		this.parsed = trees[0];

		if (typeof this.parsed != "object") {
			WASMC.die("Error: parser output isn't an object.");
		}

		if (typeof this.parsed.meta == "undefined") {
			this.parsed.meta = {};
		}

		if (typeof this.parsed.data == "undefined") {
			this.parsed.data = {};
		}

		if (typeof this.parsed.code == "undefined") {
			this.parsed.code = [];
		}
	}

	/**
	 * {@link module:wasm~WASMC#parse Parses} and processes the source code and writes the output.
	 * @param  source The source code of a wasm program.
	 * @return The assembled output of the program.
	 */
	compile(source: string): Long[] {
		this.parse(source);
		const labels = this.findAllLabels();
		this.symbolTable = this.createSymbolTable(labels, true);
		this.processMetadata();
		this.processData();

		const expanded = this.expandCode();
		this.metaOffsetData = this.metaOffsetCode.add(expanded.length * 8);

		const end = this.metaOffsetData.toInt() + this.dataLength * 8;
		this.offsets[".end"] = end;
		this.metaOffsetEnd = Long.fromInt(end, true);

		this.setDataOffsets(this.metaOffsetData.toInt());
		this.reprocessData();
		this.processCode(this.expandLabels(expanded));
		this.symbolTable = this.createSymbolTable(labels, false);

		this.assembled = [...this.meta, ...this.symbolTable, ...this.code, ...this.data];

		if (this.options.debug) {
			console.log({
				meta: WASMC.longs2strs(this.meta),
				data: WASMC.longs2strs(this.data),
				code: WASMC.longs2strs(this.code),
				offsets: this.offsets
			});
		}

		return this.assembled;
	}

	/**
	 * Writes the output to a file.
	 * @param {string}  outfile The path to the output file.
	 * @param {string} [infile] The name of the input file.
	 */
	writeOutput(outfile: string, infile: string): void {
		const frozen = WASMC.longs2strs(this.assembled).join("\n");

		fs.writeFileSync(outfile, frozen);
		console.log(chalk.green.bold(" \u2714"), "Successfully assembled", infile? chalk.bold(infile) : "the program",
		            "and saved the output to", chalk.bold(outfile) + ".");

		if (0 < this.unknownSymbols.length) {
			console.log(chalk.yellow.bold(" ?"), "Unknown symbol" + (this.unknownSymbols.length == 1? "" : "s") + ":",
			            this.unknownSymbols.map(s => chalk.bold(s)).join(", "));
		}
	}
	
	/**
	 * Extracts and processes the program's metadata and stores it in {@link module:wasm~WASMC#meta meta}.
	 */
	processMetadata(): void {
		const [name, version, author] = [this.parsed.meta.name || "?", this.parsed.meta.version || "?",
		                                 this.parsed.meta.author || "?"];
		
		const orcid = this.parsed.meta.orcid === undefined? "0000000000000000"
		                                                  : this.parsed.meta.orcid.replace(/\D/g, "");
		if (orcid.length != 16) {
			WASMC.die("Error: invalid ORCID.");
		}
		
		// Convert the ORCID into two Longs and stash them in the correct positions in meta.
		this.meta = [Long.UZERO, Long.UZERO, Long.UZERO, Long.UZERO, ...[orcid.substr(0, 8), orcid.substr(8)]
		            .map(half => WASMC.chunk2long(half.split("")))];
		
		// Append the name-version-author string.
		this.meta = this.meta.concat(WASMC.str2longs(`${name}\0${version}\0${author}\0`));
		
		// The beginning of the symbol table comes right after the end of the meta section.
		this.metaOffsetSymbols = Long.fromInt(this.meta.length * 8, true);
		
		// The code section begins right after the symbol table.
		this.metaOffsetCode = this.metaOffsetSymbols.add(this.symbolTable.length * 8);
	}

	/**
	 * Extracts and processes the program's data and stores it in {@link module:wasm~WASMC#data data}.
	 */
	processData(): void {
		let length = 0;
		_(this.parsed.data).forEach(([type, value], key) => {
			if (key[0] != "%") {
				this.log(chalk.yellow("Assigning"), "[" + chalk.bold(length) + "]", "to", chalk.bold(key));
				this.dataOffsets[key] = length;
			}
			
			const pieces = this.convertDataPieces(type, value, key);
			this.data = this.data.concat(pieces);
			length += pieces.length * 8;
		});
	}

	/**
	 * Replaces variable reference placeholders in the data section with the proper values of the pointers.
	 */
	reprocessData(): void {
		for (const key in this.dataVariables) {
			const ref = this.dataVariables[key];
			const index = this.dataOffsets[key] / 8;
			this.data[index] = Long.fromInt(this.offsets[ref], true);
		}
	}

	/**
	 * Adds an offset to each data offset and inserts the results into the offsets object.
	 * @param dataSectionStart The start offset of the data section.
	 */
	setDataOffsets(dataSectionStart: number): void {
		Object.keys(this.dataOffsets).forEach(key => this.offsets[key] = this.dataOffsets[key] + dataSectionStart);
	}

	/**
	 * Converts a data entry to an array of longs.
	 * @param  type  A data type.
	 * @param  value A data value.
	 * @return The encoded form of the entry.
	 */
	convertDataPieces(type: string, value: number | string, key?: string): Long[] {
		if (type.match(/^(in|floa)t$/)) {
			return [Long.fromValue(value)];
		}
		
		if (type == "string") {
			return WASMC.str2longs(value + "\0");
		}

		if (type == "bytes") {
			if (typeof value != "number") {
				throw new Error(`Expected numerical value for data type "bytes", received string.`);
			}

			return [...Array(Math.ceil(value / 8))].map(() => Long.fromInt(0));
		}

		if (type == "var") {
			// Just a placeholder for now; replaced with the address in reprocessData().
			if (key) {
				this.dataVariables[key] = value.toString();
			}

			return [Long.UZERO];
		}

		throw new Error(`Error: unknown data type "${type}".`);
	}

	/**
	 * Copies an array of expanded code into the {@link module:wasm~WASMC#code main code array}.
	 * @param expanded The list of expanded instructions to compile and store.
	 */
	processCode(expanded: ASMInstruction[]): void {
		expanded.forEach(item => this.addCode(item));
	}

	/**
	 * Returns a copy of {@link module:wasm~WASMC#parsed parsed}.code with all pseudoinstructions expanded.
	 * @return An array of expanded instructions.
	 */
	expandCode(): ASMInstruction[] {
		const expanded: ASMInstruction[] = [];
		// In the first pass, we expand pseudoinstructions into their constituent parts. Some instructions have to be
		// processed once again after labels have been sorted out so we can replace variable references with addresses.
		this.parsed.code.forEach(item => {
			const [label, op, ...args] = item;
			if (label) {
				this.offsets[label] = this.metaOffsetCode.toInt() + expanded.length * 8;
				this.log(chalk.magenta("Assigning"), chalk.bold(this.offsets[label]), "to", label,
				         "based on an expanded length equal to", chalk.bold(expanded.length));
			}

			const add = (x: ASMInstruction) => expanded.push(x);

			const addPush = (args: Register[], _label: string = label) => {
				const getLabel = () => [_label, _label = null][0];
				args.forEach(reg => {
					add([getLabel(), "spush", _0, reg, _0]);
				});
			};

			const addPop = (args: Register[], _label: string = label) => {
				const getLabel = () => [_label, _label = null][0];
				args.forEach((reg: Register) => {
					add([getLabel(), "spop", _0, _0, reg]);
				});
			};

			if (op == "call") {
				const [name, ...vals] = args;

				// There can't be more arguments than the set of argument registers can handle.
				if (MAX_ARGS < vals.length) {
					throw new Error(`Too many arguments given to subroutine (given ${vals.length}, maximum is ` +
					                `${MAX_ARGS})`);
				}

				const currentValues = _.range(0, vals.length).map(n => _A[n]);
				if (item.inSubroutine) {
					currentValues.unshift(_RA);
				}

				if (currentValues.length) {
					// Push the current return address and the current values in
					// $a_0...$a_{n-1} (in that order) to the stack.
					addPush(currentValues, label);
				}

				// For each argument in the subroutine call, set its corresponding
				// argument register based on the type of the argument. (An argument
				// can be an immediate value, a register, a variable dereference or
				// a label/variable reference.)
				vals.forEach((val, i) => {
					if (typeof val == "number") {
						add([null, "set", _0, _A[i], Long.fromInt(val).toUnsigned().toInt()]);
					} else if (!(val instanceof Array)) {
						throw new Error(`Invalid value for argument ${i + 1}: ${JSON.stringify(val)}`);
					} else if (val[0] == "register") {
						add([null, "or", val, _0, _A[i]]);
					} else if (val[0] == "label") {
						add([null, "li", _0, _A[i], val]);
					} else if (val[0] == "address") {
						add([null, "set", _0, _A[i], ["label", val[1]]]);
					} else {
						throw new Error(`Invalid value for argument ${i + 1}: ${JSON.stringify(val)}`);
					}
				});

				// Store the program counter in $rt and jump to the subroutine.
				add([null, "j", _0, ["label", name], true]);

				if (currentValues.length) {
					// Now that we've returned from the subroutine, pop the values we pushed earlier,
					// but in reverse order.
					addPop(currentValues.reverse(), null);
				}
			} else if (op == "ext") {
				const funct = args[3];
				if (funct == EXTS.prc) {
					const type = args[1][0];
					if (type == "char") {
						add([label, "set", _0, _M[2], args[1][1]]);
						add([null, "ext", 0, ["register", "m", 2], 0, EXTS.prc]);
						return;
					} else if (type == "string") {
						let lastChar;
						args[1][1].split("").forEach((c, i) => {
							const char = c.charCodeAt(0);
							if (char != lastChar) {
								add([i == 0? label : null, "set", _0, _M[2], char]);
								lastChar = char;
							}
							
							add([null, "ext", 0, ["register", "m", 2], 0, EXTS.prc]);
						});

						return;
					}
				}
				
				add([label, "ext", ...args]);
			} else if (op == "mv") {
				add([label, "or", args[0], _0, args[1]]);
			} else if (op == "push") {
				addPush(args);
			} else if (op == "pop") {
				addPop(args);
			} else if (op == "jeq") {
				const right = args[0];
				const left = args[1];
				if (isLabelRef(right)) {
					add([label, "li", _0, _M[2], right]);
					add([null, "seq", _M[2], left, _M[0]]);
				} else if (typeof right == "number") {
					add([label, "seqi", left, _M[0], right]);
				} else {
					add([label, "seq", right, left, _M[0]]);
				}

				if (args[2][0] == "value") {
					// Set $m1 to the immediate value and then conditionally jump to $m1.
					add([null, "set", _0, _M[1], args[2][1]]);
					add([null, "jrc", _0, _M[0], _M[1]]);
				} else if (args[2][0] == "register") {
					// We're already given a register, so we don't have to do anything with $m1.
					add([null, "jrc", _0, _M[0], args[2]]);
				} else if (args[2][0] == "label") {
					// Load the value of the given variable into $m1 and then conditionally jump to $m1.
					this.log("jeq with label:", args[2]);
					add([null, "set",  _0, _M[1], args[2]]);
					add([null, "jrc", _0, _M[0],   _M[1]]);
				}
			} else if (R_TYPES.includes(OPCODES[op]) && args.some(isLabelRef)) {
				let [rt, rs, rd] = args;
				let [lt, ls, ld] = [rt, rs, rd].map(isLabelRef);
				let _label = label, getLabel = (): string => [_label, _label = null][0];
				[rt, rs].forEach((reg, i) => {
					if (isLabelRef(reg)) {
						// Whoops, this register isn't actually a register
						add([getLabel(), "li", _0, _M[i], reg]);
					}
				});

				add([getLabel(), op, ...[rt, rs, rd].map((reg, i) => [lt, ls, ld][i]? _M[i] : reg)]);

				if (ld) {
					add([getLabel(), "si", _M[2], _0, rd]);
				}
			} else if (I_TYPES.includes(OPCODES[op]) && args.some(isLabelRef)) {
				let [rs, rd, imm] = args;
				let [ls, ld] = [rs, rd].map(isLabelRef);
				let _label = label, getLabel = () => [_label, _label = null][0];
				if (ls) {
					add([getLabel(), "li", _0, _M[0], rs]);
				}

				add([getLabel(), op, ...[...[rs, rd].map((reg, i) => [ls, ld][i]? _M[i] : reg), imm]]);

				if (ld) {
					add([getLabel(), "si", _M[1], _0, rd]);
				}
			} else if (op == "gap") {
				for (let i = 0; i < args[0]; i++) {
					add([i? null : label, "nop"]);
				}
			} else {
				add(item);
			}
		});

		return expanded;
	}

	/**
	 * Replaces all label references in a given array of expanded instructions with the corresponding memory addresses.
	 * Mutates the input array.
	 * @param  expanded An array of expanded instructions (see {@link module:wasm~WASMC#expandCode expandCode}).
	 * @return The mutated input array with label references replaced with memory addresses.
	 */
	expandLabels(expanded: ASMInstruction[]): ASMInstruction[] {
		// In the second pass, we replace label references with the corresponding
		// addresses now that we know the address of all the labels.
		expanded.forEach(item => {
			// First off, now that we've recorded all the label positions,
			// we can remove the label tags.
			item.shift();

			// Look at everything past the operation name (the arguments).
			item.slice(1).forEach((arg, i) => {
				// If the argument is a label reference,
				if (isLabelRef(arg)) {
					// replace it with an address from the offsets map. 
					const offset = this.offsets[arg[1]];
					if (typeof offset == "undefined") {
						item[i + 1] = WASMC.encodeSymbol(arg[1]);
						item.flags = FLAGS.UNKNOWN_SYMBOL;
						if (!this.unknownSymbols.includes(arg[1])) {
							this.unknownSymbols.push(arg[1]);
						}
					} else {
						item[i + 1] = offset;
						item.flags = FLAGS.KNOWN_SYMBOL;
					}
				}
			});
		});

		return expanded;
	}

	/**
	 * Compiles an instruction and adds it to the {@link module:wasm~WASMC#code code array}.
	 * @param item The instruction to compile and add.
	 */
	addCode(item: ASMInstruction): void {
		this.code.push(this.compileInstruction(item));
	}

	/**
	 * Compiles an array representation of an instruction into a Long containing the bytecode.
	 * @param  instruction An uncompiled instruction.
	 * @return The bytecode representation of the instruction.
	 */
	compileInstruction(instruction: ASMInstruction): Long {
		const [op, ...args] = instruction;
		const {flags} = instruction;
		if (op == "ext") {
			const [rt, rs, rd] = args.slice(0, 3).map(WASMC.convertRegister);
			return this.rType(OPCODES.ext, rt, rs, rd, args[3], flags, null);
		} else if (R_TYPES.includes(OPCODES[op])) {
			const [rt, rs, rd] = args.slice(0, 3).map(WASMC.convertRegister);
			return this.rType(OPCODES[op], rt, rs, rd, FUNCTS[op], flags, args[3]);
		} else if (I_TYPES.includes(OPCODES[op])) {
			const [rs, rd] = args.slice(0, 2).map(this.convertValue, this);
			return this.iType(OPCODES[op], rs, rd, args[2], flags, null);
		} else if (J_TYPES.includes(OPCODES[op])) {
			return this.jType(OPCODES[op], this.convertValue(args[0]), args[1], args[2], flags, args[3]);
		} else if (op == "nop") {
			return Long.UZERO;
		} else {
			WASMC.warn(`Unhandled instruction ${chalk.bold.red(op)}.`, [op, ...args]);
			return Long.fromString("6666deadc0de6666", true, 16);
		}
	}

	/**
	 * Compiles an object representation of an instruction into a Long containing the bytecode.
	 * @param  instruction An uncompiled instruction.
	 * @return The bytecode representation of the instruction.
	 */
	unparseInstruction(instruction: ASMInstruction): Long {
		const {op, type} = instruction;
		if (type == "r") {
			const {opcode, rt, rs, rd, funct, flags, conditions} = instruction;
			return this.rType(opcode, rt, rs, rd, funct, flags, conditions);
		} else if (type == "i") {
			const {opcode, rs, rd, imm, flags, conditions} = instruction;
			return this.iType(opcode, rs, rd, imm, flags, conditions);
		} else if (type == "j") {
			const {opcode, rs, addr, link, flags, conditions} = instruction;
			return this.jType(opcode, rs, addr, link, flags, conditions);
		} else if (op == "nop") {
			return Long.UZERO;
		} else {
			WASMC.warn(`Unhandled instruction ${chalk.bold.red(op)}.`, instruction);
			return Long.fromString("6666deadc0de6666", true, 16);
		}
	}

	/**
	 * If the input is an array or number accepted by convertRegister, the output is the corresponding register index.
	 * If the input is a string, then the label map is checked and the corresponding address is returned.
	 * @param  x The value to convert.
	 * @return The converted value.
	 * @throws Will throw an exception if the input is of an unrecognized type.
	 */
	convertValue(x: Register | number | string): number {
		if (x instanceof Array || typeof x == "number") {
			return WASMC.convertRegister(x);
		}

		if (typeof x == "string") {
			if (typeof this.offsets[x] == "undefined") {
				throw new Error(`Undefined label: ${x}`);
			}

			return this.offsets[x];
		}

		throw new Error(`Unrecognized value: ${x}`);
	}

	/**
	 * Compiles an R-type instruction into bytecode.
	 * @param  opcode      The instruction's opcode.
	 * @param  rt          The instruction's secondary source register.
	 * @param  rs          The instruction's primary source register.
	 * @param  rd          The instruction's destination register.
	 * @param  func        The instruction's function field.
	 * @param [flags=0]    The linker flags to embed in the instruction.
	 * @param [conditions] The conditions for the instruction.
	 * @param [warn=true]  Whether to enable warnings for invalid ranges.
	 * @return The compiled instruction.
	 */
	rType(opcode: number, rt: number, rs: number, rd: number, func: number, flags: number = 0,
	      conditions: string = null, warn: boolean = true): Long {

		if (!R_TYPES.includes(opcode))
			throw new Error(`Opcode ${opcode} isn't a valid r-type`);

		if (warn) {
			if (rt < 0 || 127 < rt) WASMC.warn(`rt (${rt}) not within the valid range (0–127)`);
			if (rs < 0 || 127 < rs) WASMC.warn(`rs (${rs}) not within the valid range (0–127)`);
			if (rd < 0 || 127 < rd) WASMC.warn(`rd (${rd}) not within the valid range (0–127)`);
			if (func < 0 || 4095 < func) WASMC.warn(`func (${func}) not within the valid range (0–4095)`);
		}

		let lower = func | (this.ignoreFlags? 0 : flags << 12) | (CONDITIONS[conditions] << 14) | ((rd & 1) << 31);
		let upper = (rd >> 1) | (rs << 6) | (rt << 13) | (opcode << 20);
		let long = Long.fromBits(lower, upper, true);

		return long;
	}

	/**
	 * Compiles an I-type instruction into bytecode.
	 * @param  opcode      The instruction's opcode.
	 * @param  rs          The instruction's source register.
	 * @param  rd          The instruction's destination register.
	 * @param  imm         The instruction's immediate value.
	 * @param [flags=0]    The linker flags to embed in the instruction.
	 * @param [conditions] The conditions for the instruction.
	 * @param [warn=true]  Whether to enable warnings for invalid ranges.
	 * @return The compiled instruction.
	 */
	iType(opcode: number, rs: number, rd: number, imm: number, flags: number = 0,
	      conditions: string = null, warn: boolean = true): Long {

		if (!I_TYPES.includes(opcode))
			throw new Error(`Opcode ${opcode} isn't a valid i-type`);

		if (warn) {
			if (rs < 0 || 127 < rs) WASMC.warn(`rs (${rs}) not within the valid range (0–127)`);
			if (rd < 0 || 127 < rd) WASMC.warn(`rd (${rd}) not within the valid range (0–127)`);
			if (imm < -2147483648 || 2147483647 < imm) {
				WASMC.warn(`imm (${imm}) not within the valid range (-2147483648–2147483647)`);
			}
		}

		const lower = imm;
		const upper = rd | (rs << 7) | (this.ignoreFlags? 0 : flags << 14) |
		              (CONDITIONS[conditions] << 16) | (opcode << 20);
		return Long.fromBits(lower, upper);
	}

	/**
	 * Compiles a J-type instruction into bytecode.
	 * @param  opcode      The instruction's opcode.
	 * @param  rs          The instruction's source register.
	 * @param  addr        The instruction's address field.
	 * @param  link        The instruction's link bit.
	 * @param [flags=0]    The linker flags to embed in the instruction.
	 * @param [conditions] The conditions for the instruction.
	 * @param [warn=true]  Whether to enable warnings for invalid ranges.
	 * @return The compiled instruction.
	 */
	jType(opcode: number, rs: number, addr: number, link: boolean = false, flags: number = 0,
	      conditions: string = null, warn: boolean = true): Long {

		if (!J_TYPES.includes(opcode))
			throw new Error(`Opcode ${opcode} isn't a valid j-type`);

		if (warn) {
			if (rs < 0 || 127 < rs) WASMC.warn(`rs (${rs}) not within the valid range (0–127)`);
			if (addr < 0 || 4294967295 < addr)
				WASMC.warn(`addr (${addr}) not within the valid range (0–4294967295)`);
		}

		const lower = addr;
		const upper = (this.ignoreFlags? 0 : flags) | (CONDITIONS[conditions] << 2) |
		              (+link << 12) | (rs << 13) | (opcode << 20);
		return Long.fromBits(lower, upper, true);
	}

	/**
	 * Prints a warning.
	 * @param args The arguments to pass to console.log.
	 */
	static warn(...args: any[]): void {
		console.warn(...args);
	}

	/**
	 * Prints a message if the debug {@link module:wasm~WASMC#options option} is set.
	 * @param args The arguments to pass to console.log.
	 */
	log(...args: any[]): void {
		if (this.options.debug) {
			console.log(...args);
		}
	}

	/**
	 * Finds an array of all labels found in the program's data and code sections.
	 * @return {string[]} An array of labels.
	 */
	findAllLabels(): string[] {
		return _.uniq([
			...Object.keys(this.parsed.data).filter(label => label[0] != "%"),
			...this.parsed.code.map(([label]) => label).filter(label => label)
		]);
	}

	/**
	 * Returns the number of words in the symbol table, regardless of whether the symbol table has been made yet.
	 * @param  {string[]} labels An array of labels.
	 * @return {number} The length of the symbol table.
	 */
	static symbolTableLength(labels: string[]): number {
		return _.uniq([...labels, ".end"]).reduce((a, b) => a + 2 + Math.ceil(b.length / 8), 0);
	}

	/**
	 * Returns a symbol table.
	 * @param  labels         An array of labels.
	 * @param [skeleton=true] Whether to set all addresses to zero at first.
	 * @return An encoded symbol table.
	 */
	createSymbolTable(labels: string[], skeleton: boolean = true): Long[] {
		return _.uniq([...labels, ".end"]).map(label => {
			const length = Math.ceil(label.length / 8) & 0xffff;
			let type = SYMBOL_TYPES.unknown;

			if (!skeleton && label in this.dataVariables) {
				const ptr = this.dataVariables[label];
				type = ptr in this.offsets? SYMBOL_TYPES.knownPointer : SYMBOL_TYPES.unknownPointer;
				if (!(ptr in this.offsets || this.unknownSymbols.includes(ptr))) {
					const index = (this.offsets[label] - this.metaOffsetData.toInt()) / 8;
					this.data[index] = Long.fromInt(WASMC.encodeSymbol(ptr), true);
					this.unknownSymbols.push(ptr);
				}
			}

			return [
				Long.fromBits(length | (type << 16), WASMC.encodeSymbol(label), true),
				skeleton? Long.UZERO : Long.fromInt(this.offsets[label], true),
				...WASMC.str2longs(label)
			];
		}).flat();
	}

	/**
	 * Encodes a parsed symbol table into an array of Longs.
	 * @param  symbolTable An object mapping a symbol name to a tuple of its ID and its address.
	 * @return An encoded symbol table.
	 */
	static encodeSymbolTable(symbolTable: SymbolTable): Long[] {
		return _.flatten(Object.keys(symbolTable).map(label => [
			Long.fromBits(Math.ceil(label.length / 8), WASMC.encodeSymbol(label), true),
			symbolTable[label][1],
			...WASMC.str2longs(label)
		]));
	}

	/**
	 * Returns the length in words of the data section.
	 */
	get dataLength(): number {
		return Object.values(this.parsed.data).reduce((a, b) => a + this.convertDataPieces(b[0], b[1]).length, 0);
	}

	get metaOffsetSymbols(): Long { return this.meta[0]; }
	get metaOffsetCode():    Long { return this.meta[1]; }
	get metaOffsetData():    Long { return this.meta[2]; }
	get metaOffsetEnd():     Long { return this.meta[3]; }
	set metaOffsetSymbols(to: Long) { this.meta[0] = to; }
	set metaOffsetCode(to: Long)    { this.meta[1] = to; }
	set metaOffsetData(to: Long)    { this.meta[2] = to; }
	set metaOffsetEnd(to: Long)     { this.meta[3] = to; }

	/**
	 * Prints a message to stderr and exits the process with return code 1.
	 * @param args The arguments to pass to `console.error`.
	 */
	static die(...a: any[]): never {
		console.error(...a);
		process.exit(1);
		throw a;
	}

	/**
	 * Converts an array of 8 characters into a Long.
	 * @param  chunk An array of 8 characters.
	 * @return A Long containing the concatenated ASCII values of the characters.
	 */
	static chunk2long(chunk: string[]): Long {
		return Long.fromString(chunk.map(c => c.charCodeAt(0).toString(16).padStart(2, "0")).join(""), true, 16);
	}

	/**
	 * Adds nulls to the end of the string to lengthen it to a multiple of 8.
	 * If the length is already a multiple of eight, add one null at the end.
	 * @param  str The string to be nullpadded.
	 * @return The concatenation of the given string and a number of null characters.
	 */
	static nullpad(str: string): string {
		return str.length % 8? str.padEnd(Math.ceil(str.length / 8) * 8, "\0") : `${str}\0`;
	}

	/**
	 * Nullpads and chunks a given string into an array of Longs.
	 * @param  str The string to convert into Longs.
	 * @return An array of Longs representing the input string.
	 */
	static str2longs(str: string): Long[] {
		if (str == "") {
			return [Long.UZERO];
		}

		return _.chunk(str.padEnd(Math.ceil(str.length / 8) * 8, "\0").split(""), 8).map(WASMC.chunk2long);
	}

	/**
	 * Returns an array containing the 16-length zero-padded hex representations of a given array of Longs.
	 * If any element of the input array isn't a Long value, it will be represented as a string of 16 "x"s.
	 * @param  longs An array of Longs to convert to strings.
	 * @return An array of zero-padded hex strings corresponding to the inputs.
	 */
	static longs2strs(longs: Long[]): string[] {
		return longs.map(l => l.toString(16).padStart(16, "0"));
	}

	/**
	 * If the input is an array (expected format: ["register", ...]), then the output is the number corresponding to
	 * that array. Otherwise, if the input is something other than an array, then the output is same as the input.
	 * @param  x An array representing a register, such as ["register", "return", 0] for $rt or ["register", "t", 22]
	 *           for $t16.
	 * @return The ID corresponding to the register.
	 */
	static convertRegister(x: Register | number): number {
		return x instanceof Array? REGISTER_OFFSETS[x[x.length - 2]] + x[x.length - 1] : x;
	}

	/**
	 * Encodes the name of a symbol.
	 * @param  {string} name A symbol name.
	 * @return {number} The symbol name encoded as a number.
	 */
	static encodeSymbol(name: string): number {
		const hash = createHash("sha256");
		hash.update(name);
		// Can be up to 13 digits before precision limits become apparent, but we need only 8 anyway
		return Math.abs(parseInt(hash.digest("hex").substr(0, 7), 16));
	}
}

const _A:  Register[] = _.range(0, 16).map(n => ["register", "a", n]);
const _RA: Register   = ["register", "return", 0];
const _M:  Register[] = _.range(0, 16).map(n => ["register", "m", n]);
const _0:  Register    = ["register", "zero",  0];

if (require.main === module) {
	const options = minimist(process.argv.slice(2), {
		alias: {
			b: "binary",
			d: "debug"
		},
		boolean: ["binary", "debug"],
		default: {
			binary: false,
			debug: false
		}
	}), filename = options._[0];

	if (!filename) {
		console.log("Usage: ./wasmc.js <filename> [out]");
		process.exit(0);
	}

	options.out = options._[1];

	const asm = new WASMC(options);
	asm.compile(fs.readFileSync(filename, "utf8"));
	asm.writeOutput(!options.out? filename.replace(/\.wasm$/i, "") + ".why" : options.out, filename);
}

/**
 * Represents an object mapping a symbol name to a tuple of its ID and its address.
 * @typedef {Object<string, Array<number, Long>>} SymbolTable
 */

/**
 * Represents a symbol type.
 * @typedef {"code"|"data"|"other"} SymbolType
 */
