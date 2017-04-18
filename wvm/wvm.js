#!/usr/bin/env node
let WASMC = require("../wasm/wasmc.js"),
	fs = require("fs"),
	Long = require("long"),
	_ = require("lodash"),
	minimist = require("minimist"),
	chalk = require("chalk"),
	Parser = require("./parser.js");

require("string.prototype.padstart").shim();
require("string.prototype.padend").shim();

const { EXCEPTIONS, R_TYPES, I_TYPES, J_TYPES, OPCODES, FUNCTS, REGISTER_OFFSETS, TRAPS } = require("../wasm/constants.js");

/** Class representing a virtual machine. */
class WVM {
	constructor({ memorySize=640000, program, memory: initial }={}) {
		if (typeof program != "object") {
			throw `Unable to load program.`;
		};

		if (!(initial instanceof Array)) {
			throw `Unable to load memory.`;
		};

		({ offsets: this.offsets, handlers: this.handlers, meta: this.meta, code: this.code} = program);
		this.initial = initial;
		this.memorySize = memorySize;
		this.resetMemory();
		this.resetRegisters();
		this.programCounter = this.offsets.$code;
		this.active = true;
		this.cycles = 0;
	};

	onTick() { };
	onSetWord(addr, val) { };
	onSetByte(addr, val) { };
	onPrintChar(val) { };

	loadInstruction() {
		return this.getWord(this.programCounter);
	};

	start() {
		while (this.active) {
			this.tick();
		};
	};

	tick() {
		let instr = Parser.parseInstruction(this.loadInstruction());
		if (!instr) {
			console.error(chalk.red("Invalid instruction:"), instr, this.loadInstruction().toString(2));
			return;
		};

		let skipPC = false;
		if (instr.op == "nop") {
		} else if (typeof this[`op_${instr.op}`] == "undefined") {
			console.warn(chalk.yellow(`Unimplemented ${instr.type} operation:`), instr.op);
		} else {
			const fn = this[`op_${instr.op}`].bind(this);
			if (instr.type == "r") {
				skipPC = fn(instr.rt, instr.rs, instr.rd, instr.shift, instr.funct);
			} else if (instr.type == "i") {
				skipPC = fn(instr.rs, instr.rd, instr.imm);
			} else if (instr.type == "j") {
				skipPC = fn(instr.rs, instr.addr);
			};
		};

		if (!skipPC) {
			this.programCounter += 8;
		};
		
		this.cycles++;
		this.onTick();
	};

	resetMemory() {
		this.memory = new Uint8Array(this.memorySize * 8);
		this.initial.forEach((long, i) => this.setWord(8*i, long));
	};

	resetRegisters() {
		this.registers = _.range(0, 128).map(() => Long.ZERO);
		this.registers[REGISTER_OFFSETS.stack] = Long.fromInt(8*(this.memorySize - 1), true);
	};

	getWord(k, signed=false) {
		k = k instanceof Long? k.toInt() : k;

		if (k % 8) {
			// This isn't supposed to happen, because the key is misaligned. (The key is supposed to be a multiple of 8.
			// In the future, this may cause an exception.
		};

		return new Long(this.memory[k+7] | this.memory[k+6] << 8 | this.memory[k+5] << 16 | this.memory[k+4] << 24, this.memory[k+3] << 32 | this.memory[k+2] << 40 | this.memory[k+1] << 48 | this.memory[k] << 56, signed);
	};


	setWord(k, v) {
		k = k instanceof Long? k.toInt() : k;
		v = v instanceof Long? v : Long.fromInt(v);

		if (k % 8) {
			// Another misalignment.
		};

		const mask = 0xff;
		for (let i = 0; i < 4; i++) {
			this.memory[k + 7 - i] = v.low >> 8*i & mask;
		};

		for (let i = 4; i < 8; i++) {
			this.memory[k + 7 - i] = v.high >> 8*i & mask;
		};

		this.onSetWord(k, v);
		return true;
	};

	/**
	 * Gets a byte from memory.
	 * @param {number} k - The index of the byte to get.
	 * @param {number} The byte at the given address.
	 */
	getByte(k) {
		return this.memory[k];
	};

	/**
	 * Sets a byte in memory to a given value.
	 * @param {number} k - The index of the byte to set.
	 * @param {number} v - The byte to write to memory.
	 * @return {boolean} A boolean representing whether the byte was successfully set.
	 */
	setByte(k, v) {
		this.memory[k] = v instanceof Long? v.low & 0xff : v;
		this.onSetByte(k, v);
		return true;
	};

	log(...args) {
		console.log(...args);
	};

	stop() {
		this.active = false;
		console.warn("Process halted.");
	};

	link() {
		this.registers[REGISTER_OFFSETS.return] = Long.fromInt(this.programCounter + 8, true);
	};

