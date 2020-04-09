#!/usr/bin/env ts-node
import * as fs from "fs";
import * as Long from "long";
import _ from "../util";

import {SymbolTable} from "./wasmc";
import {EXCEPTIONS, R_TYPES, I_TYPES, J_TYPES, OPCODES, FUNCTS, REGISTER_OFFSETS, EXTS, CONDITIONS, FLAGS, ConditionName, OpType, FlagValue, RType, IType, JType, isFlag} from "./constants";

const minimist = require("minimist");
const chalk_ = require("chalk");
const chalk = new chalk_.Instance({level: 1});
const {yellow, magenta, dim, cyan, red, green, bold} = chalk;

/**
 * @module wasm
 */

require("string.prototype.padstart").shim();
require("string.prototype.padend").shim();

const OPCODES_INV = _.multiInvert(OPCODES);
const OFFSETS_INV = _.multiInvert(REGISTER_OFFSETS);
const CONDITIONS_INV = <{[key: number]: ConditionName}> _.invert(CONDITIONS);
const OFFSET_VALUES = _.uniq(Object.values(REGISTER_OFFSETS)).sort((a, b) => (b as any) - (a as any));

export type SegmentOffsets = {$symtab: number, $code: number, $data: number, $end: number};
type MetaField = "orcid" | "name" | "version" | "author";
type FormatRFunction = (op: string, rt: string, rs: string, rd: string, funct: number, flags: FlagValue,
                        conditions: ConditionName | null) => string;
type FormatIFunction = (op: string, rs: string, rd: string, imm: Long, flags: FlagValue,
                        conditions: ConditionName | null, symbols: SymbolTable) => string;
type FormatJFunction = (op: string, rs: string, addr: Long, link: boolean, flags: FlagValue,
                        conditions: ConditionName | null, symbols: SymbolTable) => string;

export type ParserInstructionR = {
	type: "r",
	op: RType | "ext",
	opcode: number,
	conditions: ConditionName,
	flags: FlagValue,
	rt: number,
	rs: number,
	rd: number,
	funct: number
};

export type ParserInstructionI = {
	type: "i",
	op: IType,
	opcode: number,
	conditions: ConditionName,
	flags: FlagValue,
	rs: number,
	rd: number,
	imm: Long
};

export type ParserInstructionJ = {
	type: "j",
	op: JType,
	opcode: number,
	conditions: ConditionName,
	flags: FlagValue,
	rs: number,
	link: boolean,
	addr: Long
};

export type ParserNOP = {op: "nop", type: "nop", opcode: null, flags: null, rs: null, conditions: null};
export type ParserInstruction = ParserInstructionR | ParserInstructionI | ParserInstructionJ | ParserNOP;
export type ParserMeta = {[key in MetaField]: string};
type FormatStyle = "wasm" | "mnem";

/**
 * Contains code for parsing compiled wvm bytecode.
 */
export default class Parser {
	raw: Long[];
	rawSymbols: Long[];
	rawMeta: Long[];
	rawCode: Long[];
	rawData: Long[];
	meta: ParserMeta;
	
	private static _formatStyle: FormatStyle = "wasm";
	static formatR: FormatRFunction = Parser.formatR_w;
	static formatI: FormatIFunction = Parser.formatI_w;
	static formatJ: FormatJFunction = Parser.formatJ_w;

	offsets: SegmentOffsets;
	symbols: SymbolTable;
	code: ParserInstruction[];

	/**
	 * Constructs a new `Parser` instance.
	 * @param {Long[]} [longs] An optional array of Longs that will be passed to {@link module:wasm~Parser#load load}
	 *                         if specified.
	 */
	constructor(longs?: Long[]) {
		if (longs) {
			this.load(longs);
		}
	}

	/**
	 * Loads a program from a file and parses it.
	 * @param {string}   filename     The filename of the program to load.
	 * @param {boolean} [silent=true] Whether to suppress debug output.
	 */
	open(filename: string, silent: boolean = true) {
		this.read(fs.readFileSync(filename, "utf8"), silent);
	}

