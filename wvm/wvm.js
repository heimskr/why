#!/usr/local/bin/node
let WASMC = require("../wasm/wasmc.js"),
	fs = require("fs"),
	Long = require("long"),
	_ = require("lodash"),
	minimist = require("minimist"),
	chalk = require("chalk"),
	Parser = require("./parser.js");

class WVM {
	constructor({ memorySize=640000, program }={}) {
		if (typeof program != "object") {
			throw `Unable to load program.`;
		};

		this.memory = new Uint32Array(memorySize * 2);
		this.memorySize = memorySize;
		this.resetRegisters();
		this.programCounter = program.offsets.$code;
	};

	resetRegisters() {
		this.registers = _.range(0, 128).map(() => new Long());
	};

	get(k, signed=false) {
		return new Long(this.memory[2*k], this.memory[2*k + 1], signed);
	};

	set(k, v) {
		if (v instanceof Long) {
			[this.memory[2*k], this.memory[2*k + 1]] = [v.low, v.high];
		} else {
			this.memory[2*k] = v & 0xffffffff
			this.memory[2*k + 1] = v & 0xffffffff00000000;
		};

		return true;
	};

	op_j(rs, addr) {
		this.programCounter = imm;
	};

	op_jc(rs, addr) {
		if (!this.registers[rd].equals(0)) {
			this.programCounter = imm;
		};
	};

	op_c(rt, rs, rd) {
		this.set(this.registers[rd], this.get(this.registers[rs]));
	};

	op_l(rt, rs, rd) {
		this.registers[rd] = this.get(this.registers[rs]);
	};

	op_s(rt, rs, rd) {
		this.set(this.registers[rd], rs);
	};

	op_li(rs, rd, imm) {
		this.registers[rd] = this.get(imm);
	};

	op_si(rs, rd, imm) {
		this.set(imm, this.registers[rs]);
	};

	op_set(rs, rd, imm) {
		this.registers[rd] = Long.fromInt(imm, true);
	};
};

if (require.main === module) {
	const opt = minimist(process.argv.slice(2), { }), filename = opt._[0];

	if (!filename) {
		return console.log("Usage: node wvm.js [filename]");
	};

	let { offsets, handlers, meta, code } = Parser.open(filename);
	let vm = new WVM({ program: { offsets, handlers, meta, code } });
	console.log(vm.registers);
};
