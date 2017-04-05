#!/usr/local/bin/node
let WASMC = require("../wasm/wasmc.js"),
	fs = require("fs"),
	Long = require("long"),
	_ = require("lodash"),
	minimist = require("minimist"),
	chalk = require("chalk"),
	Parser = require("./parser.js");

chalk = new chalk.constructor({ enabled: !process.browser });

require("string.prototype.padstart").shim();
require("string.prototype.padend").shim();

const { EXCEPTIONS, R_TYPES, I_TYPES, J_TYPES, OPCODES, FUNCTS, REGISTER_OFFSETS, TRAPS } = require("../wasm/constants.js");

const WVM = module.exports = class WVM {
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
		this.ttl = 50;
	};

	onTick() { };
	onSet(addr, val) { };

	loadInstruction() {
		return this.get(this.programCounter);
	};

	start() {
		while (this.active && this.ttl--) {
			this.tick();
		};
	};

	tick() {
		let instr = Parser.parseInstruction(this.loadInstruction());
		if (!instr) {
			console.error(chalk.red("Invalid instruction:"), instr, this.loadInstruction().toString(2));
			return;
		};

		console.log(`[${this.programCounter.toString().padStart(4, " ")}]`, "Instruction:", instr);
		if (typeof this[`op_${instr.op}`] == "undefined") {
			console.warn(chalk.yellow(`Unimplemented ${instr.type} operation:`), instr.op);
		} else {
			const fn = this[`op_${instr.op}`].bind(this);
			if (instr.type == "r") {
				fn(instr.rt, instr.rs, instr.rd, instr.shift, instr.funct);
			} else if (instr.type == "i") {
				fn(instr.rs, instr.rd, instr.imm);
			} else if (instr.type == "j") {
				fn(instr.rs, instr.addr);
			};
		};

		this.programCounter++;
		this.onTick();
	};

	resetMemory() {
		this.memory = new Uint32Array(this.memorySize * 2);
		this.initial.forEach((long, i) => this.set(i, long));
	};

	resetRegisters() {
		this.registers = this.regs = _.range(0, 128).map(() => Long.ZERO);
		this.registers[REGISTER_OFFSETS.stack] = this.memorySize - 1;
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

		this.onSet(k, v);
		return true;
	};

	op_trap(rt, rs, rd, shift, funct) {
		if (funct == TRAPS.printr) {
			console.log(`${Parser.getRegister(rs)}:`, this.registers[rs]);
		} else if (funct == TRAPS.halt) {
			console.warn("Process halted.");
			this.active = false;
		} else { // This may be changed to an exception in the future.
			console.log("Unknown trap:", {rt, rs, rd, shift, func});
		};
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

	// mult
	// multu
	// multi

	// Yeah, this is unreadable. :~)
	op_mfhi  (rt, rs, rd)  { this.regs[rd] = this.hi                                                                                              };
	op_mflo  (rt, rs, rd)  { this.regs[rd] = this.lo                                                                                              };
	op_s     (rt, rs, rd)  { this.set(this.regs[rd], rs)                                                                                          };
	op_si    (rs, rd, imm) { this.set(imm, this.regs[rs])                                                                                         };
	op_li    (rs, rd, imm) { this.regs[rd] = this.get(imm);                                                                                       };
	op_ori   (rs, rd, imm) { this.regs[rd] = this.regs[rs].or(imm)                                                                                };
	op_andi  (rs, rd, imm) { this.regs[rd] = this.regs[rs].and(imm)                                                                               };
	op_xori  (rs, rd, imm) { this.regs[rd] = this.regs[rs].xor(imm)                                                                               };
	op_l     (rt, rs, rd)  { this.regs[rd] = this.get(this.regs[rs])                                                                              };
	op_set   (rs, rd, imm) { this.regs[rd] = Long.fromInt(imm, true)                                                                              };
	op_or    (rt, rs, rd)  { this.regs[rd] = this.regs[rs].or(this.regs[rt])                                                                      };
	op_lui   (rs, rd, imm) { this.regs[rd].high = imm; this.regs[rd].low = 0                                                                      };
	op_and   (rt, rs, rd)  { this.regs[rd] = this.regs[rs].and(this.regs[rt])                                                                     };
	op_xor   (rt, rs, rd)  { this.regs[rd] = this.regs[rs].xor(this.regs[rt])                                                                     };
	op_c     (rt, rs, rd)  { this.set(this.regs[rd], this.get(this.regs[rs]))                                                                     };
	op_j     (rs, addr)    { this.programCounter = Long.fromInt(imm, true).toInt()                                                                };
	op_jr    (rt, rs, rd)  { this.programCounter = this.regs[rd].toUnsigned().toInt()                                                             };
	op_add   (rt, rs, rd)  { this.regs[rd] = this.regs[rs].toSigned().add(this.regs[rt].toSigned())                                               };
	op_addu  (rt, rs, rd)  { this.regs[rd] = this.regs[rs].toUnsigned().add(this.regs[rt].toUnsigned())                                           };
	op_sub   (rt, rs, rd)  { this.regs[rd] = this.regs[rs].toSigned().subtract(this.regs[rt].toSigned())                                          };
	op_subu  (rt, rs, rd)  { this.regs[rd] = this.regs[rs].toUnsigned().subtract(this.regs[rt].toUnsigned())                                      };
	op_jc    (rs, addr)    { if (!this.regs[rd].equals(0)) this.programCounter = Long.fromInt(imm, true).toInt()                                  };
	op_jrc   (rt, rs, rd)  { if (!this.regs[rs].equals(0)) this.programCounter = this.regs[rd].toUnsigned().toInt()                               };
	op_not   (rt, rs, rd)  { let { low, high, unsigned } = this.regs[rs]; this.regs[rd] = new Long(~low, ~high, unsigned)                         };
	op_seq   (rt, rs, rd)  { this.regs[rd] = this.regs[rs].toSigned().equals(this.regs[rt].toSigned())? Long.UONE : Long.UZERO                    };
	op_seqi  (rs, rd, imm) { this.regs[rd] = this.regs[rs].toSigned().equals(Long.fromInt(imm, false))? Long.UONE : Long.UZERO                    };
	op_seqiu (rs, rd, imm) { this.regs[rd] = this.regs[rs].toUnsigned().equals(Long.fromInt(imm, true))? Long.UONE : Long.UZERO                   };
	op_sl    (rt, rs, rd)  { this.regs[rd] = this.regs[rs].toSigned().lessThan(this.regs[rt].toSigned())? Long.UONE : Long.UZERO                  };
	op_sli   (rs, rd, imm) { this.regs[rd] = this.regs[rs].toSigned().lessThan(Long.fromInt(imm, false))? Long.UONE : Long.UZERO                  };
	op_nori  (rs, rd, imm) { let { low, high, unsigned } = this.regs[rs].or(imm); this.regs[rd] = new Long(~low, ~high, unsigned)                 };
	op_sliu  (rs, rd, imm) { this.regs[rd] = this.regs[rs].toUnsigned().lessThan(Long.fromInt(imm, true))? Long.UONE : Long.UZERO                 };
	op_sliu  (rs, rd, imm) { this.regs[rd] = this.regs[rs].toUnsigned().lessThan(Long.fromInt(imm, true))? Long.UONE : Long.UZERO                 };
	op_nandi (rs, rd, imm) { let { low, high, unsigned } = this.regs[rs].and(imm); this.regs[rd] = new Long(~low, ~high, unsigned)                };
	op_xnori (rs, rd, imm) { let { low, high, unsigned } = this.regs[rs].xor(imm); this.regs[rd] = new Long(~low, ~high, unsigned)                };
	op_sequ  (rt, rs, rd)  { this.regs[rd] = this.regs[rs].toUnsigned().equals(this.regs[rt].toUnsigned())? Long.UONE : Long.UZERO                };
	op_slu   (rt, rs, rd)  { this.regs[rd] = this.regs[rs].toUnsigned().lessThan(this.regs[rt].toUnsigned())? Long.UONE : Long.UZERO              };
	op_sle   (rt, rs, rd)  { this.regs[rd] = this.regs[rs].toSigned().lessThanOrEqual(this.regs[rt].toSigned())? Long.UONE : Long.UZERO           };
	op_slei  (rs, rd, imm) { this.regs[rd] = this.regs[rs].toSigned().lessThanOrEqual(Long.fromInt(imm, false))? Long.UONE : Long.UZERO           };
	op_sleiu (rs, rd, imm) { this.regs[rd] = this.regs[rs].toUnsigned().lessThanOrEqual(Long.fromInt(imm, true))? Long.UONE : Long.UZERO          };
	op_addi  (rs, rd, imm) { this.regs[rd] = this.regs[rs].toSigned().add(imm instanceof Long? imm.toSigned() : Long.fromInt(imm, false))         };
	op_sleu  (rt, rs, rd)  { this.regs[rd] = this.regs[rs].toUnsigned().lessThanOrEqual(this.regs[rt].toUnsigned())? Long.UONE : Long.UZERO       };
	op_nor   (rt, rs, rd)  { let { low, high, unsigned } = this.regs[rs].or(this.regs[rt]); this.regs[rd] = new Long(~low, ~high, unsigned)       };
	op_nand  (rt, rs, rd)  { let { low, high, unsigned } = this.regs[rs].and(this.regs[rt]); this.regs[rd] = new Long(~low, ~high, unsigned)      };
	op_nxor  (rt, rs, rd)  { let { low, high, unsigned } = this.regs[rs].xor(this.regs[rt]); this.regs[rd] = new Long(~low, ~high, unsigned)      };
	op_addiu (rs, rd, imm) { this.regs[rd] = this.regs[rs].toUnsigned().add(imm instanceof Long? imm.toUnsigned() : Long.fromInt(imm, true))      };
	op_subi  (rs, rd, imm) { this.regs[rd] = this.regs[rs].toSigned().subtract(imm instanceof Long? imm.toSigned() : Long.fromInt(imm, false))    };
	op_subiu (rs, rd, imm) { this.regs[rd] = this.regs[rs].toUnsigned().subtract(imm instanceof Long? imm.toUnsigned() : Long.fromInt(imm, true)) };

	// Is there really any difference between seqiu and seqi, or between seq and sequ?

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