	/**
	 * Parses a string representation of a program.
	 * @param {string}   text         A text representation (\n-separated hexadecimal numbers) of a program.
	 * @param {boolean} [silent=true] Whether to suppress debug output.
	 */
	read(text: string, silent: boolean = true) {
		this.load(text.split("\n").map((s) => Long.fromString(s, true, 16)), silent);
	}

	/**
	 * Stores an array of longs and {@link module:wasm~Parser#parse parses} them.
	 * @param {Long[]}   longs        An array of Longs.
	 * @param {boolean} [silent=true] Whether to suppress debug output.
	 */
	load(longs: Long[], silent: boolean = true) {
		this.raw = longs;
		this.parse(silent);
	}

	/**
	 * Parses the stored raw longs.
	 * @param {boolean} [silent=true] Whether to suppress debug output.
	 */
	parse(silent: boolean = true) {
		const longs = this.raw;

		/**
		 * The offsets defined in the program's metadata section.
		 * @type {Object<string, number>}
		 */
		this.offsets = {
			$symtab: longs[0].toInt(),
			$code:   longs[1].toInt(),
			$data:   longs[2].toInt(),
			$end:    longs[3].toInt()
		};

		/**
		 * Contains all the unparsed Longs in the metadata section.
		 * @type {Long[]}
		 */
		this.rawMeta = longs.slice(0, this.offsets.$symtab / 8);

		const [metaName, metaVersion, metaAuthor] = _.longStrings(longs.slice(7, this.offsets.$code));
		
		/**
		 * The parsed metadata section.
		 * @type {Object}
		 */
		this.meta = {
			orcid: _.chunk(_.longString([longs[5], longs[6]]), 4).map(n => n.join("")).join("-"),
			name: metaName,
			version: metaVersion,
			author: metaAuthor,
		};

		/**
		 * Contains all the unparsed Longs in the symbol table.
		 * @type {Long[]}
		 */
		this.rawSymbols = longs.slice(this.offsets.$symtab / 8, this.offsets.$code / 8);

		/**
		 * Contains the parsed symbol table.
		 * @type {SymbolTable}
		 */
		this.symbols = this.getSymbols();

		/**
		 * Contains all the unparsed Longs in the code section.
		 * @type {Long[]}
		 */
		this.rawCode = longs.slice(this.offsets.$code / 8, this.offsets.$data / 8);

		/**
		 * Contains all the parsed instructions.
		 * @type {Object}
		 */
		this.code = this.rawCode.map(Parser.parseInstruction);

		/**
		 * Contains all the unparsed Longs in the data section.
		 * @type {Long[]}
		 */
		this.rawData = longs.slice(this.offsets.$data / 8, this.offsets.$end / 8);

		if (!silent) {
			console.log(dim("/*"));
			console.log(green.dim("#offsets"));
			console.log(Object.keys(this.offsets).map((k: string) =>
				`${dim(`${k}:`)} ${magenta.dim(this.offsets[k])}`
			).join("\n"));
			console.log(dim("*/") + "\n");

			console.log(green("#meta"));
			console.log(Object.keys(this.meta).map((k: string) =>
				`${cyan(k)}: ${yellow(`"${this.meta[k]}"`)}`
			).join("\n") + "\n");

			console.log([
				"",
				green("#code"),
				...this.code.map(instr => Parser.formatInstruction(instr, this.symbols))
			].join("\n"));
		}
	}

	/**
	 * Reads the program's symbol table.
	 * @return {SymbolTable} An object mapping a symbol name to tuple of its ID and its address.
	 */
	getSymbols(): SymbolTable {
		const longs = this.raw;
		const start = longs[0].toInt() / 8;
		const end = longs[1].toInt() / 8;
		const out = {};
		for (let i = start, j = 0; i < end && j < 10000; j++) {
			const id = Math.abs(longs[i].high);
			const len = longs[i].low & 0xffff;
			const type = longs[i].low >> 16;
			const addr = longs[i + 1];

			const encodedName = longs.slice(i + 2, i + 2 + len).map((l) => l.toString(16).padStart(16, "0")).join("").replace(/(00)+$/, "");
			const name = _.chunk(encodedName, 2).map((x) => String.fromCharCode(parseInt(x.join(""), 16))).join("");

			out[name] = [id, addr, type];

			i += 2 + len;
		}

		return out;
	}

