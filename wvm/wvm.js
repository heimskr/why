#!/usr/bin/env node
let Long = require("long"),
	_ = require("lodash"),
	minimist = require("minimist"),
	chalk = require("chalk"),
	Parser = require("./parser.js"),
	printansi = require("./printansi.js");

require("string.prototype.padstart").shim();
require("string.prototype.padend").shim();

const {REGISTER_OFFSETS, TRAPS, ALU_MASKS} = require("../wasm/constants.js");

/**
 * `wvm` is the virtual machine for why.js. It executes bytecode produced by `wasmc`.
 *
 * @module wvm
 */

/**
 * Class representing a virtual machine.
 */
class WVM {
	constructor({ memorySize=640000, program, memory: initial }={}) {
		if (typeof program != "object") {
			throw "Unable to load program.";
		}

		if (!(initial instanceof Array)) {
			throw "Unable to load memory.";
		}

		({ offsets: this.offsets, handlers: this.handlers, meta: this.meta, code: this.code, symbols: this.symbols} = program);
		this.initial = initial;
		this.memorySize = memorySize;
		this.resetMemory();
		this.resetRegisters();
		this.programCounter = this.offsets.$code;
		this.active = true;
		this.cycles = 0;

		this.prcBuffer = "";
		this.noBuffer = false;

		this.onPrintChar /* (val) */ = null;
	}

	onTick() { }
	onSetWord(/*addr, val*/) { }
	onSetByte(/*addr, val*/) { }

	/**
	 * Returns the instruction currently pointed to by the program counter.
	 * @return {Long} A word containing an instruction.
	 */
	loadInstruction() {
		return this.getWord(this.programCounter);
	}

	/**
	 * Starts executing instructions in a loop until the VM is no longer active.
	 */
	start() {
		while (this.active) {
			this.tick();
		}
	}

	/**
	 * Loads and executes a single instruction.
	 */
	tick() {
		const loaded = this.loadInstruction();
		let instr = Parser.parseInstruction(loaded);
		if (!instr) {
			console.error(chalk.red("Invalid instruction:"), instr, loaded.toString(16).padStart(16, "0"), "/", loaded.toString(2).padStart(64, "0"));
			return;
		}

		let skipPC = false;
		if (instr.op == "nop") {
			// Do nothing.
		} else if (typeof this[`op_${instr.op}`] == "undefined") {
			console.warn(chalk.yellow(`Unimplemented ${instr.type} operation:`), instr.op);
		} else {
			const fn = this[`op_${instr.op}`].bind(this);
			if (instr.type == "r") {
				skipPC = fn(instr.rt, instr.rs, instr.rd, instr.funct, instr.conditions);
			} else if (instr.type == "i") {
				skipPC = fn(instr.rs, instr.rd, instr.imm);
			} else if (instr.type == "j") {
				skipPC = fn(instr.rs, instr.addr, instr.link, instr.conditions);
			}
		}

		if (!skipPC) {
			this.programCounter += 8;
		}
		
		this.cycles++;
		this.onTick();
	}

	/**
	 * Resets all values in the memory to zero.
	 */
	resetMemory() {
		this.memory = new Uint8Array(this.memorySize * 8);
		this.initial.forEach((long, i) => this.setWord(8*i, long));
	}

	/**
	 * Resets all registers to zero, except the stack pointer, which is set to point at the end of the memory.
	 */
	resetRegisters() {
		this.registers = _.range(0, 128).map(() => Long.ZERO);
		this.sp = Long.fromInt(8*(this.memorySize - 1), true);
	}

	/**
	 * Gets a word from memory.
	 * @param {number} k The index of the word to get.
	 * @return {number} The word at the given address.
	 */
	getWord(k, signed=false) {
		k = k instanceof Long? k.toInt() : k;

		if (k % 8) {
			// This isn't supposed to happen, because the key is misaligned. (The key is supposed to be a multiple of 8.
			// In the future, this may cause an exception.
		}

		return new Long(this.memory[k+7] | this.memory[k+6] << 8 | this.memory[k+5] << 16 | this.memory[k+4] << 24, this.memory[k+3] << 32 | this.memory[k+2] << 40 | this.memory[k+1] << 48 | this.memory[k] << 56, signed);
	}