	op_add(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].toSigned().add(this.registers[rt].toSigned());
	};

	op_addu(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].toUnsigned().add(this.registers[rt].toUnsigned());
	};

	// mult

	op_sub(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].toSigned().subtract(this.registers[rt].toSigned());
	};

	op_subu(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].toUnsigned().subtract(this.registers[rt].toUnsigned());
	};

	// multu

	op_sll(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].toSigned().shiftLeft(this.registers[rt]);
	};

	op_srl(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].toSigned().shiftRightUnsigned(this.registers[rt]);
	};

	op_sra(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].toSigned().shiftRight(this.registers[rt]);
	};

	op_and(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].and(this.registers[rt]);
	};

	op_nand(rt, rs, rd) {
		let { low, high, unsigned } = this.registers[rs].and(this.registers[rt]); this.registers[rd] = new Long(~low, ~high, unsigned);
	};

	op_nor(rt, rs, rd) {
		let { low, high, unsigned } = this.registers[rs].or(this.registers[rt]);
		this.registers[rd] = new Long(~low, ~high, unsigned);
	};

	op_not(rt, rs, rd) {
		let { low, high, unsigned } = this.registers[rs];
		this.registers[rd] = new Long(~low, ~high, unsigned);
	};

	op_or(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].or(this.registers[rt]);
	};

	op_xnor(rt, rs, rd) {
		let { low, high, unsigned } = this.registers[rs].xor(this.registers[rt]); this.registers[rd] = new Long(~low, ~high, unsigned);
	};

	op_xor(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].xor(this.registers[rt]);
	};

	op_addi(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].toSigned().add(imm instanceof Long? imm.toSigned() : Long.fromInt(imm, false));
	};

	op_subi(rs, rd, imm) {
		this.registers[rd] = Long.fromInt(this.registers[rs], false).subtract(imm instanceof Long? imm.toSigned() : Long.fromInt(imm, false));
	};

	// multi

	op_addui(rs, rd, imm) {
		this.registers[rd] = Long.fromInt(this.registers[rs], true).add(imm instanceof Long? imm.toUnsigned() : Long.fromInt(imm, true));
	};

	op_subui(rs, rd, imm) {
		this.registers[rd] = Long.fromInt(this.registers[rs], true).subtract(imm instanceof Long? imm.toUnsigned() : Long.fromInt(imm, true));;
	};

	op_multui(rs, rd, imm) {
		let i = imm instanceof Long? imm.toUnsigned() : Long.fromInt(imm, true);
		let n = this.registers[rs].toUnsigned();

		let nhi = n.high;
		let nlo = n.low;

		let nhii = new Long(nhi, 0, true).multiply(i);
		let nloi = new Long(nlo, 0, true).multiply(i);

		let nhiihi = new Long(nhii.high,  0, true);
		let nhiilo = new Long(nhii.low,  0, true);
		let nloihi = new Long(nloi.high, 0, true);
		let nloilo = new Long(nloi.low,  0, true);

		let ab = nhiilo.add(nloihi);
		let ablo = ab.low;
		let abhi = ab.high;

		this.hi = nhiihi.add(abhi);
		this.lo = new Long(nloilo.toInt(), ablo.toInt(), true);
	};

	op_slli(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].toSigned().shiftLeft(imm instanceof Long? imm.toUnsigned() : imm);
	};

	op_srli(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].toSigned().shiftRightUnsigned(imm instanceof Long? imm.toUnsigned() : imm);
	};

	op_srai(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].toSigned().shiftRight(imm instanceof Long? imm.toUnsigned() : imm);
	};

	op_andi(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].and(imm);
	};

	op_nandi(rs, rd, imm) {
		let { low, high, unsigned } = this.registers[rs].and(imm);
		this.registers[rd] = new Long(~low, ~high, unsigned);
	};

	op_nori(rs, rd, imm) {
		let { low, high, unsigned } = this.registers[rs].or(imm);
		this.registers[rd] = new Long(~low, ~high, unsigned);
	};

	op_ori(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].or(imm);
	};

	op_xnori(rs, rd, imm) {
		let { low, high, unsigned } = this.registers[rs].xor(imm);
		this.registers[rd] = new Long(~low, ~high, unsigned);
	};

	op_xori(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].xor(imm);
	};

	op_mfhi(rt, rs, rd) {
		this.registers[rd] = this.hi;
	};

	op_mflo(rt, rs, rd) {
		this.registers[rd] = this.lo;
	};

	op_lui(rs, rd, imm) {
		this.registers[rd].high = imm;
		this.registers[rd].low = 0;
	};

	op_sl(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].toSigned().lessThan(this.registers[rt].toSigned())? Long.UONE : Long.UZERO;
	};

	op_sle(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].toSigned().lessThanOrEqual(this.registers[rt].toSigned())? Long.UONE : Long.UZERO;
	};

	op_seq(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].toUnsigned().equals(this.registers[rt].toUnsigned())? Long.UONE : Long.UZERO;
	};

	op_slu(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].toUnsigned().lessThan(this.registers[rt].toUnsigned())? Long.UONE : Long.UZERO;
	};

	op_sleu(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].toUnsigned().lessThanOrEqual(this.registers[rt].toUnsigned())? Long.UONE : Long.UZERO;
	};

	op_sli(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].toSigned().lessThan(Long.fromInt(imm, false))? Long.UONE : Long.UZERO;
	};

	op_slei(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].toSigned().lessThanOrEqual(Long.fromInt(imm, false))? Long.UONE : Long.UZERO;
	};

	op_seqi(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].toUnsigned().equals(Long.fromInt(imm, false))? Long.UONE : Long.UZERO;
	};

	op_slui(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].toUnsigned().lessThan(Long.fromInt(imm, true))? Long.UONE : Long.UZERO;
	};

	op_sleui(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].toUnsigned().lessThanOrEqual(Long.fromInt(imm, true))? Long.UONE : Long.UZERO;
	};

	op_j(rs, addr) {
		this.programCounter = Long.fromInt(addr, true).toInt();
		return true;
	};

	op_jc(rs, addr) {
		if (!this.registers[rs].equals(0)) {
			this.programCounter = Long.fromInt(addr, true).toInt();
			return true;
		};
	};

	op_jl(rs, addr) {
		this.link();
		return this.op_j(rs, addr);
	};

	op_jlc(rs, addr) {
		this.link();
		return this.op_jc(rs, addr);
	};

	op_jr(rt, rs, rd) {
		this.programCounter = this.registers[rd].toUnsigned().toInt();
		return true;
	};
	
	op_jrc(rt, rs, rd) {
		if (!this.registers[rs].equals(0)) {
			this.programCounter = this.registers[rd].toUnsigned().toInt();
			return true;
		};
	};

	op_jrl(rt, rs, rd) {
		this.link();
		return this.op_jr(rt, rs, rd);
	};
	
	op_jrlc(rt, rs, rd) {
		this.link();
		return this.op_jrc(rt, rs, rd);
	};

	op_c(rt, rs, rd) {
		this.setWord(this.registers[rd], this.getWord(this.registers[rs]));
	};

	op_l(rt, rs, rd) {
		this.registers[rd] = this.getWord(this.registers[rs]);
	};

	op_s(rt, rs, rd) {
		this.setWord(this.registers[rd], this.registers[rs]);
	};

	op_cb(rt, rs, rd) {
		this.setByte(this.registers[rd], this.getByte(this.registers[rs]));
	};

	op_lb(rt, rs, rd) {
		this.registers[rd] = Long.fromInt(this.getByte(this.registers[rs]));
	};

	op_sb(rt, rs, rd) {
		this.setByte(this.registers[rd], this.registers[rs].and(0xff));
	};

	op_li(rs, rd, imm) {
		this.registers[rd] = this.getWord(imm);
	};

	op_si(rs, rd, imm) {
		this.setWord(imm, this.registers[rs]);
	};

	op_lbi(rs, rd, imm) {
		this.registers[rd] = Long.fromInt(this.getByte(imm));
	};

	op_sbi(rs, rd, imm) {
		this.setByte(imm, this.registers[rs].and(0xff));
	};

	op_set(rs, rd, imm) {
		this.registers[rd] = Long.fromInt(imm, true);
	};

	op_trap(rt, rs, rd, shift, funct) {
		if (funct == TRAPS.printr) {
			this.log(`${Parser.getRegister(rs)}: 0x${this.registers[rs].toString(16)}`);
		} else if (funct == TRAPS.halt) {
			this.stop();
			return true;
		} else if (funct == TRAPS.eval) {
			console.warn("<eval> is currently unimplemented.");
		} else if (funct == TRAPS.printc) {
			this.onPrintChar(this.registers[rs].toUnsigned().toInt() & 0xff);
		} else { // This may be changed to an exception in the future.
			console.log("Unknown trap:", {rt, rs, rd, shift, func});
		};
	};

	get hi() { return this.registers[REGISTER_OFFSETS.hi] };
	get lo() { return this.registers[REGISTER_OFFSETS.lo] };
	set hi(to) { this.registers[REGISTER_OFFSETS.hi] = to };
	set lo(to) { this.registers[REGISTER_OFFSETS.lo] = to };
};

module.exports = WVM;

if (require.main === module) {
	const opt = minimist(process.argv.slice(2), { }), filename = opt._[0];

	if (!filename) {
		console.log("Usage: node wvm.js [filename]");
		process.exit(0);
	};

	let { parsed, raw } = Parser.open(filename);
	let { offsets, handlers, meta, code } = parsed;
	let vm = new WVM({ program: { offsets, handlers, meta, code }, memory: raw });
	vm.tick();
};

if (process.browser) {
	window.WVM = WVM;
	window.Parser = Parser;
};
