#!/usr/local/bin/node
let fs = require("fs"),
	nearley = require("nearley"),
	nearleyg = require("nearley/lib/nearley-language-bootstrapped.js"),
	nearleyc = require("nearley/lib/compile.js"),
	gen = require("nearley/lib/generate.js"),
	chalk = require("chalk"),
	getline = require("get-line-from-pos"),
	minimist = require("minimist"),
	prettyjson = require("prettyjson"),
	Long = require("long"),
	_ = require("lodash");

require("string.prototype.padstart").shim();
require("string.prototype.padend").shim();

const EXCEPTIONS = ["dbz"];

const R_TYPES = [
	0b000000000001, // Math
	0b000000000010, // Logic
	0b000000001100, // Move From HI Register
	0b000000001100, // Move From LO Register
	0b000000001110, // Comparisons
	0b000000010001, // Jump to Register
	0b000000010010, // Memory
];

const I_TYPES = [
	0b000000000011, // Addition Immediate
	0b000000000100, // Subtraction Immediate
	0b000000000101, // Multiplication Immediate
	0b000000000110, // Bitwise AND Immediate
	0b000000000111, // Bitwise NAND Immediate
	0b000000001000, // Bitwise NOR Immediate
	0b000000001001, // Bitwise OR Immediate
	0b000000001010, // Bitwise XNOR Immediate
	0b000000001011, // Bitwise XOR Immediate
	0b000000001101, // Load Upper Immediate

];

const J_TYPES = [
	0b000000001111, // Jump
	0b000000010000, // Jump Conditional
];

const OPS = {
	add:   0b000000000001,
	sub:   0b000000000001,
	mult:  0b000000000001,
	and:   0b000000000010,
	nand:  0b000000000010,
	nor:   0b000000000010,
	not:   0b000000000010,
	or:    0b000000000010,
	xnor:  0b000000000010,
	xor:   0b000000000010,
	addi:  0b000000000011,
	subi:  0b000000000100,
	multi: 0b000000000101,
	andi:  0b000000000110,
	nandi: 0b000000000111,
	nori:  0b000000001000,
	ori:   0b000000001001,
	xnori: 0b000000001010,
	xori:  0b000000001011,
	mfhi:  0b000000001100,
	mflo:  0b000000001100,
	lui:   0b000000001101,
	sl:    0b000000001110,
	sle:   0b000000001110,
	seq:   0b000000001110,
	jump:  0b000000001111,
	jc:    0b000000010000,
	jr:    0b000000010001,
	mem:   0b000000010010,
};

const FUNCTS = {
	add:  0b000000000000,
	and:  0b000000000000,
	jr:   0b000000000000,
	mfhi: 0b000000000000,
	sl:   0b000000000000,
	mflo: 0b000000000001,
	nand: 0b000000000001,
	sle:  0b000000000001,
	sub:  0b000000000001,
	mult: 0b000000000010,
	nor:  0b000000000010,
	seq:  0b000000000010,
	not:  0b000000000011,
	or:   0b000000000100,
	xnor: 0b000000000101,
	xor:  0b000000000110,
};

class Wasmc {
	static die(...a) { console.error(...a); process.exit(1) };

	// Converts an array of 8 characters into a Long.
	static chunk2long(chunk) {
		return Long.fromString(chunk.map((c) => c.charCodeAt(0).toString(16).padStart(2, "0")).join(""), true, 16);
	};

	// Adds nulls to the end of the string to lengthen it to a multiple of 8.
	static nullpad(str) {
		return str.padEnd(Math.ceil(str.length / 8) * 8, "\0");
	};

	// Given any string, str2longs nullpads and chunks it and returns an array of Longs.
	static str2longs(str) {
		return _.chunk(Wasmc.nullpad(str).split(""), 8).map(Wasmc.chunk2long);
	};

	// Given an array of longs, returns an array containing the 16-length zero-padded hex representations.
	static longs2strs(longs) {
		return longs.map((l) => l.toString(16).padStart(16, "0"));
	};
	
	constructor(opt, filename) {
		this.opt = opt;
		this.filename = filename;
	};

	get binary() { return this.opt.binary };
	get debug() { return this.opt.debug };

