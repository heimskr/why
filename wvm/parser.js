#!/usr/bin/env node
let fs = require("fs"),
	Long = require("long"),
	_ = require("lodash"),
	minimist = require("minimist");

/**
 * @module wasm
 */

const chalk = new (require("chalk")).constructor({enabled: true});

require("../util.js").mixins(_);
require("string.prototype.padstart").shim();
require("string.prototype.padend").shim();

const {EXCEPTIONS, R_TYPES, I_TYPES, J_TYPES, OPCODES, FUNCTS, REGISTER_OFFSETS, TRAPS, CONDITIONS} = require("../wasm/constants.js");
const OPCODES_INV = _.multiInvert(OPCODES);
const OFFSETS_INV = _.multiInvert(REGISTER_OFFSETS);
const CONDITIONS_INV = _.invert(CONDITIONS);
const OFFSET_VALUES = _.uniq(Object.values(REGISTER_OFFSETS)).sort((a, b) => b - a);

/**
 * Contains code for parsing compiled wvm bytecode.
 */
class Parser {
	/**
	 * Constructs a new `Parser` instance.
	 * @param {Long[]} [longs] An optional array of Longs that will be passed to {@link module:wasm~Parser#load load} if specified.
	 */
	constructor(longs) {
		if (longs instanceof Array) {
			this.load(longs);
		}
	}

	/**
	 * Loads a program from a file and parses it.
	 * @param {string} filename The filename of the program to load.
	 * @param {boolean} [silent=true] Whether to suppress debug output.
	 */
	open(filename, silent=true) {
		this.read(fs.readFileSync(filename, "utf8"), silent);
	}

	/**
	 * Parses a string representation of a program.
	 * @param {string} text A text representation (\n-separated hexadecimal numbers) of a program.
	 * @param {boolean} [silent=true] Whether to suppress debug output.
	 */
	read(text, silent=true) {
		this.load(text.split("\n").map((s) => Long.fromString(s, true, 16)), silent);
	}

	/**
	 * Stores an array of longs and {@link module:wasm~Parser#parse parses} them.
	 * @param {Long[]} longs An array of Longs.
	 * @param {boolean} [silent=true] Whether to suppress debug output.
	 */
	load(longs, silent=true) {
		this.raw = [...longs];
		this.parsed = this.parse(longs, silent);
	}

