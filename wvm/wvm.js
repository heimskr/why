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

const WVM = module.exports = class WVM {
	static get DEFAULT_TTL() { return 500 };

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

		// Limits the number of clock cycles the VM will run (to prevent infinite loops).
		// This will be removed when the VM is stable enough, which may be soon.
		this.ttl = WVM.DEFAULT_TTL;
	};

	onTick() { };
	onSet(addr, val) { };

	loadInstruction() {
		return this.get(this.programCounter);
	};

	start() {
		while (this.active && this.ttl) {
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
				skipPC = !!fn(instr.rt, instr.rs, instr.rd, instr.shift, instr.funct);
			} else if (instr.type == "i") {
				skipPC = !!fn(instr.rs, instr.rd, instr.imm);
			} else if (instr.type == "j") {
				skipPC = !!fn(instr.rs, instr.addr);
			};
		};

		if (!skipPC) {
			this.programCounter++;
		};
		
		this.ttl--;
		this.onTick();
	};

	resetMemory() {
		this.memory = new Uint32Array(this.memorySize * 2);
		this.initial.forEach((long, i) => this.set(i, long));
	};

	resetRegisters() {
		this.registers = _.range(0, 128).map(() => Long.ZERO);
		this.registers[REGISTER_OFFSETS.stack] = Long.fromInt(this.memorySize - 1, true);
	};

	get(k, signed=false) {
		return new Long(this.memory[2*k], this.memory[2*k + 1], signed);
	};

	set(k, v_) {
		const v = v_ instanceof Long? v_ : Long.fromInt(v_);
		[this.memory[2*k], this.memory[2*k + 1]] = [v.low, v.high];
		this.onSet(k, v);
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
		this.registers[REGISTER_OFFSETS.return] = Long.fromInt(this.programCounter + 1, true);
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

	op_addiu(rs, rd, imm) {
		this.registers[rd] = Long.fromInt(this.registers[rs], true).add(imm instanceof Long? imm.toUnsigned() : Long.fromInt(imm, true));
	};

	op_subiu(rs, rd, imm) {
		this.registers[rd] = Long.fromInt(this.registers[rs], true).subtract(imm instanceof Long? imm.toUnsigned() : Long.fromInt(imm, true));;
	};

	op_multiu(rs, rd, imm) {
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
		this.registers[rd] = this.registers[rs].toSigned().equals(this.registers[rt].toSigned())? Long.UONE : Long.UZERO;
	};

	op_slu(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].toUnsigned().lessThan(this.registers[rt].toUnsigned())? Long.UONE : Long.UZERO;
	};

	op_sleu(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].toUnsigned().lessThanOrEqual(this.registers[rt].toUnsigned())? Long.UONE : Long.UZERO;
	};

	op_sequ(rt, rs, rd) {
		this.registers[rd] = this.registers[rs].toUnsigned().equals(this.registers[rt].toUnsigned())? Long.UONE : Long.UZERO;
	};

	op_sli(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].toSigned().lessThan(Long.fromInt(imm, false))? Long.UONE : Long.UZERO;
	};

	op_slei(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].toSigned().lessThanOrEqual(Long.fromInt(imm, false))? Long.UONE : Long.UZERO;
	};

	op_seqi(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].toSigned().equals(Long.fromInt(imm, false))? Long.UONE : Long.UZERO;
	};

	op_sliu(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].toUnsigned().lessThan(Long.fromInt(imm, true))? Long.UONE : Long.UZERO;
	};

	op_sleiu(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].toUnsigned().lessThanOrEqual(Long.fromInt(imm, true))? Long.UONE : Long.UZERO;
	};

	// Is there really any difference between seqiu and seqi, or between seq and sequ?
	op_seqiu(rs, rd, imm) {
		this.registers[rd] = this.registers[rs].toUnsigned().equals(Long.fromInt(imm, true))? Long.UONE : Long.UZERO;
	};

	op_j(rs, addr) {
		this.programCounter = Long.fromInt(addr, true).toInt();
		return true;
	};

	op_jc(rs, addr) {
		if (!this.registers[rs].equals(0)) {
			this.programCounter = Long.fromInt(imm, true).toInt();
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
		this.set(this.registers[rd], this.get(this.registers[rs]));
	};

	op_l(rt, rs, rd) {
		this.registers[rd] = this.get(this.registers[rs]);
	};

	op_s(rt, rs, rd) {
		this.set(this.registers[rd], this.registers[rs]);
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

	op_trap(rt, rs, rd, shift, funct) {
		if (funct == TRAPS.printr) {
			this.log(`${Parser.getRegister(rs)}: 0x${this.registers[rs].toString(16)}`);
		} else if (funct == TRAPS.halt) {
			this.stop();
			return true;
		} else { // This may be changed to an exception in the future.
			console.log("Unknown trap:", {rt, rs, rd, shift, func});
		};
	};


	get hi() { return this.registers[REGISTER_OFFSETS.hi] };
	get lo() { return this.registers[REGISTER_OFFSETS.lo] };
	set hi(to) { this.registers[REGISTER_OFFSETS.hi] = to };
	set lo(to) { this.registers[REGISTER_OFFSETS.lo] = to };
};

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