	/**
	 * Finds the length of the metadata section of the program.
	 * @return {number} The length of the metadata section in bytes.
	 */
	getMetaLength(): number {
		return this.raw[0].toInt();
	}

	/**
	 * Finds the length of the symbol table of the program.
	 * @return {number} The length of the symbol table in bytes.
	 */
	getSymbolTableLength(): number {
		return this.raw[1].subtract(this.raw[0]).toNumber();
	}

	/**
	 * Finds the length of the code section of the program.
	 * @return {number} The length of the code section in bytes.
	 */
	getCodeLength(): number {
		return this.raw[2].subtract(this.raw[1]).toNumber();
	}

	/**
	 * Finds the length of the data section of the program.
	 * @return {number} The length of the data section in bytes.
	 */
	getDataLength(): number {
		return this.raw[3].subtract(this.raw[2]).toNumber();
	}

	/**
	 * Parses a single instruction.
	 * @param  {Long|string} instruction An instruction represented as either a Long of a 64-long string of binary digits.
	 * @return {Object} An object containing information about the instruction. Always contains `op`, `opcode`, `rs`,
	 *                  `flags` and `type` for non-NOPs; other output varies depending on the type of the instruction.
	 */
	static parseInstruction(instruction: Long | string): ParserInstruction {
		const instructionString: string = typeof instruction == "string"?
			instruction : instruction.toString(2).padStart(64, "0");

		const get = (from: number, length?: number) => parseInt(instructionString.substr(from, length), 2);
		const opcode = get(0, 12);
		const type = Parser.instructionType(opcode);

		if (opcode == 0) {
			return {op: "nop", type: "nop", opcode: null, flags: null, rs: null, conditions: null};
		}

		const flags = get({r: 50, i: 16, j: 30}[type], 2);
		if (!isFlag(flags)) {
			throw new Error(`Invalid flags value when parsing instruction: ${flags}`);
		}

		if (type == "r") {
			const funct = get(52);
			return {
				op: opcode == OPCODES.ext? "ext" : OPCODES_INV[opcode].filter((op) =>
					OPCODES_INV[opcode].length == 1 || Parser.instructionType(opcode) == "r" && FUNCTS[op] == funct
				)[0],
				opcode,
				rt: get(12, 7),
				rs: get(19, 7),
				rd: get(26, 7),
				funct,
				conditions: CONDITIONS_INV[get(46, 4)],
				flags,
				type: "r"
			};
		} else if (type == "i") {
			return {
				op: OPCODES_INV[opcode][0],
				opcode,
				conditions: CONDITIONS_INV[get(12, 4)],
				flags,
				rs: get(18, 7),
				rd: get(25, 7),
				imm: Long.fromString(instructionString.substr(32), false, 2),
				type: "i"
			};
		} else if (type == "j") {
			return {
				op: OPCODES_INV[opcode][0],
				opcode,
				rs: get(12, 7),
				link: !!get(19, 1),
				conditions: CONDITIONS_INV[get(26, 4)],
				flags,
				addr: Long.fromString(instructionString.substr(32), false, 2),
				type: "j"
			};
		}
	}

	/**
	 * Styles an operator.
	 * @param  {string} oper An operator.
	 * @return {string} A styled operator.
	 */
	static colorOper(oper: string): string {
		return bold(oper);
	}

