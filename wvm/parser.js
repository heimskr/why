#!/usr/local/bin/node
// Parses wasmc output.

let Wasmc = require("../wasm/wasmc.js"),
	fs = require("fs"),
	Long = require("long"),
	_ = require("lodash"),
	minimist = require("minimist");

require("../util.js")(_);
require("string.prototype.padstart").shim();
require("string.prototype.padend").shim();

const { EXCEPTIONS, R_TYPES, I_TYPES, J_TYPES, OPCODES, FUNCTS, REGISTER_OFFSETS } = require("../wasm/constants.js");
const SEDOCPO = _.multiInvert(OPCODES);
const STESFFO = _.multiInvert(REGISTER_OFFSETS);
const OFFSET_VALUES = Object.values(REGISTER_OFFSETS).sort((a, b) => b - a);

const Parser = exports.Parser = {
	open(filename) {
		const text = fs.readFileSync(filename, { encoding: "utf8" });
		return Parser.parse(text.split("\n").map((s) => Long.fromString(s, true, 16)));
	},

	parse(longs) {
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

		let code = longs.slice(offsets.$code, offsets.$end).map(Parser.parseInstruction);

		console.log({
			offsets,
			handlers,
			meta,
			code
		});

		console.log([, ...code].join("\n"));
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
			// const shift = get(36, 16);
			// const op = SEDOCPO[opcode].filter((op) => Parser.instructionType(opcode) == "r" && FUNCTS[op] == funct)[0];
			// const rt = get(12, 7);
			// const rs = get(19, 7);
			// const rd = get(26, 7);

			// return {op, rt, rs, rd, shift, funct};

			return Parser.formatR(SEDOCPO[opcode].filter((op) => Parser.instructionType(opcode) == "r" && FUNCTS[op] == funct)[0], Parser.getRegister(get(12, 7)), Parser.getRegister(get(19, 7)), Parser.getRegister(get(26, 7)), get(36, 16), funct);
			return {
				op: SEDOCPO[opcode].filter((op) => Parser.instructionType(opcode) == "r" && FUNCTS[op] == funct)[0],
				rt: Parser.getRegister(get(12, 7)),
				rs: Parser.getRegister(get(19, 7)),
				rd: Parser.getRegister(get(26, 7)),
				shift: get(36, 16),
				funct
			};
		} else if (type == "i") {
			return Parser.formatI(SEDOCPO[opcode][0], Parser.getRegister(get(18, 7)), Parser.getRegister(get(25, 7)), get(32));
			return {
				op: SEDOCPO[opcode][0],
				rs: Parser.getRegister(get(18, 7)),
				rd: Parser.getRegister(get(25, 7)),
				imm: get(32)
			};
		} else if (type == "j") {
			return Parser.formatJ(SEDOCPO[opcode][0], Parser.getRegister(get(12, 7)), get(32));
			return {
				op: SEDOCPO[opcode][0],
			};
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
		for (let i = 0; i < OFFSET_VALUES.length; i++) {
			if (OFFSET_VALUES[i] <= n) {
				const s = STESFFO[OFFSET_VALUES[i]][0];
				return "$" + (s.match(/^[a-z]$/)? s + (n - OFFSET_VALUES[i]) : s);
			};
		};

		return null;
	},

	formatR(op, rt, rs, rd, shift, funct) {
		if (op == "add")  return `${rs} + ${rt} -> ${rd}`;
		if (op == "sub")  return `${rs} - ${rt} -> ${rd}`;
		if (op == "mult") return `${rs} * ${rt}`;
		if (op == "and")  return `${rs} & ${rt} -> ${rd}`;
		if (op == "nand") return `${rs} ~& ${rt} -> ${rd}`;
		if (op == "nor")  return `${rs} ~| ${rt} -> ${rd}`;
		if (op == "not")  return `~${rs} -> ${rd}`;
		if (op == "or")   return `${rs} | ${rt} -> ${rd}`;
		if (op == "xnor") return `${rs} ~x ${rt} -> ${rd}`;
		if (op == "xor")  return `${rs} x ${rt} -> ${rd}`;
		if (op == "mfhi") return `%hi -> ${rd}`;
		if (op == "mflo") return `%lo -> ${rd}`;
		if (op == "sl")   return `${rs} < ${rt} -> ${rd}`;
		if (op == "sle")  return `${rs} <= ${rt} -> ${rd}`;
		if (op == "seq")  return `${rs} == ${rt} -> ${rd}`;
		if (op == "jr")   return `: ${rd}`;
		if (op == "jrc")  return `: ${rd} (${rs})`;
		if (op == "c")    return `[${rs}] -> [${rd}]`;
		if (op == "l")    return `[${rs}] -> ${rd}`;
		if (op == "s")    return `${rs} -> [${rd}]`;
	},

	formatI(op, rs, rd, imm) {
		if (op == "addi")  return `${rs} + ${imm} -> ${rd}`;
		if (op == "subi")  return `${rs} - ${imm} -> ${rd}`;
		if (op == "multi") return `${rs} * ${imm}`;
		if (op == "andi")  return `${rs} & ${imm} -> ${rd}`;
		if (op == "nandi") return `${rs} ~& ${imm} -> ${rd}`;
		if (op == "nori")  return `${rs} ~| ${imm} -> ${rd}`;
		if (op == "ori")   return `${rs} | ${imm} -> ${rd}`;
		if (op == "xnori") return `${rs} ~x ${imm} -> ${rd}`;
		if (op == "xori")  return `${rs} x ${imm} -> ${rd}`;
		if (op == "lui")   return `lui: ${imm} -> ${rd}`;
		if (op == "li")    return `[${imm}] -> ${rd}`;
		if (op == "si")    return `${rs} -> [${imm}]`;
		if (op == "set")   return `${imm} -> ${rd}`;
	},

	formatJ(op, rs, addr) {
		if (op == "j")  return `: ${addr}`;
		if (op == "jc") return `: ${addr} (${rs})`;
	}
};

if (require.main === module) {
	const opt = minimist(process.argv.slice(2), { }), filename = opt._[0];

	if (!filename) {
		return console.log("Usage: node parser.js [filename]");
	};

	Parser.open(filename);
};
