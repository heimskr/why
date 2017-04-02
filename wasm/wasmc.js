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

const { EXCEPTIONS, R_TYPES, I_TYPES, J_TYPES, OPCODES, FUNCTS, REGISTER_OFFSETS } = require("./constants.js");

class WASMC {
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
		return _.chunk(WASMC.nullpad(str).split(""), 8).map(WASMC.chunk2long);
	};

	// Given an array of longs, returns an array containing the 16-length zero-padded hex representations.
	static longs2strs(longs) {
		return longs.map((l) => l instanceof Long? l.toString(16).padStart(16, "0") : "x".repeat(16));
	};

	// If the input is an array (expected format: ["register", ...]), then the output is the number corresponding to that array.
	// Otherwise, if the input is something other than an array, then the output is same as the input.
	static convertRegister(x) {
		return x instanceof Array? (x.length == 0? 0 : REGISTER_OFFSETS[x[1]] + x[2]) : x;
	};
	
	constructor(opt, filename) {
		this.opt = opt;
		this.filename = filename;
		this.offsets = { };
		this.parsed = { };
		this.meta = [];
		this.data = [];
		this.offsets = { };
		this.code = [];
		this.expanded = [];
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
		const source = fs.readFileSync(this.filename, {encoding: "utf8"}) + "\n";

		let trees;
		try {
			trees = parser.feed(source).results;
		} catch (e) {
			console.error(chalk.red("Syntax error"), "at", chalk.white(`${getline(source, e.offset)}:${e.offset - source.split(/\n/).slice(0, getline(source, e.offset) - 1).join("\n").length}`) + ":");
			if (this.opt.dev) {
				console.log(e.message.replace(/\(@(\d+):([^)]+)\)/g, ($0, $1, $2) => { const _line = getline(source, e.offset); return `(@${_line}:${e.offset - source.split(/\n/).slice(0, _line).join("\n").length + $2})` }));
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

		this.parsed = trees[0];

		if (typeof this.parsed != "object") {
			WASMC.die("Parser output isn't an object.");
		};

		if (typeof this.parsed.metadata == "undefined") {
			this.parsed.metadata = { };
		};

		if (typeof this.parsed.data == "undefined") {
			this.parsed.data = { };
		};

		if (typeof this.parsed.code == "undefined") {
			this.parsed.code = { };
		};
	};

	compile() {
		this.parse();
		this.processMetadata();
		this.processData();

		this.handlers = [...Array(EXCEPTIONS.length)].map(() => Long.UZERO); // just a placeholder for now.

		this.expandCode();
		this.processCode();

		this.meta[3] = Long.fromInt([this.meta, this.handlers, this.data, this.code].reduce((a, b) => a + b.length, 0), true);
		const out = this.meta.concat(this.handlers).concat(this.data).concat(this.code);

		if (this.opt.debug) {
			console.log({
				meta: WASMC.longs2strs(this.meta),
				handlers: WASMC.longs2strs(this.handlers),
				data: WASMC.longs2strs(this.data),
				code: WASMC.longs2strs(this.code),
				out: WASMC.longs2strs(out),
				offsets: this.offsets
			});
		} else {
			let outname = typeof this.opt.out != "string"? this.filename.replace(/\.wasm$/i, "") + ".why" : this.opt.out;
			fs.writeFileSync(outname, WASMC.longs2strs(out).join("\n"));
			console.log(chalk.green("\u2714"), "Successfully assembled", chalk.bold(this.filename), "and saved the bytecode to", chalk.bold(outname) + ".");
		};
	};

	processMetadata(parsed) {
		let [name, version, author] = [this.parsed.meta.name || "?", this.parsed.meta.version || "?", this.parsed.meta.author || "?"];

		const orcid = typeof this.parsed.meta.orcid == "undefined"? "0000000000000000" : this.parsed.meta.orcid.replace(/\D/g, "");
		if (orcid.length != 16) {
			WASMC.die("Error: invalid ORCID.");
		};

		// Convert the ORCID into two Longs and stash them in the correct positions in meta.
		[this.meta[4], this.meta[5]] = [orcid.substr(0, 8), orcid.substr(8)].map((half) => WASMC.chunk2long(half.split("")));

		// Append the name-version-author string.
		this.meta = this.meta.concat(WASMC.str2longs(`${name}\0${version}\0${author}\0`));
		
		// The beginning of the handler pointer section comes right after the end of the meta section.
		this.meta[0] = Long.fromInt(this.meta.length, true);

		// The handlers section is exactly as large as the set of exceptions; the data section begins
		// at the sum of the lengths of the meta section and the handlers section.
		this.meta[1] = Long.fromInt(this.meta.length + EXCEPTIONS.length);

		this.log({ meta: this.meta, version, author });
	};

	processData() {
		let offset = this.meta[1].toInt();
		_(this.parsed.data).forEach(([type, value], key) => {
			let pieces;
			if (type.match(/^(in|floa)t$/)) {
				pieces = [Long.fromValue(value)];
			} else if (type == "string") {
				pieces = WASMC.str2longs(value);
			} else {
				WASMC.die(`Error: unknown data type "${type}" for "${key}".`);
			};

			this.offsets[key] = offset;
			this.data = this.data.concat(pieces);
			offset += pieces.length;
		});

		this.meta[2] = Long.fromInt(offset);
	};

	processCode() {
		this.expanded.forEach((item, i) => {
			this.addCode(item);
		});
	};

	expandCode() {
		const isLabelRef = (x) => x instanceof Array && x.length == 2 && x[0] == "label";
		// In the first pass, we expand pseudoinstructions into their constituent parts. Some instructions will need to be
		// gone over once again after labels have been sorted out so we can replace variable references with addresses.
		this.parsed.code.forEach((item) => {
			let [label, op, ...args] = item;
			if (label) {
				this.offsets[label] = this.meta[2].toInt() + this.expanded.length;
			};

			if (op == "mv") {
				this.expanded.push([label, "or", args[0], _0, args[1]]);
			} else if (op == "ret") {
				this.expanded.push([label, "jr", _0, _0, _RA]);
			} else if (op == "push") {
				let _label = label, getLabel = () => [_label, _label = null][0];
				args.forEach((reg, i) => {
					this.expanded.push([getLabel(), "s", _0, reg, _SP]);
					this.expanded.push([label, "addi", _SP, _SP, 1]);
				});
			} else if (op == "pop") {
				let _label = label, getLabel = () => [_label, _label = null][0];
				args.forEach((reg, i) => {
					this.expanded.push([getLabel(), "l", _0, _SP, reg]);
					this.expanded.push([label, "subi", _SP, _SP, 1]);
				});
			} else if (op == "sge") {
				this.expanded.push([label, "sle", args[1], args[0], args[2]]);
			} else if (op == "sg") {
				this.expanded.push([label, "sl", args[1], args[0], args[2]]);
			} else if (op == "jeq") {
				// First, set $m0 to rs == rt.
				this.expanded.push([label, "seq", ...args.slice(0, 2), _M[0]]);

				if (args[2][0] == "value") {
					// Set $m1 to the immediate value and then conditionally jump to $m1.
					this.expanded.push([null, "set", _0, _M[1], args[2][1]]);
					this.expanded.push([null, "jrc", _0, _M[0], _M[1]]);
				} else if (args[2][0] == "register") {
					// We're already given a register, so we don't have to do anything with $m1.
					this.expanded.push([null, "jrc", _0, _M[0], args[2]]);
				} else if (args[2][0] == "label") {
					// Load the value of the given variable into $m1 and then conditionally jump to $m1.
					this.expanded.push([null, "li", _0, _M[1], args[2]]);
					this.expanded.push([null, "jrc", _0, _M[0], _M[1]]);
				};
			} else if (R_TYPES.includes(OPCODES[op]) && _.some(args, isLabelRef)) {
				let [rt, rs, rd] = args;
				let [lt, ls, ld] = [rt, rs, rd].map(isLabelRef);
				let _label = label, getLabel = () => [_label, _label = null][0];
				[rt, rs].forEach((reg, i) => {
					if (isLabelRef(reg)) {
						// Whoops, this register isn't actually a register
						this.expanded.push([getLabel(), "li", _0, _M[i], reg]);
					};
				});

				this.expanded.push([getLabel(), op, ...[rt, rs, rd].map((reg, i) => [lt, ls, ld][i]? _M[i] : reg)]);

				if (ld) {
					this.expanded.push([getLabel(), "si", _M[2], _0, rd]);
				};
			} else if (I_TYPES.includes(OPCODES[op]) && _.some(args, isLabelRef)) {
				let [rs, rd, imm] = args;
				let [ls, ld] = [rs, rd].map(isLabelRef);
				let _label = label, getLabel = () => [_label, _label = null][0];
				if (ls) {
					this.expanded.push([getLabel(), "li", _0, _M[0], rs]);
				};

				this.expanded.push([getLabel(), op, ...[rs, rd].map((reg, i) => [ls, ld][i]? _M[i] : reg), imm]);

				if (ld) {
					this.expanded.push([getLabel(), "si", _M[1], _0, rd]);
				};
			} else {
				this.expanded.push(item);
			};
		});

		// In the second pass, we replace label references with the corresponding
		// addresses now that we know the address of all the labels.
		this.expanded.forEach((item) => {
			// First off, now that we've recorded all the label positions,
			// we can remove the label tags.
			item.shift();

			// Look at everything past the operation name (the arguments).
			item.slice(1).forEach((arg, i) => {
				// If the argument is a label reference,
				if (isLabelRef(arg)) {
					// replace it with an address from the offsets map. 
					item[i + 1] = this.offsets[arg[1]];
				};
			});
		});

		// console.log(chalk.bold.yellow("<expanded>\n"), this.expanded, chalk.bold.yellow("\n</expanded>"));
	};

	addCode([op, ...args]) {
		if (R_TYPES.includes(OPCODES[op])) {
			this.code.push(this.rType(OPCODES[op], ...args.map(WASMC.convertRegister), 0, FUNCTS[op]));
		} else if (I_TYPES.includes(OPCODES[op])) {
			this.code.push(this.iType(OPCODES[op], ...args.map(this.convertValue, this)));
		} else if (J_TYPES.includes(OPCODES[op])) {
			this.code.push(this.jType(OPCODES[op], ...args.map(this.convertValue, this)));
		} else if (op == "nop") {
			this.code.push(Long.UZERO);
		} else {
			console.log(`Unhandled instruction ${chalk.bold.red(op)}.`, [op, ...args]);
			this.code.push(Long.fromInt(0xdead, true));
		};
	};

	// If x is an array accepted by convertRegister, the output is the index of that array.
	// If x is a string, then the label map is checked and the address for the label x is returned.
	// If x is a number, return it.
	convertValue(x) {
		if (x instanceof Array || typeof x == "number") {
			return WASMC.convertRegister(x);
		};

		if (typeof x == "string") {
			if (typeof this.offsets[x] == "undefined") {
				throw `Undefined label: ${x}`;
			};

			return this.offsets[x];
		};

		if (typeof x == "number") {
			return x;
		};

		throw `Unrecognized value: ${x}`;
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

		// this.log(`Lower: ${lower.toString(2).padStart(32, "_")} (${lower.toString(2).length})`);
		// this.log(`Upper: ${upper.toString(2).padStart(32, "_")} (${upper.toString(2).length})`);
		// this.log(`Long: ${long.toString(16)}, ${long.toString(2)} <--`, long);

		return long;
	};

	iType(opcode, rs, rd, imm) {
		if (!I_TYPES.includes(opcode)) throw `opcode ${opcode} isn't a valid i-type`;
		if (rs < 0 || 127 < rs) throw `rs (${rs}) not within the valid range (0–127)`;
		if (rd < 0 || 127 < rd) throw `rd (${rd}) not within the valid range (0–127)`;
		if (imm < 0 || 4294967295 < imm) throw `imm (${imm}) not within the valid range (-2147483648–2147483647)`;

		let lower = imm;
		let upper = rd | (rs << 7) | (opcode << 20);
		let long = Long.fromBits(lower, upper, true);

		// this.log(`Lower: ${lower.toString(2).padStart(32, "_")} (${lower.toString(2).length})`);
		// this.log(`Upper: ${upper.toString(2).padStart(32, "_")} (${upper.toString(2).length})`);
		// this.log(`Long: ${long.toString(16)}, ${long.toString(2)} <--`, long);

		return long;
	};

	jType(opcode, rs, addr) {
		if (!J_TYPES.includes(opcode)) throw `opcode ${opcode} isn't a valid j-type`;
		if (rs < 0 || 127 < rs) throw `rs (${rs}) not within the valid range (0–127)`;
		if (addr < 0 || 4294967295 < addr) throw `addr (${addr}) not within the valid range (0–4294967295)`;

		let lower = addr;
		let upper = (rs << 13) | (opcode << 20);
		let long = Long.fromBits(lower, upper, true);

		// this.log(`Lower: ${lower.toString(2).padStart(32, "_")} (${lower.toString(2).length})`);
		// this.log(`Upper: ${upper.toString(2).padStart(32, "_")} (${upper.toString(2).length})`);
		// this.log(`Long: ${long.toString(16)}, ${long.toString(2)} <--`, long);

		return long;
	};

	log(...args) {
		if (this.debug) {
			console.log(...args);
		};

		return !!this.debug;
	};
};

module.exports = WASMC;
const _R = _.range(0, 16).map((n) => ["register", "r", n]); // w
const _A = _.range(0, 16).map((n) => ["register", "a", n]); // i
const _T = _.range(0, 24).map((n) => ["register", "t", n]); // n
const _S = _.range(0, 24).map((n) => ["register", "s", n]); // k
const _K = _.range(0, 17).map((n) => ["register", "k", n]); // 
const _RA = ["register", "return", 0];                      // ;
const _M = _.range(0, 16).map((n) => ["register", "m", n]); // )
const _E = _.range(0,  6).map((n) => ["register", "e", n]);
const _SP = ["register", "stack", 0];
const _0 = ["register", "zero",  0];

if (require.main === module) {
	const opt = minimist(process.argv.slice(2), {
		alias: { b: "binary", d: "debug", o: "out" },
		boolean: ["binary", "debug"],
		default: { binary: false, debug: false }
	}), filename = opt._[0];

	if (!filename) {
		return console.log("Usage: node wasmc.js [filename]");
	};

	new WASMC(opt, filename).compile();
};