	/**
	 * Decompiles an instruction to the corresponding wasm source.
	 * @param  {Long|string|Object} instruction An instruction represented as either a Long, a 64-long string of
	 *                                          binary digits or an already-parsed object.
	 * @param  {SymbolTable} [symbols] A symbol table.
	 * @return {string} The wasm equivalent of the instruction.
	 */
	static formatInstruction(instruction: Long | string | ParserInstruction, symbols: SymbolTable): string {
		let parsed: ParserInstruction;
		if (instruction instanceof Long) {
			instruction = instruction.toString(2).padStart(64, "0");
		}
		
		if (typeof instruction == "object" && "op" in instruction) {
			parsed = instruction;
		} else {
			parsed = Parser.parseInstruction(instruction);
		}

		const {opcode, type, flags, rs, conditions} = parsed;

		if (type == "nop") {
			return "<>";
		}

		const srs = Parser.getRegister(rs);

		if (type == "j") {
			const {addr, link} = <ParserInstructionJ> parsed;
			return Parser.formatJ(OPCODES_INV[opcode][0], srs, addr, link, flags, conditions, symbols);
		}

		const srd = Parser.getRegister((<ParserInstructionR | ParserInstructionI> parsed).rd);

		if (type == "r") {
			const {rt, funct, flags} = (<ParserInstructionR> parsed);
			const srt = Parser.getRegister(rt);
			return Parser.formatR(OPCODES_INV[opcode].filter((op) => op == "ext" || FUNCTS[op] == funct)[0],
			srt, srs, srd, funct, flags, conditions);
		}
		
		if (type == "i") {
			const {imm} = (<ParserInstructionI> parsed);
			return Parser.formatI(OPCODES_INV[opcode][0], srs, srd, imm, flags, conditions, symbols);
		}

		if (typeof instruction == "string") {
			throw new Error(`Can't parse instruction ${instruction} (opcode = ${opcode}, type = "${type}").`);
		}
		
		throw new Error(`Can't parse instruction (opcode = ${opcode}, type = "${type}").`);
	}

	/**
	 * Returns the instruction type corresponding to an opcode.
	 * @param  {number} opcode An opcode.
	 * @return {?("r"|"i"|"j")} One of the operation types (r, i, j) if the opcode was recognized; `null` otherwise.
	 */
	static instructionType(opcode: number): OpType | null {
		if (R_TYPES.includes(opcode)) return "r";
		if (I_TYPES.includes(opcode)) return "i";
		if (J_TYPES.includes(opcode)) return "j";
		return null;
	}

	/**
	 * Converts a numeric register ID to its string representation.
	 * @param  {number} n A register ID.
	 * @return {string} A register name.
	 */
	static getRegister(n: number): string {
		if (n == REGISTER_OFFSETS.return) {
			return "$rt";
		}

		for (let i = 0; i < OFFSET_VALUES.length; i++) {
			if (OFFSET_VALUES[i] <= n) {
				const s = OFFSETS_INV[OFFSET_VALUES[i]][0];
				return "$" + (s.match(/^[ratskemf]$/)? s + (n - OFFSET_VALUES[i]).toString(16) : s);
			}
		}

		return null;
	}

