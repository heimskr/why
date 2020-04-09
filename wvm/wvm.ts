#!/usr/bin/env ts-node
import * as Long from "long";
import _ from "../util";
import Parser, {SegmentOffsets, ParserMeta, ParserInstruction} from "../wasm/parser";
import {SymbolTable} from "../wasm/wasmc";
import printansi from "./printansi.js";
const minimist = require("minimist");
const chalk = require("chalk");

require("string.prototype.padstart").shim();
require("string.prototype.padend").shim();

import {REGISTER_OFFSETS, EXTS, ALU_MASKS, RINGS, INTERRUPTS, Ring, ConditionName, isRing} from "../wasm/constants";

export type Program = {
	offsets: SegmentOffsets,
	meta: ParserMeta,
	code: ParserInstruction[],
	symbols: SymbolTable
};

/**
 * `wvm` is the virtual machine for why.js. It executes bytecode produced by `wasmc`.
 *
 * @module wvm
 */

/**
 * Class representing a virtual machine.
 */
export default class WVM {
	initial: Long[];
	memory: Uint8Array;
	registers: Long[];
	memorySize: number;
	meta: ParserMeta;
	code: ParserInstruction[];
	symbols: SymbolTable;
	programCounter: number;
	active: boolean;
	cycles: number;
	interruptTableAddress: number | null;
	timeoutID: number;
	prcBuffer: string;
	noBuffer: boolean;

	onPrintChar: ((val: string) => void) | null;
	onChangeRing: (newRing: number) => void = () => {};
	onSetByte: (addr: number, byte: number) => void = () => {};
	onSetWord: (addr: number, word: Long) => void = () => {};
	onSetHalfword: (addr: number, word: number) => void = () => {};
	onTick: () => void = () => {};
	
	private _ring: Ring;

	offsets: SegmentOffsets;
	constructor(program: Program, raw: Long[], memorySize: number = 640000) {
		if (typeof program != "object") {
			throw "Unable to load program.";
		}

		if (!(raw instanceof Array)) {
			throw "Unable to load memory.";
		}

		({
			offsets: this.offsets,
			meta: this.meta,
			code: this.code,
			symbols: this.symbols
		} = program);
		this.initial = raw;
		this.memorySize = memorySize;
		this.resetMemory();
		this.resetRegisters();
		this.programCounter = this.offsets.$code;
		this.active = true;
		this.cycles = 0;
		this.ring = RINGS.kernel;
		this.interruptTableAddress = null;
		this.timeoutID = 0;

		this.prcBuffer = "";
		this.noBuffer = false;

		this.onPrintChar /* (val) */ = null;
	}

	set ring(to) { this.onChangeRing(this._ring = to); }
	get ring() { return this._ring; }

	/**
	 * Returns the instruction currently pointed to by the program counter.
	 * @return {Long} A word containing an instruction.
	 */
	loadInstruction(): Long {
		return this.getWord(this.programCounter);
	}

	/**
	 * Starts executing instructions in a loop until the VM is no longer active.
	 */
	start(): void {
		while (this.active) {
			this.tick();
		}
	}

