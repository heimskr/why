#!/usr/bin/env node
let fs = require("fs"),
	Long = require("long"),
	_ = require("lodash"),
	minimist = require("minimist");

const chalk = new (require("chalk")).constructor({enabled: true});

require("../util.js").mixins(_);
require("string.prototype.padstart").shim();
require("string.prototype.padend").shim();

const {EXCEPTIONS, R_TYPES, I_TYPES, J_TYPES, OPCODES, FUNCTS, REGISTER_OFFSETS, TRAPS} = require("../wasm/constants.js");
const SEDOCPO = _.multiInvert(OPCODES);
const STESFFO = _.multiInvert(REGISTER_OFFSETS);
const OFFSET_VALUES = _.uniq(Object.values(REGISTER_OFFSETS)).sort((a, b) => b - a);

module.exports = class Parser {
	constructor(longs) {
		if (longs instanceof Array) {
			this.load(longs);
		}
	}

	open(filename, silent = true) {
		return this.read(fs.readFileSync(filename, "utf8"), silent);
	}

	read(text, silent=true) {
		return this.load(text.split("\n").map((s) => Long.fromString(s, true, 16)));
	}

	load(longs, silent=true) {
		this.raw = [...longs];
		this.parsed = this.parse(longs, silent);

		return {
			parsed: this.parsed,
			raw: this.raw
		};
	}

	parse(silent=true) {
		const longs = this.raw;

		this.offsets = {
			$symtab: longs[0].toInt(),
			$handlers: longs[1].toInt(),
			$code: longs[2].toInt(),
			$data: longs[3].toInt(),
			$end: longs[4].toInt()
		};

		this.handlers = longs.slice(this.offsets.$handlers, this.offsets.$data).map((x, i) => [EXCEPTIONS[i], x.toInt()]);

		this.rawMeta = longs.slice(0, this.offsets.$symtab / 8);
		this.meta = {
			orcid: _.chain([4, 5]).map((n) => longs[n]).longString().join("").chunk(4).map((n) => n.join("")).join("-").value()
		};
		[this.meta.name, this.meta.version, this.meta.author] = _(longs).slice(6, this.offsets.$handlers).longStrings();

		this.rawSymbols = longs.slice(this.offsets.$symtab / 8, this.offsets.$handlers / 8);
		this.symbols = this.getSymbols();

		this.rawCode = longs.slice(this.offsets.$code / 8, this.offsets.$end / 8);
		this.code = this.rawCode.map(Parser.parseInstruction);

		this.rawData = longs.slice(this.offsets.$data / 8, this.offsets.$end / 8);



		if (!silent) {
			console.log(chalk.dim("/*"));
			console.log(chalk.green.dim("#offsets"));
			console.log(Object.keys(this.offsets).map((k) => `${chalk.dim(`${k}:`)} ${chalk.magenta.dim(this.offsets[k])}`).join("\n"));
			console.log(chalk.dim("*/") + "\n");

			console.log(chalk.green("#meta"));
			console.log(Object.keys(meta).map((k) => `${chalk.cyan(k)}: ${chalk.yellow(`"${meta[k]}"`)}`).concat([""]).join("\n"));

			console.log(chalk.green("#handlers"));
			console.log(handlers.map(([k, v]) => `${k}: ${chalk.magenta(v)}`).join("\n"));

			console.log([, chalk.green("#code"), ...code].join("\n"));
		}
	}

	getSymbols() {
		const longs = this.raw;
		const start = longs[0].toInt() / 8;
		const end = longs[1].toInt() / 8;
		const out = {};
		for (let i = start, j = 0; i < end && j < 10000; j++) {
			const id = longs[i].high;
			const len = longs[i].low;
			const addr = longs[i + 1];

			const encodedName = longs.slice(i + 2, i + 2 + len).map((l) => l.toString(16).padStart(16, "0")).join("").replace(/(00)+$/, "");
			const name = _.chunk(encodedName, 2).map((x) => String.fromCharCode(parseInt(x.join(""), 16))).join("");

			out[name] = [id, addr];

			i += 2 + len;
		}

		return out;
	}

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
				op: opcode == OPCODES.trap? "trap" : SEDOCPO[opcode].filter((op) => SEDOCPO[opcode].length == 1 || Parser.instructionType(opcode) == "r" && FUNCTS[op] == funct)[0],
				opcode,
				rt: get(12, 7),
				rs: get(19, 7),
				rd: get(26, 7),
				funct,
				flags: get(49, 3),
				type: "r"
			};
		} else if (type == "i") {
			return {
				op: SEDOCPO[opcode][0],
				opcode,
				rs: get(18, 7),
				rd: get(25, 7),
				imm: Long.fromString(instruction.substr(32), false, 2).toInt(),
				flags: get(15, 3),
				type: "i"
			};
		} else if (type == "j") {
			return {
				op: SEDOCPO[opcode][0],
				opcode,
				rs: get(12, 7),
				addr: get(32),
				flags: get(29, 3),
				type: "j"
			};
		} else if (opcode == 0) {
			return {op: "nop", opcode};
		}
	}

	static colorOper(oper) {
		return chalk.bold(oper);
	}

	static formatInstruction(instruction) {
		if (instruction instanceof Long) {
			instruction = instruction.toString(2).padStart(64, "0");
		}

		const get = (...args) => parseInt(instruction.substr(...args), 2);
		const opcode = get(0, 12);
		const type = Parser.instructionType(opcode);

		if (type == "r") {
			const funct = get(52);
			return Parser.formatR(SEDOCPO[opcode].filter((op) => Parser.instructionType(opcode) == "r" && (opcode == OPCODES.trap || FUNCTS[op] == funct))[0], Parser.getRegister(get(12, 7)), Parser.getRegister(get(19, 7)), Parser.getRegister(get(26, 7)), funct);
		} else if (type == "i") {
			return Parser.formatI(SEDOCPO[opcode][0], Parser.getRegister(get(18, 7)), Parser.getRegister(get(25, 7)), Long.fromString(instruction.substr(32), false, 2).toInt());
		} else if (type == "j") {
			return Parser.formatJ(SEDOCPO[opcode][0], Parser.getRegister(get(12, 7)), get(32));
		} else if (opcode == 0) {
			return "<>";
		}

		throw new Error(`Can't parse instruction ${instruction} (opcode = ${opcode}, type = "${type}").`);
	}

	static instructionType(opcode) {
		if (R_TYPES.includes(opcode)) return "r";
		if (I_TYPES.includes(opcode)) return "i";
		if (J_TYPES.includes(opcode)) return "j";
	}

	static getRegister(n) {
		if (n == REGISTER_OFFSETS.return) {
			return "$rt";
		}

		for (let i = 0; i < OFFSET_VALUES.length; i++) {
			if (OFFSET_VALUES[i] <= n) {
				const s = STESFFO[OFFSET_VALUES[i]][0];
				return "$" + (s.match(/^[ratskemf]$/)? s + (n - OFFSET_VALUES[i]).toString(16) : s);
			}
		}

		return null;
	}

	static format(instruction) {
		const {type, op, rt, rs, rd, funct, imm, addr} = instruction;
		if (type == "r") {
			return Parser.formatR(op, rt, rs, rd, funct);
		} else if (type == "i") {
			return Parser.formatI(op, rs, rd, imm);
		} else if (type == "j") {
			return Parser.formatJ(op, rs, addr);
		}

		return "?";
	}

	static formatR(op, rt, rs, rd, funct) {
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
		if (op == "spop")  return `${chalk.dim("]")} ${chalk.yellow(rd)}`;
		return `(unknown R-type: ${Parser.colorOper(op)})`;
	}

	static formatI(op, rs, rd, imm) {
		const mathi = (increment, opequals, op) => {
			if (rs == rd) {
				return imm == 1? chalk.yellow(rd) + chalk.yellow.dim(increment) : `${chalk.yellow(rs)} ${Parser.colorOper(opequals)} ${chalk.magenta(imm)}`;
			}

			return `${chalk.yellow(rs)} ${Parser.colorOper(op)} ${chalk.magenta(imm)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		};

		const alt_op = (oper) => `${chalk.yellow(rs)} ${Parser.colorOper(oper + (rs == rd? "=" : ""))} ${chalk.magenta(imm) + (rs != rd? chalk.dim(" -> ") + chalk.yellow(rd) : "")}`;
		if (op == "addi")   return mathi("++", "+=", "+");
		if (op == "subi")   return mathi("--", "-=", "-");
		if (op == "addui")  return `${mathi("++", "+=", "+")} /u`;
		if (op == "subui")  return `${mathi("--", "-=", "-")} /u`;
		if (op == "multi")  return `${chalk.yellow(rs)} ${Parser.colorOper("*")} ${chalk.magenta(imm)}`;
		if (op == "multui") return `${chalk.yellow(rs)} ${Parser.colorOper("*")} ${chalk.magenta(imm)} /u`;
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
		if (op == "lui")    return `${chalk.dim("lui:")} ${chalk.magenta(imm)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "li")     return `[${chalk.magenta(imm)}] ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "si")     return `${chalk.yellow(rs)} ${chalk.dim("->")} [${chalk.magenta(imm)}]`;
		if (op == "set")    return `${chalk.magenta(imm)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "sli")    return `${chalk.yellow(rs)} ${Parser.colorOper("<") } ${chalk.magenta(imm)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "slei")   return `${chalk.yellow(rs)} ${Parser.colorOper("<=")} ${chalk.magenta(imm)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "seqi")   return `${chalk.yellow(rs)} ${Parser.colorOper("==")} ${chalk.magenta(imm)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "slui")   return `${chalk.yellow(rs)} ${Parser.colorOper("<") } ${chalk.magenta(imm)} ${chalk.dim("->")} ${chalk.yellow(rd)} /u`;
		if (op == "sleui")  return `${chalk.yellow(rs)} ${Parser.colorOper("<=")} ${chalk.magenta(imm)} ${chalk.dim("->")} ${chalk.yellow(rd)} /u`;
		if (op == "lbi")    return `[${chalk.magenta(imm)}] ${chalk.dim("->")} ${chalk.yellow(rd)} /b`;
		if (op == "sbi")    return `${chalk.yellow(rs)} ${chalk.dim("->")} [${chalk.magenta(imm)}] /b`;
		if (op == "lni")    return `[${chalk.magenta(imm)}] ${chalk.dim("->")} [${chalk.yellow(rd)}]`;
		if (op == "lbni")   return `[${chalk.magenta(imm)}] ${chalk.dim("->")} [${chalk.yellow(rd)}] /b`;
		return `(unknown I-type: ${Parser.colorOper(op)})`;
	}

	static formatJ(op, rs, addr) {
		if (op == "j")   return `${chalk.dim(":") } ${chalk.magenta(addr)}`;
		if (op == "jc")  return `${chalk.dim(":") } ${chalk.magenta(addr)} ${chalk.red("if")} ${chalk.yellow(rs)}`;
		if (op == "jl")  return `${chalk.dim("::")} ${chalk.magenta(addr)}`;
		if (op == "jlc") return `${chalk.dim("::")} ${chalk.magenta(addr)} ${chalk.red("if")} ${chalk.yellow(rs)}`;
		return `(unknown J-type: ${Parser.colorOper(op)})`;
	}

	static formatTrap(rt, rs, rd, funct) {
		if (funct == TRAPS.printr) return `<${chalk.cyan("print")} ${chalk.yellow(rs)}>`;
		if (funct == TRAPS.prc)    return `<${chalk.cyan("prc")} ${chalk.yellow(rs)}>`;
		if (funct == TRAPS.prd)    return `<${chalk.cyan("prd")} ${chalk.yellow(rs)}>`;
		if (funct == TRAPS.prx)    return `<${chalk.cyan("prx")} ${chalk.yellow(rs)}>`;
		if (funct == TRAPS.halt)   return `<${chalk.cyan("halt")}>`;
		return `<${chalk.bold("trap")} ${chalk.red(funct)}>`;
	}

	/**
	 * Finds the length of the symbol table of a program.
	 * @param  {Long[]} longs An array of Longs.
	 * @return {number} The length of the symbol table in words.
	 */
	static getSymbolTableLength(longs) {
		return longs[1].subtract(longs[0]).toNumber() / 8;
	}

	/**
	 * Finds the length of the handlers section of a program.
	 * @param  {Long[]} longs An array of Longs.
	 * @return {number} The length of the handlers section in words.
	 */
	static getHandlersLength(longs) {
		return longs[2].subtract(longs[1]).toNumber() / 8;
	}

	/**
	 * Finds the length of the code section of a program.
	 * @param  {Long[]} longs An array of Longs.
	 * @return {number} The length of the code section in words.
	 */
	static getCodeLength(longs) {
		return longs[3].subtract(longs[2]).toNumber() / 8;
	}

	/**
	 * Finds the length of the data section of a program.
	 * @param  {Long[]} longs An array of Longs.
	 * @return {number} The length of the data section in words.
	 */
	static getDataLength(longs) {
		return longs[4].subtract(longs[3]).toNumber() / 8;
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