	/**
	 * Converts an R-type instruction to its original wasm source.
	 * @param  {string} op The name of the operation.
	 * @param  {string} rt The name of the `rt` register.
	 * @param  {string} rs The name of the `rs` register.
	 * @param  {string} rd The name of the `rt` register.
	 * @param  {number} funct The ID of the function.
	 * @param  {number} [flags=0] The assembler flags.
	 * @param  {string} [conditions] The instruction conditions.
	 * @return {string} A line of wasm source.
	 */
	static formatR_w(op: string, rt: string, rs: string, rd: string, funct: number, flags: FlagValue = 0,
		conditions: ConditionName | null = null): string {
		const alt_op = (oper) => {
			if (rs == rd) return `${yellow(rs)} ${Parser.colorOper(oper + "=")} ${yellow(rt)}`;
			if (rt == rd) return `${yellow(rt)} ${Parser.colorOper(oper + "=")} ${yellow(rs)}`;
			return `${yellow(rs)} ${Parser.colorOper(oper)} ${yellow(rt)} ${dim("->")} ${yellow(rd)}`;
		};

		if (op == "add")   return alt_op("+");
		if (op == "sub")   return alt_op("-");
		if (op == "mult")  return `${yellow(rs)} ${Parser.colorOper("*")} ${yellow(rt)}`;
		if (op == "mod")   return alt_op("%");
		if (op == "div")   return alt_op("/");
		if (op == "divu")  return alt_op("/") + " /u";
		if (op == "and")   return alt_op("&");
		if (op == "nand")  return alt_op("~&");
		if (op == "nor")   return alt_op("~|");
		if (op == "not")   return `${Parser.colorOper("~") + yellow(rs)} ${dim("->")} ${yellow(rd)}`;
		if (op == "or")    return rs == "$0"? `${yellow(rt)} ${dim("->")} ${yellow(rd)}` : alt_op("|");
		if (op == "xnor")  return alt_op("~x");
		if (op == "xor")   return alt_op("x");
		if (op == "land")  return alt_op("&&");
		if (op == "lnand") return alt_op("!&&");
		if (op == "lnor")  return alt_op("!||");
		if (op == "lnot")  return Parser.colorOper("!") + yellow(rs) +
		                           (rs == rd? "." : ` ${dim("->")} ${yellow(rd)}`);
		if (op == "lor")   return rs == "$0"? `${yellow(rt)} ${dim("->")} ${yellow(rd)}` : alt_op("||");
		if (op == "lxnor") return alt_op("!xx");
		if (op == "lxor")  return alt_op("xx");
		if (op == "sll")   return alt_op("<<");
		if (op == "srl")   return alt_op(">>>");
		if (op == "sra")   return alt_op(">>");
		if (op == "mfhi")  return `${green("%hi")} ${dim("->")} ${yellow(rd)}`;
		if (op == "mflo")  return `${green("%lo")} ${dim("->")} ${yellow(rd)}`;
		if (op == "sl")    return `${yellow(rs)} ${Parser.colorOper("<") } ${yellow(rt)} ${dim("->")}`
		                        + ` ${yellow(rd)}`;
		if (op == "sle")   return `${yellow(rs)} ${Parser.colorOper("<=")} ${yellow(rt)} ${dim("->")}`
		                        + ` ${yellow(rd)}`;
		if (op == "seq")   return `${yellow(rs)} ${Parser.colorOper("==")} ${yellow(rt)} ${dim("->")}`
		                        + ` ${yellow(rd)}`;
		if (op == "jr")    return `${dim(":") } ${yellow(rd)}`;
		if (op == "jrl")   return `${dim("::")} ${yellow(rd)}`;
		if (op == "jrc")   return `${dim(":") } ${yellow(rd)} ${red("if")} ${yellow(rs)}`;
		if (op == "jrlc")  return `${dim("::")} ${yellow(rd)} ${red("if")} ${yellow(rs)}`;
		if (op == "c")     return `[${yellow(rs)}] ${dim("->")} [${yellow(rd)}]`;
		if (op == "l")     return `[${yellow(rs)}] ${dim("->")} ${ yellow(rd) }`;
		if (op == "s")     return `${ yellow(rs) } ${dim("->")} [${yellow(rd)}]`;
		if (op == "multu") return `${yellow(rs)} ${Parser.colorOper("*") } ${yellow(rt)} /u`;
		if (op == "slu")   return `${yellow(rs)} ${Parser.colorOper("<") } ${yellow(rt)} ${dim("->")}`
		                        + ` ${yellow(rd)} /u`;
		if (op == "sleu")  return `${yellow(rs)} ${Parser.colorOper("<=")} ${yellow(rt)} ${dim("->")}`
		                        + ` ${yellow(rd)} /u`;
		if (op == "ext")   return Parser.formatExt(rt, rs, rd, funct);
		if (op == "cb")    return `[${yellow(rs)}] ${dim("->")} [${yellow(rd)}] /b`;
		if (op == "lb")    return `[${yellow(rs)}] ${dim("->")} ${ yellow(rd) } /b`;
		if (op == "sb")    return `${ yellow(rs) } ${dim("->")} [${yellow(rd)}] /b`;
		if (op == "ch")    return `[${yellow(rs)}] ${dim("->")} [${yellow(rd)}] /h`;
		if (op == "lh")    return `[${yellow(rs)}] ${dim("->")} ${ yellow(rd) } /h`;
		if (op == "sh")    return `${ yellow(rs) } ${dim("->")} [${yellow(rd)}] /h`;
		if (op == "spush") return `${dim("[")} ${yellow(rs)}`;
		if (op == "spop")  return `\xa0\xa0${yellow(rd)} ${dim("]")}`;
		if (op == "time")  return `${cyan("time")} ${yellow(rs)}`;
		if (op == "ring")  return `${cyan("ring")} ${yellow(rs)}`;
		return `(unknown R-type: ${Parser.colorOper(op)})`;
	}