	parse() {
		let grammar;
		try {
			grammar = require("./wasm.js");
		} catch (e) {
			console.error("Couldn't read wasm.js.");
			if (this.opt.debug) {
				console.error(e);
			};

			process.exit(1);
		};

		const parser = new nearley.Parser(grammar.ParserRules, grammar.ParserStart);
		const source = `\n${fs.readFileSync(filename, {encoding: "utf8"})}\n`;

		let trees;
		try {
			trees = parser.feed(source).results;
		} catch (e) {
			console.error(chalk.red("Syntax error"), "at", chalk.white(`${getline(source, e.offset) - 1}:${e.offset - source.split(/\n/).slice(0, getline(source, e.offset) - 1).join("\n").length - 1}`) + ":");
			if (this.opt.dev) {
				console.log(e.message.replace(/\(@(\d+):([^)]+)\)/g, ($0, $1, $2) => { const _line = getline(source, e.offset) - 1; return `(@${_line}:${e.offset - source.split(/\n/).slice(0, _line).join("\n").length - 1 + $2})` }));
			};

			process.exit(1);
		};

		if (trees.length > 1) {
			trees.forEach((tree) => console.log(JSON.stringify(trees[tree], null, 4)));
			console.error(chalk.red.italic(`\nAmbiguous grammar (${trees.length}).\n`));
			process.exit(1);
		} else if (trees.length === 0) {
			console.warn(chalk.red.italic("Nothing parsed."));
			process.exit(1);
		};

		let parsed = trees[0];

		if (typeof parsed != "object") {
			Wasmc.die("Parser output isn't an object.");
		};

		if (typeof parsed.metadata == "undefined") {
			parsed.metadata = { };
		};

		if (typeof parsed.data == "undefined") {
			parsed.data = { };
		};

		if (typeof parsed.code == "undefined") {
			parsed.code = { };
		};