	/**
	 * Parses the stored raw longs.
	 * @param {boolean} [silent=true] Whether to suppress debug output.
	 */
	parse(silent=true) {
		const longs = this.raw;

		/**
		 * The offsets defined in the program's metadata section.
		 * @type {Object<string, number>}
		 */
		this.offsets = {
			$symtab: longs[0].toInt(),
			$handlers: longs[1].toInt(),
			$code: longs[2].toInt(),
			$data: longs[3].toInt(),
			$end: longs[4].toInt()
		};

		/**
		 * Contains all the unparsed Longs in the handlers section.
		 * @type {Long[]}
		 */
		this.rawHandlers = longs.slice(this.offsets.$handlers / 8, this.offsets.$code / 8);

		/**
		 * An object mapping handler names to addresses.
		 * @type {Object<string, number>}
		 */
		this.handlers = _.fromPairs(this.rawHandlers.map((x, i) => [EXCEPTIONS[i], x]));

		/**
		 * Contains all the unparsed Longs in the metadata section.
		 * @type {Long[]}
		 */
		this.rawMeta = longs.slice(0, this.offsets.$symtab / 8);

		/**
		 * The parsed metadata section.
		 * @type {Object}
		 */
		this.meta = {
			orcid: _.chain([4, 5]).map((n) => longs[n]).longString().join("").chunk(4).map((n) => n.join("")).join("-").value()
		};
		[this.meta.name, this.meta.version, this.meta.author] = _(longs).slice(6, this.offsets.$handlers).longStrings();

		/**
		 * Contains all the unparsed Longs in the symbol table.
		 * @type {Long[]}
		 */
		this.rawSymbols = longs.slice(this.offsets.$symtab / 8, this.offsets.$handlers / 8);

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
			console.log(chalk.dim("/*"));
			console.log(chalk.green.dim("#offsets"));
			console.log(Object.keys(this.offsets).map((k) => `${chalk.dim(`${k}:`)} ${chalk.magenta.dim(this.offsets[k])}`).join("\n"));
			console.log(chalk.dim("*/") + "\n");

			console.log(chalk.green("#meta"));
			console.log(Object.keys(this.meta).map((k) => `${chalk.cyan(k)}: ${chalk.yellow(`"${this.meta[k]}"`)}`).concat([""]).join("\n"));

			console.log(chalk.green("#handlers"));
			console.log(this.handlers.map(([k, v]) => `${k}: ${chalk.magenta(v)}`).join("\n"));

			console.log([, chalk.green("#code"), ...this.code].join("\n"));
		}
	}

	/**
	 * Reads the program's symbol table.
	 * @return {SymbolTable} An object mapping a symbol name to tuple of its ID and its address.
	 */
	getSymbols() {
		const longs = this.raw;
		const start = longs[0].toInt() / 8;
		const end = longs[1].toInt() / 8;
		const out = {};
		for (let i = start, j = 0; i < end && j < 10000; j++) {
			const id = Math.abs(longs[i].high);
			const len = longs[i].low;
			const addr = longs[i + 1];

			const encodedName = longs.slice(i + 2, i + 2 + len).map((l) => l.toString(16).padStart(16, "0")).join("").replace(/(00)+$/, "");
			const name = _.chunk(encodedName, 2).map((x) => String.fromCharCode(parseInt(x.join(""), 16))).join("");

			out[name] = [id, addr];

			i += 2 + len;
		}

		return out;
	}

	/**
	 * Finds the length of the metadata section of the program.
	 * @return {number} The length of the metadata section in bytes.
	 */
	getMetaLength() {
		return this.raw[0];
	}

	/**
	 * Finds the length of the symbol table of the program.
	 * @return {number} The length of the symbol table in bytes.
	 */
	getSymbolTableLength() {
		return this.raw[1].subtract(this.raw[0]).toNumber();
	}

	/**
	 * Finds the length of the handlers section of the program.
	 * @return {number} The length of the handlers section in bytes.
	 */
	getHandlersLength() {
		return this.raw[2].subtract(this.raw[1]).toNumber();
	}

	/**
	 * Finds the length of the code section of the program.
	 * @return {number} The length of the code section in bytes.
	 */
	getCodeLength() {
		return this.raw[3].subtract(this.raw[2]).toNumber();
	}

	/**
	 * Finds the length of the data section of the program.
	 * @return {number} The length of the data section in bytes.
	 */
	getDataLength() {
		return this.raw[4].subtract(this.raw[3]).toNumber();
	}

	/**
	 * Parses a single instruction.
	 * @param  {Long|string} instruction An instruction represented as either a Long of a 64-long string of binary digits.
	 * @return {Object} An object containing information about the instruction. Always contains `op`, `opcode`, `rs`,
	 *                  `flags` and `type` for non-NOPs; other output varies depending on the type of the instruction.
	 */
	static parseInstruction(instruction) {
		if (instruction instanceof Long) {
			instruction = instruction.toString(2).padStart(64, "0");
		}

		const get = (...args) => parseInt(instruction.substr(...args), 2);
		const opcode = get(0, 12);
		const type = Parser.instructionType(opcode);

		if (type == "r") {
			const funct = get(52);
			return {
				op: opcode == OPCODES.trap? "trap" : OPCODES_INV[opcode].filter((op) => OPCODES_INV[opcode].length == 1 || Parser.instructionType(opcode) == "r" && FUNCTS[op] == funct)[0],
				opcode,
				rt: get(12, 7),
				rs: get(19, 7),
				rd: get(26, 7),
				funct,
				conditions: CONDITIONS_INV[get(46, 4)],
				flags: get(50, 2),
				type: "r"
			};
		} else if (type == "i") {
			return {
				op: OPCODES_INV[opcode][0],
				opcode,
				conditions: CONDITIONS_INV[get(12, 4)],
				flags: get(16, 2),
				rs: get(18, 7),
				rd: get(25, 7),
				imm: Long.fromString(instruction.substr(32), false, 2).toInt(),
				type: "i"
			};
		} else if (type == "j") {
			return {
				op: OPCODES_INV[opcode][0],
				opcode,
				rs: get(12, 7),
				link: get(19, 1),
				conditions: CONDITIONS_INV[get(26, 4)],
				flags: get(30, 2),
				addr: Long.fromString(instruction.substr(32), false, 2).toInt(),
				type: "j"
			};
		} else if (opcode == 0) {
			return {op: "nop", opcode};
		}
	}

	/**
	 * Styles an operator.
	 * @param  {string} oper An operator.
	 * @return {string} A styled operator.
	 */
	static colorOper(oper) {
		return chalk.bold(oper);
	}

	/**
	 * Decompiles an instruction to the corresponding wasm source.
	 * @param  {Long|string} instruction An instruction represented as either a Long of a 64-long string of binary digits.
	 * @param  {SymbolTable} [symbols] A symbol table.
	 * @return {string} The wasm equivalent of the instruction.
	 */
	static formatInstruction(instruction, symbols) {
		if (instruction instanceof Long) {
			instruction = instruction.toString(2).padStart(64, "0");
		}

		const {opcode, flags, type, funct, rt, rs, rd, imm, addr, link, conditions} = Parser.parseInstruction(instruction);

		if (opcode == 0) {
			return "<>";
		}

		const srs = Parser.getRegister(rs);

		if (type == "j") {
			return Parser.formatJ(OPCODES_INV[opcode][0], srs, addr, link, flags, conditions, symbols);
		}

		const srd = Parser.getRegister(rd);

		if (type == "r") {
			const srt = Parser.getRegister(rt);
			return Parser.formatR(OPCODES_INV[opcode].filter((op) => op == "trap" || FUNCTS[op] == funct)[0], srt, srs, srd, funct, flags, conditions);
		}
		
		if (type == "i") {
			return Parser.formatI(OPCODES_INV[opcode][0], srs, srd, imm, flags, conditions, symbols);
		}

		throw new Error(`Can't parse instruction ${instruction} (opcode = ${opcode}, type = "${type}").`);
	}

	/**
	 * Returns the instruction type corresponding to an opcode.
	 * @param  {number} opcode An opcode.
	 * @return {?("r"|"i"|"j")} One of the operation types (r, i, j) if the opcode was recognized; `undefined` otherwise.
	 */
	static instructionType(opcode) {
		if (R_TYPES.includes(opcode)) return "r";
		if (I_TYPES.includes(opcode)) return "i";
		if (J_TYPES.includes(opcode)) return "j";
	}

	/**
	 * Converts a numeric register ID to its string representation.
	 * @param  {number} n A register ID.
	 * @return {string} A register name.
	 */
	static getRegister(n) {
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
	static formatR_w(op, rt, rs, rd, funct, flags=0, conditions=null) {
		const alt_op = (oper) => {
			if (rs == rd) return `${chalk.yellow(rs)} ${Parser.colorOper(oper + "=")} ${chalk.yellow(rt)}`;
			if (rt == rd) return `${chalk.yellow(rt)} ${Parser.colorOper(oper + "=")} ${chalk.yellow(rs)}`;
			return `${chalk.yellow(rs)} ${Parser.colorOper(oper)} ${chalk.yellow(rt)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		};

		if (op == "add")   return alt_op("+");
		if (op == "sub")   return alt_op("-");
		if (op == "mult")  return `${chalk.yellow(rs)} ${Parser.colorOper("*")} ${chalk.yellow(rt)}`;
		if (op == "mod")   return alt_op("%");
		if (op == "and")   return alt_op("&");
		if (op == "nand")  return alt_op("~&");
		if (op == "nor")   return alt_op("~|");
		if (op == "not")   return `${Parser.colorOper("~") + chalk.yellow(rs)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "or")    return rs == "$0"? `${chalk.yellow(rt)} ${chalk.dim("->")} ${chalk.yellow(rd)}` : alt_op("|");
		if (op == "xnor")  return alt_op("~x");
		if (op == "xor")   return alt_op("x");
		if (op == "land")  return alt_op("&&");
		if (op == "lnand") return alt_op("!&&");
		if (op == "lnor")  return alt_op("!||");
		if (op == "lnot")  return rs == rd? `${Parser.colorOper("!") + chalk.yellow(rs)}.` : `${Parser.colorOper("!") + chalk.yellow(rs)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "lor")   return rs == "$0"? `${chalk.yellow(rt)} ${chalk.dim("->")} ${chalk.yellow(rd)}` : alt_op("||");
		if (op == "lxnor") return alt_op("!xx");
		if (op == "lxor")  return alt_op("xx");
		if (op == "sll")   return alt_op("<<");
		if (op == "srl")   return alt_op(">>>");
		if (op == "sra")   return alt_op(">>");
		if (op == "mfhi")  return `${chalk.green("%hi")} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "mflo")  return `${chalk.green("%lo")} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "sl")    return `${chalk.yellow(rs)} ${Parser.colorOper("<") } ${chalk.yellow(rt)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "sle")   return `${chalk.yellow(rs)} ${Parser.colorOper("<=")} ${chalk.yellow(rt)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "seq")   return `${chalk.yellow(rs)} ${Parser.colorOper("==")} ${chalk.yellow(rt)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "jr")    return `${chalk.dim(":") } ${chalk.yellow(rd)}`;
		if (op == "jrl")   return `${chalk.dim("::")} ${chalk.yellow(rd)}`;
		if (op == "jrc")   return `${chalk.dim(":") } ${chalk.yellow(rd)} ${chalk.red("if")} ${chalk.yellow(rs)}`;
		if (op == "jrlc")  return `${chalk.dim("::")} ${chalk.yellow(rd)} ${chalk.red("if")} ${chalk.yellow(rs)}`;
		if (op == "c")     return `[${chalk.yellow(rs)}] ${chalk.dim("->")} [${chalk.yellow(rd)}]`;
		if (op == "l")     return `[${chalk.yellow(rs)}] ${chalk.dim("->")} ${ chalk.yellow(rd)}`;
		if (op == "s")     return `${ chalk.yellow(rs) } ${chalk.dim("->")} [${chalk.yellow(rd)}]`;
		if (op == "addu")  return `${alt_op("+")} /u`;
		if (op == "subu")  return `${alt_op("-")} /u`;
		if (op == "multu") return `${chalk.yellow(rs)} ${Parser.colorOper("*") } ${chalk.yellow(rt)} /u`;
		if (op == "slu")   return `${chalk.yellow(rs)} ${Parser.colorOper("<") } ${chalk.yellow(rt)} ${chalk.dim("->")} ${chalk.yellow(rd)} /u`;
		if (op == "sleu")  return `${chalk.yellow(rs)} ${Parser.colorOper("<=")} ${chalk.yellow(rt)} ${chalk.dim("->")} ${chalk.yellow(rd)} /u`;
		if (op == "trap")  return Parser.formatTrap(rt, rs, rd, funct);
		if (op == "cb")    return `[${chalk.yellow(rs)}] ${chalk.dim("->")} [${chalk.yellow(rd)}] /b`;
		if (op == "lb")    return `[${chalk.yellow(rs)}] ${chalk.dim("->")} ${ chalk.yellow(rd)} /b`;
		if (op == "sb")    return `${ chalk.yellow(rs) } ${chalk.dim("->")} [${chalk.yellow(rd)}] /b`;
		if (op == "spush") return `${chalk.dim("[")} ${chalk.yellow(rs)}`;
		if (op == "spop")  return `\xa0\xa0${chalk.yellow(rd)} ${chalk.dim("]")}`;
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
	static formatI_w(op, rs, rd, imm, flags=0, conditions=null, symbols={}) {
		const target = flags == 1 && _.findKey(symbols, (s) => s[1].eq(imm)) || imm;

		const mathi = (increment, opequals, op) => {
			if (rs == rd) {
				return imm == 1? chalk.yellow(rd) + chalk.yellow.dim(increment) : `${chalk.yellow(rs)} ${Parser.colorOper(opequals)} ${chalk.magenta(imm)}`;
			}

			return `${chalk.yellow(rs)} ${Parser.colorOper(op)} ${chalk.magenta(imm)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		};

		const alt_op = (oper) => `${chalk.yellow(rs)} ${Parser.colorOper(oper + (rs == rd? "=" : ""))} ${chalk.magenta(target) + (rs != rd? chalk.dim(" -> ") + chalk.yellow(rd) : "")}`;
		if (op == "addi")   return mathi("++", "+=", "+");
		if (op == "subi")   return mathi("--", "-=", "-");
		if (op == "addui")  return `${mathi("++", "+=", "+")} /u`;
		if (op == "subui")  return `${mathi("--", "-=", "-")} /u`;
		if (op == "multi")  return `${chalk.yellow(rs)} ${Parser.colorOper("*")} ${chalk.magenta(target)}`;
		if (op == "multui") return `${chalk.yellow(rs)} ${Parser.colorOper("*")} ${chalk.magenta(target)} /u`;
		if (op == "modi")   return alt_op("%");
		if (op == "andi")   return alt_op("&");
		if (op == "nandi")  return alt_op("~&");
		if (op == "nori")   return alt_op("~|");
		if (op == "ori")    return alt_op("|");
		if (op == "xnori")  return alt_op("~x");
		if (op == "xori")   return alt_op("x");
		if (op == "slli")   return alt_op("<<");
		if (op == "srli")   return alt_op(">>>");
		if (op == "srai")   return alt_op(">>");
		if (op == "lui")    return `${chalk.dim("lui:")} ${chalk.magenta(target)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "li")     return `[${chalk.magenta(target)}] ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "si")     return `${chalk.yellow(rs)} ${chalk.dim("->")} [${chalk.magenta(target)}]`;
		if (op == "set")    return `${chalk.magenta(target)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "sli")    return `${chalk.yellow(rs)} ${Parser.colorOper("<") } ${chalk.magenta(target)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "slei")   return `${chalk.yellow(rs)} ${Parser.colorOper("<=")} ${chalk.magenta(target)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "sgi")    return `${chalk.yellow(rs)} ${Parser.colorOper(">") } ${chalk.magenta(target)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "sgei")   return `${chalk.yellow(rs)} ${Parser.colorOper(">=")} ${chalk.magenta(target)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "seqi")   return `${chalk.yellow(rs)} ${Parser.colorOper("==")} ${chalk.magenta(target)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "slui")   return `${chalk.yellow(rs)} ${Parser.colorOper("<") } ${chalk.magenta(target)} ${chalk.dim("->")} ${chalk.yellow(rd)} /u`;
		if (op == "sleui")  return `${chalk.yellow(rs)} ${Parser.colorOper("<=")} ${chalk.magenta(target)} ${chalk.dim("->")} ${chalk.yellow(rd)} /u`;
		if (op == "lbi")    return `[${chalk.magenta(target)}] ${chalk.dim("->")} ${chalk.yellow(rd)} /b`;
		if (op == "sbi")    return `${chalk.yellow(rs)} ${chalk.dim("->")} [${chalk.magenta(target)}] /b`;
		if (op == "lni")    return `[${chalk.magenta(target)}] ${chalk.dim("->")} [${chalk.yellow(rd)}]`;
		if (op == "lbni")   return `[${chalk.magenta(target)}] ${chalk.dim("->")} [${chalk.yellow(rd)}] /b`;
		return `(unknown I-type: ${Parser.colorOper(op)})`;
	}

	/**
	 * Converts a J-type instruction to its original wasm source.
	 * @param  {string} op The name of the operation.
	 * @param  {string} rs The name of the `rs` register.
	 * @param  {number} addr An immediate value.
	 * @param  {boolean} link Whether the link bit is set.
	 * @param  {number} [flags=0] The assembler flags.
	 * @param  {string} [conditions] The instruction conditions.
	 * @param  {SymbolTable} [symbols] A symbol table.
	 * @return {string} A line of wasm source.
	 */
	static formatJ_w(op, rs, addr, link, flags=0, conditions=null, symbols={}) {
		const target = chalk.magenta(flags == 1 && _.findKey(symbols, (s) => s[1].eq(addr)) || addr);
		const sym = link? "::" : ":";
		const cond = {"p": "+", "n": "-", "z": "0", "nz": "*"}[conditions] || "";
		if (op == "j")   return `${chalk.dim(cond + sym)} ${target}`;
		if (op == "jc")  return `${chalk.dim(sym)} ${target} ${chalk.red("if")} ${chalk.yellow(rs)}`;
		return `(unknown J-type: ${Parser.colorOper(op)})`;
	}

	/**
	 * Converts an R-type instruction to its mnemonic representation.
	 * @param  {string} op The name of the operation.
	 * @param  {string} rt The name of the `rt` register.
	 * @param  {string} rs The name of the `rs` register.
	 * @param  {string} rd The name of the `rt` register.
	 * @param  {number} funct The ID of the function.
	 * @param  {number} [flags=0] The assembler flags.
	 * @param  {string} [conditions] The instruction conditions.
	 * @return {string} A mnemonic representation of the instruction.
	 */
	static formatR_m(op, rt, rs, rd, funct, flags=0, conditions=null) {
		let base;
		
		if (op == "trap") {
			base = chalk.cyan(Object.keys(TRAPS).filter(t => funct == TRAPS[t])[0] || ("trap " + funct));
		} else {
			base = chalk.cyan(op);
		}

		let includeRs = true, includeRt = true;
		if (op == "trap") {
			if (funct == TRAPS.halt) {
				includeRs = includeRt = false;
			} else if (funct == TRAPS.prd || funct == TRAPS.prc || funct == TRAPS.prx) {
				includeRt = false;
			}
		}

		if (includeRs) {
			base += " " + chalk.yellow(rs);

			if (includeRt) {
				base += chalk.dim(",") + " " + chalk.yellow(rt);
			}
		}
		
		if (rd == "$0") {
			return base;
		} else {
			return `${base} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		}
	}

	/**
	 * Converts an I-type instruction to its mnemonic representation.
	 * @param  {string} op The name of the operation.
	 * @param  {string} rs The name of the `rs` register.
	 * @param  {string} rd The name of the `rt` register.
	 * @param  {number} imm An immediate value.
	 * @param  {number} [flags=0] The assembler flags.
	 * @param  {string} [conditions] The instruction conditions.
	 * @param  {SymbolTable} [symbols] A symbol table.
	 * @return {string} A mnemonic representation of the instruction.
	 */
	static formatI_m(op, rs, rd, imm, flags=0, conditions=null, symbols={}) {
		const target = flags == 1 && _.findKey(symbols, (s) => s[1].eq(imm)) || imm;
		
		let base = `${chalk.cyan(op)} `;
		
		if (rs != "$0") {
			base += `${chalk.yellow(rs) + chalk.dim(",")} `;
		}
		
		base += chalk.magenta(target);
		
		if (rd == "$0") {
			return base;
		} else {
			return base + `${chalk.dim(",")} ${chalk.yellow(rd)}`;
		}
	}

	/**
	 * Converts a J-type instruction to its mnemonic representation.
	 * @param  {string} op The name of the operation.
	 * @param  {string} rs The name of the `rs` register.
	 * @param  {number} addr An immediate value.
	 * @param  {boolean} link Whether the link bit is set.
	 * @param  {number} [flags=0] The assembler flags.
	 * @param  {string} [conditions] The instruction conditions.
	 * @param  {SymbolTable} [symbols] A symbol table.
	 * @return {string} A mnemonic representation of the instruction.
	 */
	static formatJ_m(op, rs, addr, link, flags=0, conditions=null, symbols={}) {
		const target = chalk.magenta(flags == 1 && _.findKey(symbols, (s) => s[1].eq(addr)) || addr);

		
		const cond = {"p": "+", "n": "-", "z": "0", "nz": "*"}[conditions] || "";
		return `${chalk.cyan(op)}${conditions? chalk.cyan("_" + conditions) : ""} ${chalk.yellow(rs)}, ${target}`;

		if (op == "j")   return `${chalk.dim(cond + sym)} ${target}`;
		if (op == "jc")  return `${chalk.dim(sym)} ${target} ${chalk.red("if")} ${chalk.yellow(rs)}`;
		return `(unknown J-type: ${Parser.colorOper(op)})`;
	}

	static formatR = Parser.formatR_m;
	static formatI = Parser.formatI_m;
	static formatJ = Parser.formatJ_m;

	/**
	 * Converts a trap instruction to its original wasm source.
	 * @param  {string} rt The name of the `rt` register.
	 * @param  {string} rs The name of the `rs` register.
	 * @param  {string} rd The name of the `rt` register.
	 * @param  {number} funct The ID of the function.
	 * @return {string} A line of wasm source.
	 */
	static formatTrap(rt, rs, rd, funct) {
		if (funct == TRAPS.printr) return `<${chalk.cyan("print")} ${chalk.yellow(rs)}>`;
		if (funct == TRAPS.prc)    return `<${chalk.cyan("prc")} ${chalk.yellow(rs)}>`;
		if (funct == TRAPS.prd)    return `<${chalk.cyan("prd")} ${chalk.yellow(rs)}>`;
		if (funct == TRAPS.prx)    return `<${chalk.cyan("prx")} ${chalk.yellow(rs)}>`;
		if (funct == TRAPS.halt)   return `<${chalk.cyan("halt")}>`;
		return `<${chalk.bold("trap")} ${chalk.red(funct)}>`;
	}
}

module.exports = Parser;

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
		let num = name;
		if (typeof name == "string") {
			if (name.match(/^0x/)) {
				num = parseInt(name.substr(2), 16);
			} else if (name.match(/^0b/)) {
				num = parseInt(name.substr(2), 2);
			} else {
				num = parseInt(name);
			}
		}

		console.log(Parser.formatInstruction(num.toString(2).padStart(64, "0")));
	} else {
		Parser.open(name, false);
	}
}