	/**
	 * Converts an I-type instruction to its original wasm source.
	 * @param  {string} op The name of the operation.
	 * @param  {string} rs The name of the `rs` register.
	 * @param  {string} rd The name of the `rt` register.
	 * @param  {number} imm An immediate value.
	 * @param  {number} [flags=0] The assembler flags.
	 * @param  {string} [conditions] The instruction conditions.
	 * @param  {SymbolTable} [symbols] A symbol table.
	 * @return {string} A line of wasm source.
	 */
	static formatI_w(op: string, rs: string, rd: string, imm: Long, flags: FlagValue = 0,
	                 conditions: ConditionName | null = null, symbols: SymbolTable = {}): string {
		const target = Parser.getTarget(imm, flags, symbols);

		const mathi = (increment: string, opequals: string, op: string): string => {
			const imms = imm.toString();
			if (rs == rd) {
				return imm.eq(1)? yellow(rd) + yellow.dim(increment)
				                : `${yellow(rs)} ${Parser.colorOper(opequals)} ${magenta(imms)}`;
			}

			return `${yellow(rs)} ${Parser.colorOper(op)} ${magenta(imms)} ${dim("->")} `
			        + yellow(rd);
		};

		const alt_op = (oper: string): string =>
			`${yellow(rs)} ${Parser.colorOper(oper + (rs == rd? "=" : ""))} ${magenta(target)}` +
			(rs != rd? dim(" -> ") + yellow(rd) : "");

		if (op == "addi")   return mathi("++", "+=", "+");
		if (op == "subi")   return mathi("--", "-=", "-");
		if (op == "multi")  return `${yellow(rs)} ${Parser.colorOper("*")} ${magenta(target)}`;
		if (op == "multui") return `${yellow(rs)} ${Parser.colorOper("*")} ${magenta(target)} /u`;
		if (op == "modi")   return alt_op("%");
		if (op == "divi")   return alt_op("/");
		if (op == "divui")  return alt_op("/") + " /u";
		if (op == "andi")   return alt_op("&");
		if (op == "nandi")  return alt_op("~&");
		if (op == "nori")   return alt_op("~|");
		if (op == "ori")    return alt_op("|");
		if (op == "xnori")  return alt_op("~x");
		if (op == "xori")   return alt_op("x");
		if (op == "slli")   return alt_op("<<");
		if (op == "srli")   return alt_op(">>>");
		if (op == "srai")   return alt_op(">>");
		if (op == "lui")    return `${dim("lui:")} ${magenta(target)} ${dim("->")} `
		                         + yellow(rd);
		if (op == "li")     return `[${magenta(target)}] ${dim("->")} ${yellow(rd)}`;
		if (op == "si")     return `${yellow(rs)} ${dim("->")} [${magenta(target)}]`;
		if (op == "set")    return `${magenta(target)} ${dim("->")} ${yellow(rd)}`;
		if (op == "sli")    return `${yellow(rs)} ${Parser.colorOper("<") } ${magenta(target)} `
		                         + `${dim("->")} ${yellow(rd)}`;
		if (op == "slei")   return `${yellow(rs)} ${Parser.colorOper("<=")} ${magenta(target)} `
		                         + `${dim("->")} ${yellow(rd)}`;
		if (op == "sgi")    return `${yellow(rs)} ${Parser.colorOper(">") } ${magenta(target)} `
		                         + `${dim("->")} ${yellow(rd)}`;
		if (op == "sgei")   return `${yellow(rs)} ${Parser.colorOper(">=")} ${magenta(target)} `
		                         + `${dim("->")} ${yellow(rd)}`;
		if (op == "seqi")   return `${yellow(rs)} ${Parser.colorOper("==")} ${magenta(target)} `
		                         + `${dim("->")} ${yellow(rd)}`;
		if (op == "slui")   return `${yellow(rs)} ${Parser.colorOper("<") } ${magenta(target)} `
		                         + `${dim("->")} ${yellow(rd)} /u`;
		if (op == "sleui")  return `${yellow(rs)} ${Parser.colorOper("<=")} ${magenta(target)} `
		                         + `${dim("->")} ${yellow(rd)} /u`;
		if (op == "lbi")    return `[${magenta(target)}] ${dim("->")} ${yellow(rd)} /b`;
		if (op == "sbi")    return `${yellow(rs)} ${dim("->")} [${magenta(target)}] /b`;
		if (op == "lni")    return `[${magenta(target)}] ${dim("->")} [${yellow(rd)}]`;
		if (op == "lbni")   return `[${magenta(target)}] ${dim("->")} [${yellow(rd)}] /b`;
		if (op == "int")    return `${cyan("int")} ${imm}`;
		if (op == "rit")    return `${cyan("rit")} ${magenta(target)}`;
		if (op == "timei")  return `${cyan("time")} ${imm}`;
		if (op == "ringi")  return `${cyan("ring")} ${imm}`;
		return `(unknown I-type: ${Parser.colorOper(op)})`;
	}