	/**
	 * @param {number} k The index of the byte to set.
	 * @param {number} v The word to write to memory.
	 * @return {boolean} A boolean representing whether the word was successfully set.
	 */
	setWord(k, v) {
		k = k instanceof Long? k.toInt() : k;
		v = v instanceof Long? v : Long.fromInt(v);

		if (k % 8) {
			// Another misalignment.
		}

		const mask = 0xff;
		for (let i = 0; i < 4; i++) {
			this.memory[k + 7 - i] = v.low >> 8*i & mask;
		}

		for (let i = 4; i < 8; i++) {
			this.memory[k + 7 - i] = v.high >> 8*i & mask;
		}

		this.onSetWord(k, v);
		return true;
	}

	/**
	 * Gets a byte from memory.
	 * @param {number} k - The index of the byte to get.
	 * @return {number} The byte at the given address.
	 */
	getByte(k) {
		return this.memory[k];
	}

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
	}

	/**
	 * Finds all symbols present at a given address.
	 * @param {number|Long} addr An address.
	 * @return {string[]} An array of symbol names.
	 */
	symbolsAt(addr) {
		return _.keys(this.symbols).filter((k) => this.symbols[k][1].eq(addr));
	}

	log(...args) {
		console.log(...args);
	}

	stop() {
		this.active = false;
		console.warn("Process halted.");
	}

	link() {
		this.registers[REGISTER_OFFSETS.return] = Long.fromInt(this.programCounter + 8, true);
	}

	clearFlags() {
		this.st = this.st.and(~0b1111);
	}

	updateFlags(n) {
		this.clearFlags();
		if (n instanceof Long) {
			if (n.isZero()) {
				this.st = this.st.or(ALU_MASKS.z);
			} else if (n.isNegative()) {
				this.st = this.st.or(ALU_MASKS.n);
			}
		} else {
			if (n == 0) {
				this.st = this.st.or(ALU_MASKS.z);
			} else if (n < 0) {
				this.st = this.st.or(ALU_MASKS.n);
			}
		}
	}

	get flagZ() { return !!(this.st & ALU_MASKS.z); }
	get flagN() { return !!(this.st & ALU_MASKS.n); }
	get flagC() { return !!(this.st & ALU_MASKS.c); }
	get flagO() { return !!(this.st & ALU_MASKS.o); }
	
	checkConditions(cond) {
		switch (cond) {
			case "p": return !(this.flagZ || this.flagN);
			case "n": return this.flagN;
			case "z": return this.flagZ;
			case "nz": return !this.flagZ;
		}

		return true;
	}

	op_add(rt, rs, rd) {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().add(this.registers[rt].toSigned()));
	}

	op_addu(rt, rs, rd) {
		this.updateFlags(this.registers[rd] = this.registers[rs].toUnsigned().add(this.registers[rt].toUnsigned()));
	}

	// mult

	op_sub(rt, rs, rd) {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().subtract(this.registers[rt].toSigned()));
	}

	op_subu(rt, rs, rd) {
		this.updateFlags(this.registers[rd] = this.registers[rs].toUnsigned().subtract(this.registers[rt].toUnsigned()));
	}

	// multu

	op_sll(rt, rs, rd) {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().shiftLeft(this.registers[rt]));
	}

	op_srl(rt, rs, rd) {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().shiftRightUnsigned(this.registers[rt]));
	}

	op_sra(rt, rs, rd) {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().shiftRight(this.registers[rt]));
	}

	op_mod(rt, rs, rd) {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().mod(this.registers[rt].toSigned()));
	}

	op_and(rt, rs, rd) {
		this.updateFlags(this.registers[rd] = this.registers[rs].and(this.registers[rt]));
	}

	op_nand(rt, rs, rd) {
		let { low, high, unsigned } = this.registers[rs].and(this.registers[rt]);
		this.updateFlags(this.registers[rd] = new Long(~low, ~high, unsigned));
	}

	op_nor(rt, rs, rd) {
		let { low, high, unsigned } = this.registers[rs].or(this.registers[rt]);
		this.updateFlags(this.registers[rd] = new Long(~low, ~high, unsigned));
	}

	op_not(rt, rs, rd) {
		let { low, high, unsigned } = this.registers[rs];
		this.updateFlags(this.registers[rd] = new Long(~low, ~high, unsigned));
	}

	op_or(rt, rs, rd) {
		this.updateFlags(this.registers[rd] = this.registers[rs].or(this.registers[rt]));
	}

	op_xnor(rt, rs, rd) {
		let { low, high, unsigned } = this.registers[rs].xor(this.registers[rt]);
		this.updateFlags(this.registers[rd] = new Long(~low, ~high, unsigned));
	}

	op_xor(rt, rs, rd) {
		this.updateFlags(this.registers[rd] = this.registers[rs].xor(this.registers[rt]));
	}

	op_land(rt, rs, rd) {
		this.updateFlags(this.registers[rd] = this.registers[rs].and(this.registers[rt]).eq(0)? Long.ZERO : Long.ONE);
	}

	op_lnand(rt, rs, rd) {
		this.updateFlags(this.registers[rd] = this.registers[rs].and(this.registers[rt]).eq(0)? Long.ONE : Long.ZERO);
	}

	op_lnor(rt, rs, rd) {
		this.updateFlags(this.registers[rd] = this.registers[rs].or(this.registers[rt]).eq(0)? Long.ONE : Long.ZERO);
	}

	op_lnot(rt, rs, rd) {
		this.updateFlags(this.registers[rd] = this.registers[rs].eq(0)? Long.ONE : Long.ZERO);
	}

	op_lor(rt, rs, rd) {
		this.updateFlags(this.registers[rd] = this.registers[rs].or(this.registers[rt]).eq(0)? Long.ZERO : Long.ONE);
	}

	op_lxnor(rt, rs, rd) {
		this.updateFlags(this.registers[rd] = this.registers[rs].xor(this.registers[rt]).eq(0)? Long.ONE : Long.ZERO);
	}

	op_lxor(rt, rs, rd) {
		this.updateFlags(this.registers[rd] = this.registers[rs].xor(this.registers[rt]).eq(0)? Long.ZERO : Long.ONE);
	}

	op_addi(rs, rd, imm) {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().add(imm instanceof Long? imm.toSigned() : Long.fromInt(imm, false)));
	}

	op_subi(rs, rd, imm) {
		this.updateFlags(this.registers[rd] = Long.fromInt(this.registers[rs], false).subtract(imm instanceof Long? imm.toSigned() : Long.fromInt(imm, false)));
	}

	op_modi(rs, rd, imm) {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().mod(imm instanceof Long? imm.toSigned() : Long.fromInt(imm, false)));
	}

	// multi

	op_addui(rs, rd, imm) {
		this.updateFlags(this.registers[rd] = Long.fromInt(this.registers[rs], true).add(imm instanceof Long? imm.toUnsigned() : Long.fromInt(imm, true)));
	}

	op_subui(rs, rd, imm) {
		this.updateFlags(this.registers[rd] = Long.fromInt(this.registers[rs], true).subtract(imm instanceof Long? imm.toUnsigned() : Long.fromInt(imm, true)));
	}

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

		if (this.hi.isZero() && this.lo.isZero()) {
			this.updateFlags(0);
		} else if (this.hi.toSigned().isNegative) {
			this.updateFlags(-1);
		} else {
			this.updateFlags(1);
		}
	}

	op_slli(rs, rd, imm) {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().shiftLeft(imm instanceof Long? imm.toUnsigned() : imm));
	}

	op_srli(rs, rd, imm) {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().shiftRightUnsigned(imm instanceof Long? imm.toUnsigned() : imm));
	}

	op_srai(rs, rd, imm) {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().shiftRight(imm instanceof Long? imm.toUnsigned() : imm));
	}

	op_andi(rs, rd, imm) {
		this.updateFlags(this.registers[rd] = this.registers[rs].and(imm));
	}

	op_nandi(rs, rd, imm) {
		let { low, high, unsigned } = this.registers[rs].and(imm);
		this.updateFlags(this.registers[rd] = new Long(~low, ~high, unsigned));
	}

	op_nori(rs, rd, imm) {
		let { low, high, unsigned } = this.registers[rs].or(imm);
		this.updateFlags(this.registers[rd] = new Long(~low, ~high, unsigned));
	}

	op_ori(rs, rd, imm) {
		this.updateFlags(this.registers[rd] = this.registers[rs].or(imm));
	}

	op_xnori(rs, rd, imm) {
		let { low, high, unsigned } = this.registers[rs].xor(imm);
		this.updateFlags(this.registers[rd] = new Long(~low, ~high, unsigned));
	}

	op_xori(rs, rd, imm) {
		this.updateFlags(this.registers[rd] = this.registers[rs].xor(imm));
	}

	op_lui(rs, rd, imm) {
		this.registers[rd].high = imm;
	}

	op_sl(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].toSigned().lessThan(this.registers[rt].toSigned())? Long.UONE : Long.UZERO;
	}

	op_sle(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].toSigned().lessThanOrEqual(this.registers[rt].toSigned())? Long.UONE : Long.UZERO;
	}

	op_seq(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].toUnsigned().equals(this.registers[rt].toUnsigned())? Long.UONE : Long.UZERO;
	}

	op_slu(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].toUnsigned().lessThan(this.registers[rt].toUnsigned())? Long.UONE : Long.UZERO;
	}

	op_sleu(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].toUnsigned().lessThanOrEqual(this.registers[rt].toUnsigned())? Long.UONE : Long.UZERO;
	}

	op_sli(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].toSigned().lessThan(Long.fromInt(imm, false))? Long.UONE : Long.UZERO;
	}

	op_slei(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].toSigned().lessThanOrEqual(Long.fromInt(imm, false))? Long.UONE : Long.UZERO;
	}

	op_seqi(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].toUnsigned().equals(Long.fromInt(imm, false))? Long.UONE : Long.UZERO;
	}

	op_sgi(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].toSigned().greaterThan(Long.fromInt(imm, false))? Long.UONE : Long.UZERO;
	}

	op_sgei(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].toSigned().greaterThanOrEqual(Long.fromInt(imm, false))? Long.UONE : Long.UZERO;
	}

	op_slui(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].toUnsigned().lessThan(Long.fromInt(imm, true))? Long.UONE : Long.UZERO;
	}

	op_sleui(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].toUnsigned().lessThanOrEqual(Long.fromInt(imm, true))? Long.UONE : Long.UZERO;
	}

	op_j(rs, addr, link, cond) {
		if (this.checkConditions(cond)) {
			if (link) this.link();
			this.programCounter = Long.fromInt(addr, true).toInt();
			return true;
		}
	}

	op_jc(rs, addr, link, cond) {
		if (link) this.link();
		if (!this.registers[rs].equals(0)) {
			this.programCounter = Long.fromInt(addr, true).toInt();
			return true;
		}
	}

	op_jr(rt, rs, rd, funct, cond) {
		if (this.checkConditions(cond)) {
			this.programCounter = this.registers[rd].toUnsigned().toInt();
			return true;
		}
	}
	
	op_jrc(rt, rs, rd) {
		if (!this.registers[rs].equals(0)) {
			this.programCounter = this.registers[rd].toUnsigned().toInt();
			return true;
		}
	}

	op_jrl(rt, rs, rd, funct, cond) {
		if (this.checkConditions(cond)) {
			this.link();
			this.programCounter = this.registers[rd].toUnsigned().toInt();
			return true;
		}
	}
	
	op_jrlc(rt, rs, rd) {
		this.link();
		return this.op_jrc(rt, rs, rd);
	}

	op_c(rt, rs, rd) {
		this.setWord(this.registers[rd], this.getWord(this.registers[rs]));
	}

	op_l(rt, rs, rd) {
		this.registers[rd] = this.getWord(this.registers[rs]);
	}

	op_s(rt, rs, rd) {
		this.setWord(this.registers[rd], this.registers[rs]);
	}

	op_cb(rt, rs, rd) {
		this.setByte(this.registers[rd], this.getByte(this.registers[rs]));
	}

	op_lb(rt, rs, rd) {
		this.registers[rd] = Long.fromInt(this.getByte(this.registers[rs]));
	}

	op_sb(rt, rs, rd) {
		this.setByte(this.registers[rd], this.registers[rs].and(0xff));
	}

	op_spush(rt, rs) {
		this.setWord(this.sp, this.registers[rs]);
		this.sp = this.sp.subtract(8);
	}

	op_spop(rt, rs, rd) {
		this.sp = this.sp.add(8);
		this.registers[rd] = this.getWord(this.sp);
	}

	op_li(rs, rd, imm) {
		this.registers[rd] = this.getWord(imm);
	}

	op_si(rs, rd, imm) {
		this.setWord(imm, this.registers[rs]);
	}

	op_lbi(rs, rd, imm) {
		this.registers[rd] = Long.fromInt(this.getByte(imm));
	}

	op_sbi(rs, rd, imm) {
		this.setByte(imm, this.registers[rs].and(0xff));
	}

	op_set(rs, rd, imm) {
		this.registers[rd] = Long.fromInt(imm, true);
	}

	op_trap(rt, rs, rd, funct) {
		if (funct == TRAPS.printr) {
			this.flushPrcBuffer();
			this.log(`${Parser.getRegister(rs)}: 0x${this.registers[rs].toString(16)}`);
		} else if (funct == TRAPS.halt) {
			this.stop();
			return true;
		} else if (funct == TRAPS.eval) {
			console.warn("<eval> is currently unimplemented.");
		} else if (funct == TRAPS.prc) {
			const n = this.registers[rs].toUnsigned().toInt() & 0xff;
			const c = String.fromCharCode(n);
			if (this.onPrintChar) {
				this.onPrintChar(n);
			} else if (this.noBuffer) {
				console.log(c);
			} else if (c == "\n") {
				this.flushPrcBuffer(true);
			} else {
				this.prcBuffer += c;
			}
		} else if (funct == TRAPS.prd) {
			this.prcBuffer += this.registers[rs].toString();
		} else if (funct == TRAPS.prx) {
			this.prcBuffer += "0x" + this.registers[rs].toString(16);
		} else {
			console.log("Unknown trap:", {rt, rs, rd, shift, funct});
		}
	}

	flushPrcBuffer(force = false) {
		if (force || this.prcBuffer) {
			printansi(this.prcBuffer);
			this.prcBuffer = "";
		}
	}

	get hi() { return this.registers[REGISTER_OFFSETS.hi]; }
	get lo() { return this.registers[REGISTER_OFFSETS.lo]; }
	get sp() { return this.registers[REGISTER_OFFSETS.stack]; }
	get st() { return this.registers[REGISTER_OFFSETS.st]; }
	set hi(to) { this.registers[REGISTER_OFFSETS.hi] = to; }
	set lo(to) { this.registers[REGISTER_OFFSETS.lo] = to; }
	set sp(to) { this.registers[REGISTER_OFFSETS.stack] = to; }
	set st(to) { this.registers[REGISTER_OFFSETS.st] = to; }
}

module.exports = WVM;

if (require.main === module) {
	const opt = minimist(process.argv.slice(2), { }), filename = opt._[0];

	if (!filename) {
		console.log("Usage: node wvm.js [filename]");
		process.exit(0);
	}

	let { parsed, raw } = Parser.open(filename);
	let { offsets, handlers, meta, code } = parsed;
	let vm = new WVM({ program: { offsets, handlers, meta, code }, memory: raw });
	vm.tick();
}

if (process.browser) {
	window.WVM = WVM;
	window.Parser = Parser;
}