	/**
	 * Loads and executes a single instruction.
	 */
	tick(): void {
		const loaded = this.loadInstruction();
		let instr = Parser.parseInstruction(loaded);
		if (!instr) {
			console.error(`${chalk.red("Invalid instruction")} at ${this.programCounter}:`,
				"0x" + loaded.toString(16).padStart(16, "0"), "/", loaded.toString(2).padStart(64, "0"));
			this.stop();
			return;
		}

		let skipPC: boolean = false;
		if (instr.op == "nop") {
			// Do nothing.
		} else if (typeof this[`op_${instr.op}`] == "undefined") {
			console.warn(chalk.yellow(`Unimplemented ${instr.type} operation:`), instr.op);
		} else {
			const fn = this[`op_${instr.op}`].bind(this);
			if (instr.type == "r") {
				skipPC = !!fn(instr.rt, instr.rs, instr.rd, instr.funct, instr.conditions);
			} else if (instr.type == "i") {
				skipPC = !!fn(instr.rs, instr.rd, instr.imm);
			} else if (instr.type == "j") {
				skipPC = !!fn(instr.rs, instr.addr, instr.link, instr.conditions);
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
	resetMemory(): void {
		this.memory = new Uint8Array(this.memorySize * 8);
		this.initial.forEach((long, i) => this.setWord(8*i, long));
	}

	/**
	 * Resets all registers to zero, except the stack pointer, which is set to point at the end of the memory.
	 */
	resetRegisters(): void {
		this.registers = _.range(0, 128).map(() => Long.UZERO);
		this.sp = Long.fromInt(8*(this.memorySize - 1), true);
	}

	/**
	 * Gets a word from memory.
	 * @param {number|Long} k The index of the word to get.
	 * @return {Long} The word at the given address.
	 */
	getWord(k, signed=false) {
		k = k instanceof Long? k.toInt() : k;

		if (k % 8) {
			// This isn't supposed to happen, because the key is misaligned.
			// (The key is supposed to be a multiple of 8.)
			// In the future, this may cause an exception.
		}

		return new Long(
			this.memory[k]         | this.memory[k+1] <<  8 | this.memory[k+2] << 16 | this.memory[k+3] << 24,
			this.memory[k+4] << 32 | this.memory[k+5] << 40 | this.memory[k+6] << 48 | this.memory[k+7] << 56,
		signed);
	}

	/**
	 * @param {number|Long} k The index of the byte to set.
	 * @param {number|Long} v The word to write to memory.
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
			this.memory[k + i] = v.low >> 8*i & mask;
		}

		for (let i = 4; i < 8; i++) {
			this.memory[k + i] = v.high >> 8*i & mask;
		}

		this.onSetWord(k, v);
		return true;
	}

	/**
	 * Gets a halfword from memory.
	 * @param {number|Long} k The index of the halfword to get.
	 * @return {Long} The halfword at the given address.
	 */
	getHalfword(k, signed=false) {
		k = k instanceof Long? k.toInt() : k;

		if (k % 4) {
			// This isn't supposed to happen, because the key is misaligned.
			// (The key is supposed to be a multiple of 4.)
			// In the future, this may cause an exception.
		}

		return new Long(this.memory[k+3] | this.memory[k+2] << 8 | this.memory[k+1] << 16 | this.memory[k+0] << 24, 0,
			signed);
	}

	/**
	 * @param {number|Long} k The index of the byte to set.
	 * @param {number|Long} v The halfword to write to memory.
	 * @return {boolean} A boolean representing whether the halfword was successfully set.
	 */
	setHalfword(k, v) {
		k = k instanceof Long? k.toInt() : k;
		v = v instanceof Long? v.toInt() : v;

		if (k % 4) {
			// Another misalignment.
		}

		for (let i = 0; i < 4; i++) {
			this.memory[k + i] = v >> 8*i & 0xff;
		}

		this.onSetHalfword(k, v);
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

	get flagZ() { return !!(this.st.toInt() & ALU_MASKS.z); }
	get flagN() { return !!(this.st.toInt() & ALU_MASKS.n); }
	get flagC() { return !!(this.st.toInt() & ALU_MASKS.c); }
	get flagO() { return !!(this.st.toInt() & ALU_MASKS.o); }
	
	checkConditions(cond: ConditionName): boolean {
		switch (cond) {
			case "p": return !(this.flagZ || this.flagN);
			case "n": return this.flagN;
			case "z": return this.flagZ;
			case "nz": return !this.flagZ;
		}

		return true;
	}

	checkKernel(opName: string): boolean {
		if (RINGS.kernel < this.ring) {
			console.error(`Called ${opName} outside of kernel mode; halting.`);
			this.stop();
			return false;
		}

		return true;
	}

	interrupt(id: number, override: boolean = false): boolean {
		if (!this.interruptTableAddress) {
			console.warn("No interrupt table registered; ignoring interrupt.");
			return false;
		}

		const meta = Object.values(INTERRUPTS).filter((x) => x[0] == id)[0];
		if (!meta) {
			console.error("Invalid interrupt ID:", id);
			this.stop();
			return false;
		}

		const [, newRing, reqRing] = meta;

		if (!override && reqRing != -1 && reqRing < this.ring) {
			console.warn(`Insufficient privilege for interrupt ${id} (currently ${this.ring}, required ${reqRing});`,
			"ignoring interrupt.");
			return false;
		}

		this.e0 = Long.fromInt(this.programCounter + 8, true);
		this.e1 = Long.fromInt(this.ring, true);

		if (isRing(newRing)) {
			this.ring = newRing;
		}

		this.programCounter = this.getWord(this.interruptTableAddress + id * 8).toInt();
		return true;
	}

	timerExpired() {
		this.interrupt(INTERRUPTS.TIMER[0], true);
	}

	stackPush(val) {
		this.setWord(this.sp, val);
		this.sp = this.sp.subtract(8);
	}

	stackPop() {
		this.sp = this.sp.add(8);
		return this.getWord(this.sp);
	}

	setTimer(micro) {
		clearTimeout(this.timeoutID);
		this.timeoutID = setTimeout((() => this.timerExpired()) as TimerHandler, micro / 1e3);
	}

	op_add(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().add(this.registers[rt].toSigned()));
	}

	// mult

	op_sub(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().subtract(this.registers[rt].toSigned()));
	}

	// multu

	op_sll(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().shiftLeft(this.registers[rt]));
	}

	op_srl(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().shiftRightUnsigned(this.registers[rt]));
	}