		return parsed;
	};

	compile() {
		let parsed = this.parse();

		let meta = this.metaPreliminary(parsed);
		let { data, offsets: data_offsets, offset } = this.getData(parsed, meta);
		meta[2] = Long.fromInt(offset);
		let handlers = [...Array(EXCEPTIONS.length)].map(() => Long.UZERO); // just a placeholder for now.

		let labels = [];
		parsed.code.forEach((item, i) => {
			if (item[0]) {
				labels[item[0]] = offset + i;
				this.log(`Label ${item[0]} at offset ${offset + i}`);
			};
		});

		let code = this.getCode(parsed, meta, labels);

		const out = meta.concat(handlers).concat(data).concat(code);

		console.log({
			meta: Wasmc.longs2strs(meta),
			handlers: Wasmc.longs2strs(handlers),
			data: Wasmc.longs2strs(data),
			code: Wasmc.longs2strs(code),
			out: Wasmc.longs2strs(out),
			labels,
			data_offsets
		});
	};

	metaPreliminary(parsed) {
		let meta = [], [name, version, author] = [parsed.meta.name || "?", parsed.meta.version || "?", parsed.meta.author || "?"];

		const orcid = typeof parsed.meta.orcid == "undefined"? "0000000000000000" : parsed.meta.orcid.replace(/\D/g, "");
		if (orcid.length != 16) {
			Wasmc.die("Error: invalid ORCID.");
		};

		// Convert the ORCID into two Longs and stash them in the correct positions in meta.
		[meta[4], meta[5]] = [orcid.substr(0, 8), orcid.substr(8)].map((half) => Wasmc.chunk2long(half.split("")));

		// Append the name-version-author string.
		meta = meta.concat(Wasmc.str2longs(`${name}\0${version}\0${author}\0`));
		
		// The beginning of the handler pointer section comes right after the end of the meta section.
		meta[0] = Long.fromInt(meta.length, true);

		// The handlers section is exactly as large as the set of exceptions; the data section begins
		// at the sum of the lengths of the meta section and the handlers section.
		meta[1] = Long.fromInt(meta.length + EXCEPTIONS.length);

		this.log({parsed, meta, version, author});

		return meta;
	};

	getData(parsed, meta) {
		let data = [], offsets = {};
		let offset = meta[1].toInt();
		_(parsed.data).forEach(([type, value], key) => {
			let pieces;
			if (type.match(/^(in|floa)t$/)) {
				pieces = [Long.fromValue(value)];
			} else if (type == "string") {
				pieces = Wasmc.str2longs(value);
			} else {
				Wasmc.die(`Error: unknown data type "${type}" for "${key}".`);
			};

			offsets[key] = offset;
			offset += pieces.length;
			data = data.concat(pieces);
		});

		return { data, offsets, offset };
	};

	getCode(parsed, meta, labels) {
		let out = [];
		parsed.code.forEach((item, i) => {
			let [label, op, ...args] = item;
			if (op == "add") {

			};
		});

		return out;
	};

	rType(opcode, rt, rs, rd, shift, func) {
		if (!R_TYPES.includes(opcode)) throw `opcode ${opcode} isn't a valid r-type`;
		if (rt < 0 || 127 < rt) throw `rt (${rt}) not within the valid range (0–127)`;
		if (rs < 0 || 127 < rs) throw `rs (${rs}) not within the valid range (0–127)`;
		if (rd < 0 || 127 < rd) throw `rd (${rd}) not within the valid range (0–127)`;
		if (shift < 0 || 65535 < shift) throw `shift (${shift}) not within the valid range (0–65535)`;
		if (func < 0 || 4095 < func) throw `func (${func}) not within the valid range (0–4095)`;

		let lower = func | (shift << 12) | ((rd & 1) << 31);
		let upper = (rd >> 1) | (rs << 6) | (rt << 13) | (opcode << 20);
		let long = Long.fromBits(lower, upper, true);

		this.log(`Lower: ${lower.toString(2).padStart(32, "_")} (${lower.toString(2).length})`);
		this.log(`Upper: ${upper.toString(2).padStart(32, "_")} (${upper.toString(2).length})`);
		this.log(`Long: ${long.toString(16)}, ${long.toString(2)} <--`, long);

		return long;
	};

	iType(opcode, rs, rd, imm) {
		if (!I_TYPES.includes(opcode)) throw `opcode ${opcode} isn't a valid r-type`;
		if (rs < 0 || 127 < rs) throw `rs (${rs}) not within the valid range (0–127)`;
		if (rd < 0 || 127 < rd) throw `rd (${rd}) not within the valid range (0–127)`;
		if (imm < 0 || 4294967295 < imm) throw `imm (${imm}) not within the valid range (-2147483648–2147483647)`;

		let lower = imm;
		let upper = rd | (rs << 7) | (opcode << 20);
		let long = Long.fromBits(lower, upper, true);

		this.log(`Lower: ${lower.toString(2).padStart(32, "_")} (${lower.toString(2).length})`);
		this.log(`Upper: ${upper.toString(2).padStart(32, "_")} (${upper.toString(2).length})`);
		this.log(`Long: ${long.toString(16)}, ${long.toString(2)} <--`, long);

		return long;
	};

	jType(opcode, rs, addr) {
		if (!J_TYPES.includes(opcode)) throw `opcode ${opcode} isn't a valid r-type`;
		if (rs < 0 || 127 < rs) throw `rs (${rs}) not within the valid range (0–127)`;
		if (addr < 0 || 4294967295 < addr) throw `addr (${addr}) not within the valid range (0–4294967295)`;

		let lower = addr;
		let upper = (rs << 13) | (opcode << 20);
		let long = Long.fromBits(lower, upper, true);

		this.log(`Lower: ${lower.toString(2).padStart(32, "_")} (${lower.toString(2).length})`);
		this.log(`Upper: ${upper.toString(2).padStart(32, "_")} (${upper.toString(2).length})`);
		this.log(`Long: ${long.toString(16)}, ${long.toString(2)} <--`, long);

		return long;
	};

	log(...args) {
		if (this.debug) {
			console.log(...args);
		};

		return !!this.debug;
	};
};

const opt = minimist(process.argv.slice(2), {
	alias: { b: "binary", d: "debug" },
	boolean: ["binary", "debug"],
	default: { binary: false, debug: false }
}), filename = opt._[0];

if (!filename) {
	return console.log("Usage: node wasmc.js [filename]");
};

new Wasmc(opt, filename).compile();