	/**
	 * Converts a J-type instruction to its original wasm source.
	 * @param  {string}  op   The name of the operation.
	 * @param  {string}  rs   The name of the `rs` register.
	 * @param  {number}  addr An immediate value.
	 * @param  {boolean} link Whether the link bit is set.
	 * @param  {number} [flags=0] The assembler flags.
	 * @param  {string} [conditions] The instruction conditions.
	 * @param  {SymbolTable} [symbols] A symbol table.
	 * @return {string} A line of wasm source.
	 */
	static formatJ_w(op: string, rs: string, addr: Long, link: boolean, flags: FlagValue = 0,
	                 conditions: ConditionName | null = null, symbols: SymbolTable = {}): string {
		const target = magenta(Parser.getTarget(addr, flags, symbols));
		const sym = link? "::" : ":";
		const cond = {"p": "+", "n": "-", "z": "0", "nz": "*"}[conditions] || "";
		if (op == "j")   return `${dim(cond + sym)} ${target}`;
		if (op == "jc")  return `${dim(sym)} ${target} ${red("if")} ${yellow(rs)}`;
		return `(unknown J-type: ${Parser.colorOper(op)})`;
	}

	/**
	 * Converts an R-type instruction to its mnemonic representation.
	 * @param  {string}  op The name of the operation.
	 * @param  {string}  rt The name of the `rt` register.
	 * @param  {string}  rs The name of the `rs` register.
	 * @param  {string}  rd The name of the `rt` register.
	 * @param  {number}  funct The ID of the function.
	 * @param  {number} [flags=0] The assembler flags.
	 * @param  {string} [conditions] The instruction conditions.
	 * @return {string} A mnemonic representation of the instruction.
	 */
	static formatR_m(op: string, rt: string, rs: string, rd: string, funct: number, flags: FlagValue = 0,
	                 conditions: ConditionName | null = null): string {
		let base = cyan(op);
		if (op == "ext") {
			base = cyan.bold(Object.keys(EXTS).filter(t => funct == EXTS[t])[0] || ("ext " + funct));
		}

		return `${base} ${yellow(rs)}${dim(",")} ${yellow(rt)} ${dim("->")} ${yellow(rd)}`;
	}

	/**
	 * Converts an I-type instruction to its mnemonic representation.
	 * @param  {string}  op  The name of the operation.
	 * @param  {string}  rs  The name of the `rs` register.
	 * @param  {string}  rd  The name of the `rt` register.
	 * @param  {number}  imm An immediate value.
	 * @param  {number} [flags=0] The assembler flags.
	 * @param  {string} [conditions] The instruction conditions.
	 * @param  {SymbolTable} [symbols] A symbol table.
	 * @return {string} A mnemonic representation of the instruction.
	 */
	static formatI_m(op: string, rs: string, rd: string, imm: Long, flags: FlagValue = 0,
	                 conditions: ConditionName | null = null, symbols: SymbolTable = {}): string {
		const target = Parser.getTarget(imm, flags, symbols);
		return `${cyan(op)} ${yellow(rs) + dim(",")} ${magenta(target)} ${dim("->")} ${yellow(rd)}`;
	}