	op_sra(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().shiftRight(this.registers[rt]));
	}

	op_mod(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().mod(this.registers[rt].toSigned()));
	}

	op_div(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().div(this.registers[rt].toSigned()));
	}

	op_divu(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].toUnsigned().div(this.registers[rt].toUnsigned()));
	}

	op_mult(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		let i = this.registers[rt].toSigned();
		let n = this.registers[rs].toSigned();

		let nhi = n.high;
		let nlo = n.low;

		let nhii = new Long(nhi, 0, false).multiply(i);
		let nloi = new Long(nlo, 0, false).multiply(i);

		let nhiihi = new Long(nhii.high, 0, false);
		let nhiilo = new Long(nhii.low,  0, false);
		let nloihi = new Long(nloi.high, 0, false);
		let nloilo = new Long(nloi.low,  0, false);

		let ab = nhiilo.add(nloihi);
		let ablo = ab.low;
		let abhi = ab.high;

		this.hi = nhiihi.add(abhi);
		this.lo = new Long(nloilo.toInt(), ablo, false);

		if (this.hi.isZero() && this.lo.isZero()) {
			this.updateFlags(0);
		} else if (this.hi.isNegative) {
			this.updateFlags(-1);
		} else {
			this.updateFlags(1);
		}
	}

	op_multu(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		let i = this.registers[rt].toUnsigned();
		let n = this.registers[rs].toUnsigned();

		let nhi = n.high;
		let nlo = n.low;

		let nhii = new Long(nhi, 0, true).multiply(i);
		let nloi = new Long(nlo, 0, true).multiply(i);

		let nhiihi = new Long(nhii.high, 0, true);
		let nhiilo = new Long(nhii.low,  0, true);
		let nloihi = new Long(nloi.high, 0, true);
		let nloilo = new Long(nloi.low,  0, true);

		let ab = nhiilo.add(nloihi);
		let ablo = ab.low;
		let abhi = ab.high;

		this.hi = nhiihi.add(abhi);
		this.lo = new Long(nloilo.toInt(), ablo, true);

		if (this.hi.isZero() && this.lo.isZero()) {
			this.updateFlags(0);
		} else if (this.hi.toSigned().isNegative) {
			this.updateFlags(-1);
		} else {
			this.updateFlags(1);
		}
	}

	op_and(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].and(this.registers[rt]));
	}

	op_nand(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		let { low, high, unsigned } = this.registers[rs].and(this.registers[rt]);
		this.updateFlags(this.registers[rd] = new Long(~low, ~high, unsigned));
	}

	op_nor(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		let { low, high, unsigned } = this.registers[rs].or(this.registers[rt]);
		this.updateFlags(this.registers[rd] = new Long(~low, ~high, unsigned));
	}

	op_not(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		let { low, high, unsigned } = this.registers[rs];
		this.updateFlags(this.registers[rd] = new Long(~low, ~high, unsigned));
	}

	op_or(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].or(this.registers[rt]));
	}

	op_xnor(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		let { low, high, unsigned } = this.registers[rs].xor(this.registers[rt]);
		this.updateFlags(this.registers[rd] = new Long(~low, ~high, unsigned));
	}

	op_xor(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].xor(this.registers[rt]));
	}

	op_land(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].and(this.registers[rt]).eq(0)? Long.ZERO : Long.ONE);
	}

	op_lnand(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].and(this.registers[rt]).eq(0)? Long.ONE : Long.ZERO);
	}

	op_lnor(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].or(this.registers[rt]).eq(0)? Long.ONE : Long.ZERO);
	}

	op_lnot(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].eq(0)? Long.ONE : Long.ZERO);
	}

	op_lor(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].or(this.registers[rt]).eq(0)? Long.ZERO : Long.ONE);
	}

	op_lxnor(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].xor(this.registers[rt]).eq(0)? Long.ONE : Long.ZERO);
	}

	op_lxor(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].xor(this.registers[rt]).eq(0)? Long.ZERO : Long.ONE);
	}

	op_addi(rs: number, rd: number, imm: Long): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().add(
			imm instanceof Long? imm.toSigned() : Long.fromInt(imm, false)));
	}

	op_subi(rs: number, rd: number, imm: Long): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].subtract(
			imm instanceof Long? imm.toSigned() : Long.fromInt(imm, false)));
	}

	op_modi(rs: number, rd: number, imm: Long): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().mod(
			imm instanceof Long? imm.toSigned() : Long.fromInt(imm, false)));
	}

	op_divi(rs: number, rd: number, imm: Long): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().div(
			imm instanceof Long? imm.toSigned() : Long.fromInt(imm, false)));
	}

	op_divui(rs: number, rd: number, imm: Long): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].toUnsigned().div(
			imm instanceof Long? imm.toUnsigned() : Long.fromInt(imm, true)));
	}

	op_multi(rs: number, rd: number, imm: Long): boolean | void {
		let i = imm instanceof Long? imm.toSigned() : Long.fromInt(imm, false);
		let n = this.registers[rs].toSigned();

		let nhi = n.high;
		let nlo = n.low;

		let nhii = new Long(nhi, 0, false).multiply(i);
		let nloi = new Long(nlo, 0, false).multiply(i);

		let nhiihi = new Long(nhii.high, 0, false);
		let nhiilo = new Long(nhii.low,  0, false);
		let nloihi = new Long(nloi.high, 0, false);
		let nloilo = new Long(nloi.low,  0, false);

		let ab = nhiilo.add(nloihi);
		let ablo = ab.low;
		let abhi = ab.high;

		this.hi = nhiihi.add(abhi);
		this.lo = new Long(nloilo.toInt(), ablo, false);

		if (this.hi.isZero() && this.lo.isZero()) {
			this.updateFlags(0);
		} else if (this.hi.isNegative) {
			this.updateFlags(-1);
		} else {
			this.updateFlags(1);
		}
	}

	op_multui(rs: number, rd: number, imm: Long): boolean | void {
		let i = imm instanceof Long? imm.toUnsigned() : Long.fromInt(imm, true);
		let n = this.registers[rs].toUnsigned();

		let nhi = n.high;
		let nlo = n.low;

		let nhii = new Long(nhi, 0, true).multiply(i);
		let nloi = new Long(nlo, 0, true).multiply(i);

		let nhiihi = new Long(nhii.high, 0, true);
		let nhiilo = new Long(nhii.low,  0, true);
		let nloihi = new Long(nloi.high, 0, true);
		let nloilo = new Long(nloi.low,  0, true);

		let ab = nhiilo.add(nloihi);
		let ablo = ab.low;
		let abhi = ab.high;

		this.hi = nhiihi.add(abhi);
		this.lo = new Long(nloilo.toInt(), ablo, true);

		if (this.hi.isZero() && this.lo.isZero()) {
			this.updateFlags(0);
		} else if (this.hi.toSigned().isNegative) {
			this.updateFlags(-1);
		} else {
			this.updateFlags(1);
		}
	}

	op_int(rs: number, rd: number, imm: Long): boolean | void {
		if (imm.isNegative() || imm.greaterThanOrEqual(Object.keys(INTERRUPTS).length)) {
			console.error("Invalid interrupt ID:", imm);
			this.stop();
			return;
		}

		return this.interrupt(imm.toUnsigned().toInt());
	}

	op_rit(rs: number, rd: number, imm: Long): boolean | void {
		if (this.checkKernel("rit")) {
			this.interruptTableAddress = imm instanceof Long? imm.toInt() : imm;
		}
	}

	op_time(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		if (this.checkKernel("time")) {
			this.setTimer(this.registers[rs].toUnsigned().toInt());
		}
	}

	op_timei(rs: number, rd: number, imm: Long): boolean | void {
		if (this.checkKernel("timei")) {
			this.setTimer(imm instanceof Long? imm.toInt() : imm);
		}
	}

	op_ring(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		const ring = this.registers[rs].toUnsigned().toInt();

		if (!isRing(ring)) {
			console.warn("Unknown ring:", ring);
		}

		if (ring < this.ring || !isRing(ring)) {
			this.interrupt(INTERRUPTS.PROTEC[0], true);
		} else {
			this.ring = ring;
		}
	}

	op_ringi(rs: number, rd: number, imm: Long): boolean | void {
		const ring = imm.toUnsigned().toInt();

		if (!isRing(ring)) {
			console.warn("Unknown ring:", ring);
		}

		if (ring < this.ring || !isRing(ring)) {
			this.interrupt(INTERRUPTS.PROTEC[0], true);
		} else {
			this.ring = ring;
		}
	}

	op_slli(rs: number, rd: number, imm: Long): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().shiftLeft(
			imm instanceof Long? imm.toUnsigned() : imm));
	}

	op_srli(rs: number, rd: number, imm: Long): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().shiftRightUnsigned(
			imm instanceof Long? imm.toUnsigned() : imm));
	}

	op_srai(rs: number, rd: number, imm: Long): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].toSigned().shiftRight(
			imm instanceof Long? imm.toUnsigned() : imm));
	}

	op_andi(rs: number, rd: number, imm: Long): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].and(imm));
	}

	op_nandi(rs: number, rd: number, imm: Long): boolean | void {
		let { low, high, unsigned } = this.registers[rs].and(imm);
		this.updateFlags(this.registers[rd] = new Long(~low, ~high, unsigned));
	}

	op_nori(rs: number, rd: number, imm: Long): boolean | void {
		let { low, high, unsigned } = this.registers[rs].or(imm);
		this.updateFlags(this.registers[rd] = new Long(~low, ~high, unsigned));
	}

	op_ori(rs: number, rd: number, imm: Long): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].or(imm));
	}

	op_xnori(rs: number, rd: number, imm: Long): boolean | void {
		let { low, high, unsigned } = this.registers[rs].xor(imm);
		this.updateFlags(this.registers[rd] = new Long(~low, ~high, unsigned));
	}

	op_xori(rs: number, rd: number, imm: Long): boolean | void {
		this.updateFlags(this.registers[rd] = this.registers[rs].xor(imm));
	}

	op_lui(rs: number, rd: number, imm: Long): boolean | void {
		this.registers[rd].high = imm.getLowBitsUnsigned();
	}

	op_sl(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.registers[rd] = this.registers[rs].toSigned().lessThan(this.registers[rt].toSigned())?
			Long.UONE : Long.UZERO;
	}

	op_sle(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.registers[rd] = this.registers[rs].toSigned().lessThanOrEqual(this.registers[rt].toSigned())?
			Long.UONE : Long.UZERO;
	}

	op_seq(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.registers[rd] = this.registers[rs].toUnsigned().equals(this.registers[rt].toUnsigned())?
			Long.UONE : Long.UZERO;
	}

	op_slu(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.registers[rd] = this.registers[rs].toUnsigned().lessThan(this.registers[rt].toUnsigned())?
			Long.UONE : Long.UZERO;
	}

	op_sleu(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.registers[rd] = this.registers[rs].toUnsigned().lessThanOrEqual(this.registers[rt].toUnsigned())?
			Long.UONE : Long.UZERO;
	}

	op_sli(rs: number, rd: number, imm: Long): boolean | void {
		this.registers[rd] = this.registers[rs].toSigned().lessThan(imm.toSigned())?
			Long.UONE : Long.UZERO;
	}

	op_slei(rs: number, rd: number, imm: Long): boolean | void {
		this.registers[rd] = this.registers[rs].toSigned().lessThanOrEqual(imm.toSigned())?
			Long.UONE : Long.UZERO;
	}

	op_seqi(rs: number, rd: number, imm: Long): boolean | void {
		this.registers[rd] = this.registers[rs].toUnsigned().equals(imm.toSigned())?
			Long.UONE : Long.UZERO;
	}

	op_sgi(rs: number, rd: number, imm: Long): boolean | void {
		this.registers[rd] = this.registers[rs].toSigned().greaterThan(imm.toSigned())?
			Long.UONE : Long.UZERO;
	}

	op_sgei(rs: number, rd: number, imm: Long): boolean | void {
		this.registers[rd] = this.registers[rs].toSigned().greaterThanOrEqual(imm.toSigned())?
			Long.UONE : Long.UZERO;
	}

	op_slui(rs: number, rd: number, imm: Long): boolean | void {
		this.registers[rd] = this.registers[rs].toUnsigned().lessThan(imm.toUnsigned())?
			Long.UONE : Long.UZERO;
	}

	op_sleui(rs: number, rd: number, imm: Long): boolean | void {
		this.registers[rd] = this.registers[rs].toUnsigned().lessThanOrEqual(imm.toUnsigned())?
			Long.UONE : Long.UZERO;
	}

	op_j(rs: number, addr: Long, link: boolean, cond: ConditionName): boolean | void {
		if (this.checkConditions(cond)) {
			if (link) this.link();
			if (addr.isZero() || !addr.isPositive()) {
				console.error("Invalid jump address:", addr);
				this.stop();
				return true;
			}

			this.programCounter = addr.toUnsigned().toInt();
			return true;
		}
	}

	op_jc(rs: number, addr: Long, link: boolean, cond: ConditionName): boolean | void {
		if (link) this.link();
		if (!this.registers[rs].equals(0)) {
			this.programCounter = addr.toUnsigned().toInt();
			return true;
		}
	}

	op_jr(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean {
		if (this.checkConditions(cond)) {
			const addr = this.registers[rd].toUnsigned().toInt();
			
			if (addr == 0) {
				console.error("Invalid jump address:", addr);
				this.stop();
				return true;
			}
			
			this.programCounter = addr;
			return true;
		}
	}
	
	op_jrc(rt: number, rs: number, rd: number): boolean {
		if (!this.registers[rs].equals(0)) {
			this.programCounter = this.registers[rd].toUnsigned().toInt();
			return true;
		}
		
		return false;
	}

	op_jrl(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean {
		if (this.checkConditions(cond)) {
			this.link();
			this.programCounter = this.registers[rd].toUnsigned().toInt();
			return true;
		}

		return false;
	}
	
	op_jrlc(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.link();
		return this.op_jrc(rt, rs, rd);
	}

	op_c(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.setWord(this.registers[rd], this.getWord(this.registers[rs]));
	}

	op_l(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.registers[rd] = this.getWord(this.registers[rs]);
	}

	op_s(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.setWord(this.registers[rd], this.registers[rs]);
	}

	op_cb(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.setByte(this.registers[rd], this.getByte(this.registers[rs]));
	}

	op_lb(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.registers[rd] = Long.fromInt(this.getByte(this.registers[rs]));
	}

	op_sb(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.setByte(this.registers[rd], this.registers[rs].and(0xff));
	}

	op_ch(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.setHalfword(this.registers[rd], this.getHalfword(this.registers[rs]));
	}

	op_lh(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.registers[rd] = this.getHalfword(this.registers[rs]);
	}

	op_sh(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.setHalfword(this.registers[rd], this.registers[rs].low);
	}

	op_spush(rt, rs) {
		this.stackPush(this.registers[rs]);
	}

	op_spop(rt: number, rs: number, rd: number, funct: number, cond: ConditionName): boolean | void {
		this.registers[rd] = this.stackPop();
	}

	op_li(rs: number, rd: number, imm: Long): boolean | void {
		this.registers[rd] = this.getWord(imm);
	}

	op_si(rs: number, rd: number, imm: Long): boolean | void {
		this.setWord(imm, this.registers[rs]);
	}

	op_lbi(rs: number, rd: number, imm: Long): boolean | void {
		this.registers[rd] = Long.fromInt(this.getByte(imm));
	}

	op_sbi(rs: number, rd: number, imm: Long): boolean | void {
		this.setByte(imm, this.registers[rs].and(0xff));
	}

	op_set(rs: number, rd: number, imm: Long): boolean | void {
		this.registers[rd] = imm;
	}

	op_ext(rt: number, rs: number, rd: number, funct: number): boolean {
		if (funct == EXTS.printr) {
			this.flushPrcBuffer();
			this.log(`${Parser.getRegister(rs)}: 0x${this.registers[rs].toString(16)}`);
		} else if (funct == EXTS.halt) {
			this.stop();
			return true;
		} else if (funct == EXTS.eval) {
			console.warn("<eval> is currently unimplemented.");
		} else if (funct == EXTS.prc) {
			const n = this.registers[rs].toUnsigned().toInt() & 0xff;
			const c = String.fromCharCode(n);
			if (this.onPrintChar) {
				this.onPrintChar(n.toString());
			} else if (this.noBuffer) {
				console.log(c);
			} else if (c == "\n") {
				this.flushPrcBuffer(true);
			} else {
				this.prcBuffer += c;
			}
		} else if (funct == EXTS.prd) {
			this.prcBuffer += this.registers[rs].toString();
		} else if (funct == EXTS.prx) {
			this.prcBuffer += "0x" + this.registers[rs].toUnsigned().toString(16);
		} else {
			console.log("Unknown external:", {rt, rs, rd, funct});
		}

		return false;
	}

	flushPrcBuffer(force: boolean = false): void {
		if (force || this.prcBuffer) {
			printansi(this.prcBuffer);
			this.prcBuffer = "";
		}
	}

	get hi() { return this.registers[REGISTER_OFFSETS.hi]; }
	get lo() { return this.registers[REGISTER_OFFSETS.lo]; }
	get sp() { return this.registers[REGISTER_OFFSETS.stack]; }
	get st() { return this.registers[REGISTER_OFFSETS.st]; }
	get e0() { return this.registers[REGISTER_OFFSETS.e]; }
	get e1() { return this.registers[REGISTER_OFFSETS.e + 1]; }
	set hi(to) { this.registers[REGISTER_OFFSETS.hi] = to; }
	set lo(to) { this.registers[REGISTER_OFFSETS.lo] = to; }
	set sp(to) { this.registers[REGISTER_OFFSETS.stack] = to; }
	set st(to) { this.registers[REGISTER_OFFSETS.st] = to; }
	set e0(to) { this.registers[REGISTER_OFFSETS.e] = to; }
	set e1(to) { this.registers[REGISTER_OFFSETS.e + 1] = to; }
}

module.exports = WVM;

if (require.main === module) {
	const opt = minimist(process.argv.slice(2), { }), filename = opt._[0];

	if (!filename) {
		console.log("Usage: node wvm.js [filename]");
		process.exit(0);
	}

	const parser = new Parser();
	parser.open(filename);
	const {offsets, meta, code, raw, symbols} = parser;
	const vm = new WVM({offsets, meta, code, symbols}, raw);
	vm.tick();
}

if ((<any> process).browser) {
	(<any> window).WVM = WVM;
	(<any> window).Parser = Parser;
}
