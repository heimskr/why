#!/usr/bin/env node
let WASMC = require("../wasm/wasmc.js"),
	fs = require("fs"),
	Long = require("long"),
	_ = require("lodash"),
	minimist = require("minimist");

const chalk = new (require("chalk")).constructor({ enabled: true });

require("../util.js")(_);
require("string.prototype.padstart").shim();
require("string.prototype.padend").shim();

const { EXCEPTIONS, R_TYPES, I_TYPES, J_TYPES, OPCODES, FUNCTS, REGISTER_OFFSETS, TRAPS } = require("../wasm/constants.js");
const SEDOCPO = _.multiInvert(OPCODES);
const STESFFO = _.multiInvert(REGISTER_OFFSETS);
const OFFSET_VALUES = _.uniq(Object.values(REGISTER_OFFSETS)).sort((a, b) => b - a);

const Parser = module.exports = {
	open(filename, silent=true) {
		return Parser.read(fs.readFileSync(filename, "utf8"), silent);
	},

	read(text, silent=true) {
		if (text.match(/{/)) {
			const data = JSON.parse(text);
			const raw = data.program.split(/\s+/).map((s) => Long.fromString(s, true, 16));
			return {
				raw,
				data,
				parsed: Parser.parse(raw, silent)
			};
		} else {
			return {
				parsed: Parser.parse(text.split("\n").map((s) => Long.fromString(s, true, 16)), silent),
				raw: text.split("\n").map((s) => Long.fromString(s, true, 16))
			};
		};
	},

	parse(longs, silent=true) {
		let offsets = {
			$handlers: longs[0].toInt(),
			$data: longs[1].toInt(),
			$code: longs[2].toInt(),
			$end: longs[3].toInt()
		};

		let handlers = longs.slice(offsets.$handlers, offsets.$data).map((x, i) => [EXCEPTIONS[i], x.toInt()]);

		let meta = {
			orcid: _.chain([4, 5]).map((n) => longs[n]).longString().join("").chunk(4).map((n) => n.join("")).join("-").value()
		};

		[meta.name, meta.version, meta.author] = _(longs).slice(6, offsets.$handlers).longStrings();

		let code = longs.slice(offsets.$code / 8, offsets.$end / 8);

		if (!silent) {
			console.log(chalk.dim("/*"));
			console.log(chalk.green.dim("#offsets"));
			console.log(Object.keys(offsets).map((k) => `${chalk.dim(`${k}:`)} ${chalk.magenta.dim(offsets[k])}`).join("\n"));
			console.log(chalk.dim("*/") + "\n");

			console.log(chalk.green("#meta"));
			console.log(Object.keys(meta).map((k) => `${chalk.cyan(k)}: ${chalk.yellow(`"${meta[k]}"`)}`).concat([""]).join("\n"));

			console.log(chalk.green("#handlers"));
			console.log(handlers.map(([k, v]) => `${k}: ${chalk.magenta(v)}`).join("\n"));

			console.log([, chalk.green("#code"), ...code.map(Parser.formatInstruction)].join("\n"));
		};

		return { offsets, handlers, meta, code: code.map(Parser.parseInstruction) };
	},

	parseInstruction(instruction) {
		if (instruction instanceof Long) {
			instruction = instruction.toString(2).padStart(64, "0");
		};

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
				imm: get(32),
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
			return { op: "nop", opcode };
		};
	},

	formatInstruction(instruction) {
		if (instruction instanceof Long) {
			instruction = instruction.toString(2).padStart(64, "0");
		};

		const get = (...args) => parseInt(instruction.substr(...args), 2);
		const opcode = get(0, 12);
		const type = Parser.instructionType(opcode);

		if (type == "r") {
			const funct = get(52);
			return Parser.formatR(SEDOCPO[opcode].filter((op) => Parser.instructionType(opcode) == "r" && (opcode == OPCODES.trap || FUNCTS[op] == funct))[0], Parser.getRegister(get(12, 7)), Parser.getRegister(get(19, 7)), Parser.getRegister(get(26, 7)), funct);
		} else if (type == "i") {
			return Parser.formatI(SEDOCPO[opcode][0], Parser.getRegister(get(18, 7)), Parser.getRegister(get(25, 7)), get(32));
		} else if (type == "j") {
			return Parser.formatJ(SEDOCPO[opcode][0], Parser.getRegister(get(12, 7)), get(32));
		} else if (opcode == 0) {
			return "<>";
		};

		throw `Can't parse instruction ${instruction} (${opcode}).`;
	},

	instructionType(opcode) {
		if (R_TYPES.includes(opcode)) return "r";
		if (I_TYPES.includes(opcode)) return "i";
		if (J_TYPES.includes(opcode)) return "j";
	},

	getRegister(n) {
		if (n == REGISTER_OFFSETS.return) {
			return "$rt";
		};

		for (let i = 0; i < OFFSET_VALUES.length; i++) {
			if (OFFSET_VALUES[i] <= n) {
				const s = STESFFO[OFFSET_VALUES[i]][0];
				return "$" + (s.match(/^[ratskemf]$/)? s + (n - OFFSET_VALUES[i]).toString(16) : s);
			};
		};

		return null;
	},

	formatR(op, rt, rs, rd, funct) {
		const alt_op = (oper) => {
			if (rs == rd) return `${chalk.yellow(rs)} ${chalk.red(oper + "=")} ${chalk.yellow(rt)}`;
			if (rt == rd) return `${chalk.yellow(rt)} ${chalk.red(oper + "=")} ${chalk.yellow(rs)}`;
			return `${chalk.yellow(rs)} ${chalk.red(oper)} ${chalk.yellow(rt)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		};

		if (op == "add")   return alt_op("+");
		if (op == "sub")   return alt_op("-");
		if (op == "mult")  return `${chalk.yellow(rs)} ${chalk.red("*")} ${chalk.yellow(rt)}`;
		if (op == "and")   return alt_op("&");
		if (op == "nand")  return alt_op("~&");
		if (op == "nor")   return alt_op("~|");
		if (op == "not")   return `${chalk.red("~") + chalk.yellow(rs)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "or")    return rs == "$0"? `${chalk.yellow(rt)} ${chalk.dim("->")} ${chalk.yellow(rd)}` : alt_op("|");
		if (op == "xnor")  return alt_op("~x");
		if (op == "xor")   return alt_op("x");
		if (op == "mfhi")  return `${chalk.green("%hi")} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "mflo")  return `${chalk.green("%lo")} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "sl")    return `${chalk.yellow(rs)} ${chalk.red("<")} ${chalk.yellow(rt)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "sle")   return `${chalk.yellow(rs)} ${chalk.red("<=")} ${chalk.yellow(rt)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "seq")   return `${chalk.yellow(rs)} ${chalk.red("==")} ${chalk.yellow(rt)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "jr")    return `${chalk.dim(":")} ${chalk.yellow(rd)}`;
		if (op == "jrc")   return `${chalk.dim(":")} ${chalk.yellow(rd)} (${chalk.yellow(rs)})`;
		if (op == "jrl")   return `${chalk.dim("::")} ${chalk.yellow(rd)}`;
		if (op == "jrlc")  return `${chalk.dim("::")} ${chalk.yellow(rd)} (${chalk.yellow(rs)})`;
		if (op == "c")     return `[${chalk.yellow(rs)}] ${chalk.dim("->")} [${chalk.yellow(rd)}]`;
		if (op == "l")     return `[${chalk.yellow(rs)}] ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "s")     return `${chalk.yellow(rs)} ${chalk.dim("->")} [${chalk.yellow(rd)}]`;
		if (op == "addu")  return alt_op("?+");
		if (op == "subu")  return alt_op("?-");
		if (op == "multu") return `${chalk.yellow(rs)} ${chalk.red("?*")} ${chalk.yellow(rt)}`;
		if (op == "slu")   return `${chalk.yellow(rs)} ${chalk.red("?<")} ${chalk.yellow(rt)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "sleu")  return `${chalk.yellow(rs)} ${chalk.red("?<=")} ${chalk.yellow(rt)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "sequ")  return `${chalk.yellow(rs)} ${chalk.red("?==")} ${chalk.yellow(rt)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "trap") return this.formatTrap(rt, rs, rd, funct);
		return `(unknown r-type: ${chalk.red(op)})`;
	},

	formatI(op, rs, rd, imm) {
		const mathi = (increment, opequals, op) => {
			if (rs == rd) {
				return imm == 1? chalk.yellow(rd) + chalk.yellow.dim(increment) : `${chalk.yellow(rs)} ${chalk.red(opequals)} ${chalk.magenta(imm)}`;
			};

			return `${chalk.yellow(rs)} ${chalk.red(op)} ${chalk.magenta(imm)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		};

		const alt_op = (oper) => `${chalk.yellow(rs)} ${chalk.red(oper + (rs == rd? "=" : ""))} ${chalk.magenta(imm) + (rs != rd? chalk.dim(" -> ") + chalk.yellow(rd) : "")}`;
		if (op == "addi")   return mathi("++", "+=", "+");
		if (op == "subi")   return mathi("--", "-=", "-");
		if (op == "addiu")  return mathi("?++", "?+=", "?+");
		if (op == "subiu")  return mathi("?--", "?-=", "?-");
		if (op == "multi")  return `${chalk.yellow(rs)} ${chalk.red("*")} ${chalk.magenta(imm)}`;
		if (op == "multiu") return `${chalk.yellow(rs)} ${chalk.red("?*")} ${chalk.magenta(imm)}`;
		if (op == "andi")   return alt_op("&")
		if (op == "nandi")  return alt_op("~&")
		if (op == "nori")   return alt_op("~|")
		if (op == "ori")    return alt_op("|")
		if (op == "xnori")  return alt_op("~x")
		if (op == "xori")   return alt_op("x")
		if (op == "lui")    return `${chalk.dim("lui:")} ${chalk.magenta(imm)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "li")     return `[${chalk.magenta(imm)}] ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "si")     return `${chalk.yellow(rs)} ${chalk.dim("->")} [${chalk.magenta(imm)}]`;
		if (op == "set")    return `${chalk.magenta(imm)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "sli")    return `${chalk.yellow(rs)} ${chalk.red("<")} ${chalk.magenta(imm)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "slei")   return `${chalk.yellow(rs)} ${chalk.red("<=")} ${chalk.magenta(imm)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "seqi")   return `${chalk.yellow(rs)} ${chalk.red("==")} ${chalk.magenta(imm)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "sliu")   return `${chalk.yellow(rs)} ${chalk.red("?<")} ${chalk.magenta(imm)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "sleiu")  return `${chalk.yellow(rs)} ${chalk.red("?<=")} ${chalk.magenta(imm)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		if (op == "seqiu")  return `${chalk.yellow(rs)} ${chalk.red("?==")} ${chalk.magenta(imm)} ${chalk.dim("->")} ${chalk.yellow(rd)}`;
		return `(unknown i- type: ${chalk.red(op)})`;
	},

	formatJ(op, rs, addr) {
		if (op == "j")   return `${chalk.dim(":")} ${chalk.magenta(addr)}`;
		if (op == "jc")  return `${chalk.dim(":")} ${chalk.magenta(addr)} (${chalk.yellow(rs)})`;
		if (op == "jl")  return `${chalk.dim("::")} ${chalk.magenta(addr)}`;
		if (op == "jlc") return `${chalk.dim("::")} ${chalk.magenta(addr)} (${chalk.yellow(rs)})`;
		return `(unknown j-type: ${chalk.red(op)})`;
	},

	formatTrap(rt, rs, rd, funct) {
		if (funct == TRAPS.printr) return `<${chalk.cyan("print")} ${chalk.yellow(rs)}>`;
		if (funct == TRAPS.halt)   return `<${chalk.cyan("halt")}>`;
		return `<${chalk.bold("trap")} ${chalk.red(funct)}>`;
	}
};

if (require.main === module) {
	const opt = minimist(process.argv.slice(2), { }), filename = opt._[0];

	if (!filename) {
		console.log("Usage: node parser.js [filename]");
		process.exit(0);
	};

	Parser.open(filename, false);
};