	/**
	 * Converts a J-type instruction to its mnemonic representation.
	 * @param  {string}  op   The name of the operation.
	 * @param  {string}  rs   The name of the `rs` register.
	 * @param  {number}  addr An immediate value.
	 * @param  {boolean} link Whether the link bit is set.
	 * @param  {number} [flags=0] The assembler flags.
	 * @param  {string} [conditions] The instruction conditions.
	 * @param  {SymbolTable} [symbols] A symbol table.
	 * @return {string} A mnemonic representation of the instruction.
	 */
	static formatJ_m(op: string, rs: string, addr: Long, link: boolean, flags: FlagValue = 0,
	                 conditions: ConditionName | null = null, symbols: SymbolTable = {}): string {
		const target = magenta(Parser.getTarget(addr, flags, symbols));
		return `${cyan(op)}${conditions? cyan("_" + conditions) : ""} ${yellow(rs) + dim(",")} ${target}`;
	}

	static get formatStyle(): FormatStyle {
		return Parser._formatStyle;
	}

	static set formatStyle(to: FormatStyle) {
		Parser._formatStyle = to == "mnem"? to : "wasm";
		if (to == "mnem") {
			Parser.formatR = Parser.formatR_m;
			Parser.formatI = Parser.formatI_m;
			Parser.formatJ = Parser.formatJ_m;
		} else {
			Parser.formatR = Parser.formatR_w;
			Parser.formatI = Parser.formatI_w;
			Parser.formatJ = Parser.formatJ_w;
		}
	}

	/**
	 * Converts an external instruction to its original wasm source.
	 * @param  {string} rt The name of the `rt` register.
	 * @param  {string} rs The name of the `rs` register.
	 * @param  {string} rd The name of the `rt` register.
	 * @param  {number} funct The ID of the function.
	 * @return {string} A line of wasm source.
	 */
	static formatExt(rt: string, rs: string, rd: string, funct: number): string {
		if (funct == EXTS.printr) return `<${cyan("print")} ${yellow(rs)}>`;
		if (funct == EXTS.prc)    return `<${cyan("prc")} ${yellow(rs)}>`;
		if (funct == EXTS.prd)    return `<${cyan("prd")} ${yellow(rs)}>`;
		if (funct == EXTS.prx)    return `<${cyan("prx")} ${yellow(rs)}>`;
		if (funct == EXTS.halt)   return `<${cyan("halt")}>`;
		if (funct == EXTS.sleep)  return `<${cyan("sleep")} ${yellow(rs)}>`;
		return `<${cyan("ext")} ${red(funct)}>`;
	}

	static getTarget(imm: Long, flags: FlagValue, symbols: SymbolTable): string {
		if (flags != FLAGS.KNOWN_SYMBOL) {
			return imm.toString();
		}

		const key = _.findKey(symbols, (s) => s[1].eq(imm));
		return key? "&" + key : imm.toString();
	}
}

if (require.main === module) {
	const opt = minimist(process.argv.slice(2), {
		alias: {s: "single"},
		boolean: ["single"],
		default: {single: false}
	}), name = opt._[0];

	if (!name) {
		console.log("Usage: node parser.js [filename]");
		process.exit(0);
	}

	if (opt.single) {
		let num: number;
		if (name.match(/^0x/)) {
			num = parseInt(name.substr(2), 16);
		} else if (name.match(/^0b/)) {
			num = parseInt(name.substr(2), 2);
		} else {
			num = parseInt(name);
		}

		console.log(Parser.formatInstruction(num.toString(2).padStart(64, "0"), {}));
	} else {
		new Parser().open(name, false);
	}
}
