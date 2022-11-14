#include <bitset>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <unistd.h>

#include "lib/ansi.h"
#include "mult.h"
#include "Operations.h"
#include "Util.h"
#include "VM.h"

// If set, the server will print a notice each time paging is enabled or disabled or when the page table address is
// changed.
#define DEBUG_PAGING

namespace WVM::Operations {
	std::set<int> RSet {
		OP_RMATH, OP_AND, OP_RLOGIC, OP_RCOMP, OP_RJUMP, OP_RMEM, OP_TIME, OP_RING, OP_SEL, OP_PAGE, OP_QUERY, OP_REXT,
		OP_INTERRUPTS, OP_TRANS,
	};

	std::set<int> ISet {
		OP_ADDI, OP_SUBI, OP_MULTI, OP_SLLI, OP_SRLI, OP_SRAI, OP_MODI, OP_DIVI, OP_DIVII, OP_ANDI, OP_NANDI, OP_NORI,
		OP_ORI, OP_XNORI, OP_XORI, OP_LUI, OP_SLI, OP_SLEI, OP_CMPI, OP_SEQI, OP_SGI, OP_SGEI, OP_LI, OP_SI, OP_SET,
		OP_LNI, OP_INT, OP_RIT, OP_TIMEI, OP_RINGI, OP_SSPUSH, OP_SSPOP, OP_SLLII, OP_SRLII, OP_SRAII,
	};

	std::set<int> JSet {OP_J, OP_JC};

	void execute(VM &vm, UWord instruction) {
		instruction = Util::swapEndian(instruction);
		auto lock = vm.lockVM();
		int opcode = (instruction >> 52) & 0xfff;
		if (opcode == OP_NOP) {
			vm.increment();
		} else if (RSet.count(opcode) == 1) {
			int rs, rt, rd;
			Conditions conditions;
			int flags, funct;
			int st, tt, dt;
			decodeRType(instruction, rs, rt, rd, conditions, flags, funct, st, tt, dt);
			RArgs args(vm, vm.registers[rs], vm.registers[rt], vm.registers[rd], conditions, flags, funct, st, tt, dt);
			executeRType(opcode, args);
		} else if (ISet.count(opcode) == 1) {
			int rs, rd;
			Conditions conditions;
			int flags;
			HWord immediate;
			int st, dt, it;
			decodeIType(instruction, rs, rd, conditions, flags, immediate, st, dt, it);
			IArgs args(vm, vm.registers[rs], vm.registers[rd], conditions, flags, immediate, st, dt, it);
			executeIType(opcode, args);
		} else if (JSet.count(opcode) == 1) {
			int rs;
			bool link;
			Conditions conditions;
			int flags;
			HWord address;
			int st;
			decodeJType(instruction, rs, link, conditions, flags, address, st);
			JArgs args(vm, vm.registers[rs], link, conditions, flags, address, st);
			executeJType(opcode, args);
		} else
			throw std::runtime_error("Unknown opcode at " + std::to_string(vm.programCounter) + ": " +
				std::to_string(opcode));
	}

	void executeRType(int opcode, RArgs &args) {
		switch (opcode) {
			case OP_RMATH:
				switch (args.function) {
					case FN_ADD:       addOp(args); return;
					case FN_SUB:       subOp(args); return;
					case FN_MULT:     multOp(args); return;
					case FN_SLL:       sllOp(args); return;
					case FN_SRL:       srlOp(args); return;
					case FN_SRA:       sraOp(args); return;
					case FN_MOD:       modOp(args); return;
					case FN_DIV:       divOp(args); return;
					case FN_SEXT:     sextOp(args); return;
				}
				break;
			case OP_RLOGIC:
				switch (args.function) {
					case FN_AND:     andOp(args); return;
					case FN_NAND:   nandOp(args); return;
					case FN_NOR:     norOp(args); return;
					case FN_NOT:     notOp(args); return;
					case FN_OR:       orOp(args); return;
					case FN_XNOR:   xnorOp(args); return;
					case FN_XOR:     xorOp(args); return;
					case FN_LAND:   landOp(args); return;
					case FN_LNAND: lnandOp(args); return;
					case FN_LNOR:   lnorOp(args); return;
					case FN_LNOT:   lnotOp(args); return;
					case FN_LOR:     lorOp(args); return;
					case FN_LXNOR: lxnorOp(args); return;
					case FN_LXOR:   lxorOp(args); return;
				}
				break;
			case OP_RCOMP:
				switch (args.function) {
					case FN_SL:     slOp(args); return;
					case FN_SLE:   sleOp(args); return;
					case FN_SEQ:   seqOp(args); return;
				}
				break;
			case OP_RJUMP:
				switch (args.function) {
					case FN_JR:     jrOp(args); return;
					case FN_JRC:   jrcOp(args); return;
					case FN_JRL:   jrlOp(args); return;
					case FN_JRLC: jrlcOp(args); return;
				}
				break;
			case OP_RMEM:
				switch (args.function) {
					case FN_C:         cOp(args); return;
					case FN_L:         lOp(args); return;
					case FN_S:         sOp(args); return;
					case FN_SPUSH: spushOp(args); return;
					case FN_SPOP:   spopOp(args); return;
				}
				break;
			case OP_REXT:
				switch (args.function) {
					case FN_PR:       prOp(args); return;
					case FN_HALT:   haltOp(args); return;
					case FN_EVAL:   evalOp(args); return;
					case FN_PRC:     prcOp(args); return;
					case FN_PRD:     prdOp(args); return;
					case FN_PRX:     prxOp(args); return;
					case FN_SLEEP: sleepOp(args); return;
					case FN_PRB:     prbOp(args); return;
					case FN_REST:   restOp(args); return;
					case FN_IO:       ioOp(args); return;
				}
				break;
			case OP_TIME:
				switch (args.function) {
					case FN_TIME:     timeOp(args); return;
					case FN_SVTIME: svtimeOp(args); return;
				}
				break;
			case OP_RING:
				switch (args.function) {
					case FN_RING:     ringOp(args); return;
					case FN_SVRING: svringOp(args); return;
				}
				break;
			case OP_SEL:   selOp(args); return;
			case OP_PAGE:
				switch (args.function) {
					case FN_PGOFF: pgoffOp(args); return;
					case FN_PGON:   pgonOp(args); return;
					case FN_SETPT: setptOp(args); return;
					case FN_SVPG:   svpgOp(args); return;
					case FN_PPUSH: ppushOp(args); return;
					case FN_PPOP:   ppopOp(args); return;
				}
				break;
			case OP_QUERY:
				switch (args.function) {
					case FN_QM: qmOp(args); return;
				}
				break;
			case OP_INTERRUPTS:
				switch (args.function) {
					case FN_DI: diOp(args); return;
					case FN_EI: eiOp(args); return;
				}
				break;
			case OP_TRANS: transOp(args); return;
		}

		throw std::runtime_error("Unknown R-type: " + std::to_string(opcode) + ":" + std::to_string(args.function));
	}

	void executeIType(int opcode, IArgs &args) {
		switch (opcode) {
			case OP_ADDI:     addiOp(args); break;
			case OP_SUBI:     subiOp(args); break;
			case OP_MULTI:   multiOp(args); break;
			case OP_SLLI:     slliOp(args); break;
			case OP_SRLI:     srliOp(args); break;
			case OP_SRAI:     sraiOp(args); break;
			case OP_MODI:     modiOp(args); break;
			case OP_DIVI:     diviOp(args); break;
			case OP_DIVII:   diviiOp(args); break;
			case OP_ANDI:     andiOp(args); break;
			case OP_NANDI:   nandiOp(args); break;
			case OP_NORI:     noriOp(args); break;
			case OP_ORI:       oriOp(args); break;
			case OP_XNORI:   xnoriOp(args); break;
			case OP_XORI:     xoriOp(args); break;
			case OP_LUI:       luiOp(args); break;
			case OP_SLI:       sliOp(args); break;
			case OP_SLEI:     sleiOp(args); break;
			case OP_SEQI:     seqiOp(args); break;
			case OP_SGI:       sgiOp(args); break;
			case OP_SGEI:     sgeiOp(args); break;
			case OP_LI:         liOp(args); break;
			case OP_SI:         siOp(args); break;
			case OP_SET:       setOp(args); break;
			case OP_LNI:       lniOp(args); break;
			case OP_INT:       intOp(args); break;
			case OP_RIT:       ritOp(args); break;
			case OP_TIMEI:   timeiOp(args); break;
			case OP_RINGI:   ringiOp(args); break;
			case OP_CMPI:     cmpiOp(args); break;
			case OP_SSPUSH: sspushOp(args); break;
			case OP_SSPOP:   sspopOp(args); break;
			case OP_SLLII:   slliiOp(args); break;
			case OP_SRLII:   srliiOp(args); break;
			case OP_SRAII:   sraiiOp(args); break;
			default:
				throw std::runtime_error("Unknown I-type: " + std::to_string(opcode));
		}
	}

	void executeJType(int opcode, JArgs &args) {
		switch (opcode) {
			case OP_J:   jOp(args); break;
			case OP_JC: jcOp(args); break;
			default:
				throw std::runtime_error("Unknown J-type: " + std::to_string(opcode));
		}
	}

	void decodeRType(Wasmc::TypedInstruction typed, int &rs, int &rt, int &rd, Conditions &conds, int &flags,
	                 int &funct, int &st, int &tt, int &dt) {
		auto [instruction, types] = typed;
		rd = (instruction >> 31) & 0b1111111;
		rs = (instruction >> 38) & 0b1111111;
		rt = (instruction >> 45) & 0b1111111;
		conds = static_cast<Conditions>((instruction >> 14) & 0b1111);
		flags = (instruction >> 12) & 0b11;
		funct = instruction & 0xfff;
		st = (types >>  8) & 0xff;
		tt = (types >> 16) & 0xff;
		dt = types & 0xff;
	}

	void decodeIType(Wasmc::TypedInstruction typed, int &rs, int &rd,  Conditions &conds, int &flags, HWord &immediate,
	                 int &st, int &dt, int &it) {
		auto [instruction, types] = typed;
		rs = (instruction >> 39) & 0b1111111;
		rd = (instruction >> 32) & 0b1111111;
		conds = static_cast<Conditions>((instruction >> 48) & 0b1111);
		flags = (instruction >> 46) & 0b11;
		immediate = instruction & 0xffffffff;
		st = (types >>  8) & 0xff;
		it = (types >> 16) & 0xff; 
		dt = types & 0xff;
	}

	void decodeJType(Wasmc::TypedInstruction typed, int &rs, bool &link, Conditions &conds, int &flags, HWord &address,
	                 int &st) {
		auto [instruction, types] = typed;
		rs = (instruction >> 45) & 0b1111111;
		link = (instruction >> 44) & 1;
		conds = static_cast<Conditions>((instruction >> 34) & 0b1111);
		flags = (instruction >> 32) & 0b11;
		address = instruction & 0xffffffff;
		st = types & 0xff;
	}

	void setReg(VM &vm, Word &rd, Word value, bool update_flags = true) {
		if (vm.registerID(rd) == Why::zeroOffset)
			std::cerr << "Set register $0 at " << vm.programCounter << "!\n";
		vm.bufferChange<RegisterChange>(vm, vm.registerID(rd), value);
		if (update_flags)
			vm.updateFlags(rd = value);
		else
			rd = value;
		vm.onRegisterChange(vm.registerID(rd));
	}

	void addOp(RArgs &args) {
		setReg(args.vm, args.rd, args.rs + args.rt);
		args.vm.increment();
	}

	void subOp(RArgs &args) {
		setReg(args.vm, args.rd, args.rs - args.rt);
		args.vm.increment();
	}

	void multOp(RArgs &args) {
		// TODO: handle unsigned
		VM &vm = args.vm;
		Word old_hi = vm.hi(), old_lo = vm.lo();
		CAT_MUL128(vm.hi(), vm.lo(), args.rs, args.rt);
		vm.onRegisterChange(Why::loOffset);
		vm.onRegisterChange(Why::hiOffset);
		vm.bufferChange<RegisterChange>(Why::hiOffset, old_hi, vm.hi());
		vm.bufferChange<RegisterChange>(Why::loOffset, old_lo, vm.lo());
		vm.increment();
	}

	void sllOp(RArgs &args) {
		setReg(args.vm, args.rd, args.rs << args.rt);
		args.vm.increment();
	}

	void srlOp(RArgs &args) {
		setReg(args.vm, args.rd, static_cast<UWord>(args.rs) >> static_cast<UWord>(args.rt));
		args.vm.increment();
	}

	void sraOp(RArgs &args) {
		setReg(args.vm, args.rd, args.rs >> args.rt);
		args.vm.increment();
	}

	void modOp(RArgs &args) {
		if (args.rt == 0) {
			args.vm.recordChange<HaltChange>();
			args.vm.stop();
		} else {
			setReg(args.vm, args.rd, args.rs % args.rt);
			args.vm.increment();
		}
		args.vm.increment();
	}

	void divOp(RArgs &args) {
		if (args.rt == 0) {
			args.vm.recordChange<HaltChange>();
			args.vm.stop();
		} else {
			setReg(args.vm, args.rd, args.rs / args.rt);
			args.vm.increment();
		}
		args.vm.increment();
	}

	void sextOp(RArgs &args) {
		// TODO!

		// if ((rs & 0x80000000) != 0)
		// 	setReg(args.vm, args.rd, 0xffffffff00000000ul | rs);
		// else
		// 	setReg(args.vm, args.rd, 0x00000000fffffffful & rs);
		args.vm.increment();
	}

	// void sext16Op(RArgs &args) {
	// 	if ((rs & 0x8000) != 0)
	// 		setReg(args.vm, args.rd, 0xffffffffffff0000ul | rs);
	// 	else
	// 		setReg(args.vm, args.rd, 0x000000000000fffful & rs);
	// 	args.vm.increment();
	// }

	// void sext8Op(RArgs &args) {
	// 	if ((rs & 0x80) != 0)
	// 		setReg(args.vm, args.rd, 0xffffffffffffff00ul | rs);
	// 	else
	// 		setReg(args.vm, args.rd, 0x00000000000000fful & rs);
	// 	args.vm.increment();
	// }

	void andOp(RArgs &args) {
		setReg(args.vm, args.rd, args.rs & args.rt);
		args.vm.increment();
	}

	void nandOp(RArgs &args) {
		setReg(args.vm, args.rd, ~(args.rs & args.rt));
		args.vm.increment();
	}

	void norOp(RArgs &args) {
		setReg(args.vm, args.rd, ~(args.rs | args.rt));
		args.vm.increment();
	}

	void notOp(RArgs &args) {
		setReg(args.vm, args.rd, ~args.rs);
		args.vm.increment();
	}

	void orOp(RArgs &args) {
		setReg(args.vm, args.rd, args.rs | args.rt);
		args.vm.increment();
	}

	void xnorOp(RArgs &args) {
		setReg(args.vm, args.rd, ~(args.rs ^ args.rt));
		args.vm.increment();
	}

	void xorOp(RArgs &args) {
		setReg(args.vm, args.rd, args.rs ^ args.rt);
		args.vm.increment();
	}

	void landOp(RArgs &args) {
		setReg(args.vm, args.rd, args.rs && args.rt);
		args.vm.increment();
	}

	void lnandOp(RArgs &args) {
		setReg(args.vm, args.rd, !(args.rs && args.rt));
		args.vm.increment();
	}

	void lnorOp(RArgs &args) {
		setReg(args.vm, args.rd, !(args.rs || args.rt));
		args.vm.increment();
	}

	void lnotOp(RArgs &args) {
		setReg(args.vm, args.rd, !args.rs);
		args.vm.increment();
	}

	void lorOp(RArgs &args) {
		setReg(args.vm, args.rd, args.rs || args.rt);
		args.vm.increment();
	}

	void lxnorOp(RArgs &args) {
		setReg(args.vm, args.rd, !(!!args.rs ^ !!args.rt));
		args.vm.increment();
	}

	void lxorOp(RArgs &args) {
		setReg(args.vm, args.rd, !!(!!args.rs ^ !!args.rt));
		args.vm.increment();
	}

	void addiOp(IArgs &args) {
		setReg(args.vm, args.rd, args.rs + args.immediate);
		args.vm.increment();
	}

	void subiOp(IArgs &args) {
		setReg(args.vm, args.rd, args.rs - args.immediate);
		args.vm.increment();
	}

	void multiOp(IArgs &args) {
		// TODO: handle unsigned
		VM &vm = args.vm;
		Word old_hi = vm.hi(), old_lo = vm.lo();
		CAT_MUL128(vm.hi(), vm.lo(), args.rs, args.immediate);
		vm.onRegisterChange(Why::hiOffset);
		vm.onRegisterChange(Why::loOffset);
		vm.bufferChange<RegisterChange>(Why::hiOffset, old_hi, vm.hi());
		vm.bufferChange<RegisterChange>(Why::loOffset, old_lo, vm.lo());
		args.vm.increment();
	}

	void slliOp(IArgs &args) {
		setReg(args.vm, args.rd, args.rs << args.immediate);
		args.vm.increment();
	}

	void srliOp(IArgs &args) {
		setReg(args.vm, args.rd, UWord(args.rs) >> UWord(args.immediate));
		args.vm.increment();
	}

	void sraiOp(IArgs &args) {
		setReg(args.vm, args.rd, Word(args.rs) >> Word(args.immediate));
		args.vm.increment();
	}

	void modiOp(IArgs &args) {
		setReg(args.vm, args.rd, args.rs % args.immediate);
		args.vm.increment();
	}

	void diviOp(IArgs &args) {
		setReg(args.vm, args.rd, args.rs / args.immediate);
		args.vm.increment();
	}

	void slliiOp(IArgs &args) {
		setReg(args.vm, args.rd, args.immediate << args.rs);
		args.vm.increment();
	}

	void srliiOp(IArgs &args) {
		setReg(args.vm, args.rd, UWord(args.immediate) >> UWord(args.rs));
		args.vm.increment();
	}

	void sraiiOp(IArgs &args) {
		setReg(args.vm, args.rd, Word(args.immediate) >> Word(args.rs));
		args.vm.increment();
	}

	void diviiOp(IArgs &args) {
		setReg(args.vm, args.rd, args.immediate / args.rs);
		args.vm.increment();
	}

	void andiOp(IArgs &args) {
		setReg(args.vm, args.rd, args.rs & args.immediate);
		args.vm.increment();
	}

	void nandiOp(IArgs &args) {
		setReg(args.vm, args.rd, ~(args.rs & args.immediate));
		args.vm.increment();
	}

	void noriOp(IArgs &args) {
		setReg(args.vm, args.rd, ~(args.rs | args.immediate));
		args.vm.increment();
	}

	void oriOp(IArgs &args) {
		setReg(args.vm, args.rd, args.rs | args.immediate);
		args.vm.increment();
	}

	void xnoriOp(IArgs &args) {
		setReg(args.vm, args.rd, ~(args.rs ^ args.immediate));
		args.vm.increment();
	}

	void xoriOp(IArgs &args) {
		setReg(args.vm, args.rd, args.rs ^ args.immediate);
		args.vm.increment();
	}

	void luiOp(IArgs &args) {
		setReg(args.vm, args.rd, (args.rd & 0xffffffff) | (static_cast<UWord>(args.immediate) << 32));
		args.vm.increment();
	}

	void slOp(RArgs &args) {
		setReg(args.vm, args.rd, args.rs < args.rt, false);
		args.vm.increment();
	}

	void sleOp(RArgs &args) {
		setReg(args.vm, args.rd, args.rs <= args.rt, false);
		args.vm.increment();
	}

	void seqOp(RArgs &args) {
		setReg(args.vm, args.rd, args.rs == args.rt, false);
		args.vm.increment();
	}

	void cmpOp(RArgs &args) {
		args.vm.updateFlags(args.rs - args.rt);
		args.vm.increment();
	}

	void sliOp(IArgs &args) {
		setReg(args.vm, args.rd, args.rs < args.immediate, false);
		args.vm.increment();
	}

	void sleiOp(IArgs &args) {
		setReg(args.vm, args.rd, args.rs <= args.immediate, false);
		args.vm.increment();
	}

	void seqiOp(IArgs &args) {
		setReg(args.vm, args.rd, args.rs == args.immediate, false);
		args.vm.increment();
	}

	void sgiOp(IArgs &args) {
		setReg(args.vm, args.rd, args.rs > args.immediate, false);
		args.vm.increment();
	}

	void sgeiOp(IArgs &args) {
		setReg(args.vm, args.rd, args.rs >= args.immediate, false);
		args.vm.increment();
	}

	void cmpiOp(IArgs &args) {
		args.vm.updateFlags(args.rs - args.immediate);
		args.vm.increment();
	}

	void jOp(JArgs &args) {
		if (args.vm.checkConditions(args.conditions))
			args.vm.jump(args.address, args.link);
		else
			args.vm.increment();
	}

	void jcOp(JArgs &args) {
		if (args.rs != 0)
			args.vm.jump(args.address, args.link);
		else
			args.vm.increment();
	}

	void jrOp(RArgs &args) {
		if (args.vm.checkConditions(args.conditions)) {
			const auto reg_id = args.vm.registerID(args.rd);
			// Reenable interrupts if jumping to $e0.
			if (reg_id == Why::exceptionOffset && args.vm.checkRing(Ring::Zero)) {
				args.vm.hardwareInterruptsEnabled = true;
				args.vm.wakeRest();
			}
			args.vm.jump(args.rd, false, reg_id == Why::returnAddressOffset);
		} else
			args.vm.increment();
	}

	void jrcOp(RArgs &args) {
		if (args.rs) {
			const auto reg_id = args.vm.registerID(args.rd);
			if (reg_id == Why::exceptionOffset && args.vm.checkRing(Ring::Zero))
				args.vm.hardwareInterruptsEnabled = true;
			args.vm.jump(args.rd, false, reg_id == Why::returnAddressOffset);
		} else
			args.vm.increment();
	}

	void jrlOp(RArgs &args) {
		if (args.vm.checkConditions(args.conditions))
			args.vm.jump(args.rd, true);
		else
			args.vm.increment();
	}

	void jrlcOp(RArgs &args) {
		if (args.rs) {
			bool success;
			const Word translated = args.vm.translateAddress(args.rd, &success);
			if (!success)
				args.vm.intPfault();
			else
				args.vm.jump(translated, true);
		} else
			args.vm.increment();
	}

	void cOp(RArgs &args) {
		// TODO!: typechecking and sizes
		bool success;
		const Word translated_source = args.vm.translateAddress(args.rs, &success);
		if (!success) {
			args.vm.intPfault();
		} else {
			const Word value = args.vm.getWord(translated_source);
			const Word translated_destination = args.vm.translateAddress(args.rd, &success);
			if (!success) {
				args.vm.intPfault();
			} else if (args.vm.checkWritable()) {
				args.vm.bufferChange<MemoryChange>(args.vm, translated_destination, value, Size::Word);
				args.vm.setWord(translated_destination, value);
				args.vm.increment();
			} else
				args.vm.intBwrite(translated_destination);
		}
	}

	void lOp(RArgs &args) {
		// TODO!: typechecking and sizes
		bool success;
		const Word translated = args.vm.translateAddress(args.rs, &success);
		if (!success) {
			args.vm.intPfault();
		} else {
			setReg(args.vm, args.rd, args.vm.getWord(translated), false);
			args.vm.increment();
		}
	}

	void sOp(RArgs &args) {
		// TODO!: typechecking and sizes
		bool success;
		const Word translated = args.vm.translateAddress(args.rd, &success);
		if (!success) {
			args.vm.intPfault();
		} else if (args.vm.checkWritable()) {
			args.vm.bufferChange<MemoryChange>(args.vm, translated, args.rs, Size::Word);
			args.vm.setWord(translated, args.rs);
			args.vm.increment();
		} else
			args.vm.intBwrite(translated);
	}

	void spushOp(RArgs &args) {
		VM &vm = args.vm;
		setReg(vm, vm.sp(), vm.sp() - 8, false);
		bool success;
		const Word translated = vm.translateAddress(vm.sp(), &success);
		if (!success) {
			vm.intPfault();
		} else if (vm.checkWritable()) {
			vm.bufferChange<MemoryChange>(vm, translated,args.rs, Size::Word);
			vm.setWord(translated, args.rs);
			vm.increment();
		} else
			vm.intBwrite(translated);
	}

	void spopOp(RArgs &args) {
		bool success;
		const Word translated = args.vm.translateAddress(args.vm.sp(), &success);
		if (!success) {
			args.vm.intPfault();
		} else {
			setReg(args.vm, args.rd, args.vm.getWord(translated), false);
			setReg(args.vm, args.vm.sp(), args.vm.sp() + 8, false);
			args.vm.increment();
		}
	}

	void sspushOp(IArgs &args) {
		VM &vm = args.vm;
		setReg(vm, vm.sp(), vm.sp() - args.immediate, false);
		bool success;
		const Word translated = vm.translateAddress(vm.sp(), &success);

		if (!success) {
			vm.intPfault();
			return;
		}
		
		if (vm.checkWritable()) {
			switch (args.immediate) {
				case 1:
					vm.bufferChange<MemoryChange>(vm, translated, args.rs, Size::Byte);
					vm.setByte(translated, args.rs);
					break;
				case 2:
					vm.bufferChange<MemoryChange>(vm, translated, args.rs, Size::QWord);
					vm.setQuarterword(translated, args.rs);
					break;
				case 4:
					vm.bufferChange<MemoryChange>(vm, translated, args.rs, Size::HWord);
					vm.setHalfword(translated, args.rs);
					break;
				case 8:
					vm.bufferChange<MemoryChange>(vm, translated, args.rs, Size::Word);
					vm.setWord(translated, args.rs);
					break;
				default:
					throw std::runtime_error("Invalid push size: " + std::to_string(args.immediate));
			}

			vm.increment();
			return;
		}

		vm.intBwrite(translated);
	}

	void sspopOp(IArgs &args) {
		VM &vm = args.vm;
		bool success;
		const Word translated = vm.translateAddress(vm.sp(), &success);
		if (!success) {
			vm.intPfault();
			return;
		}

		switch (args.immediate) {
			case 1:
				setReg(vm, args.rd, vm.getByte(translated), false);
				break;
			case 2:
				setReg(vm, args.rd, vm.getQuarterword(translated), false);
				break;
			case 4:
				setReg(vm, args.rd, vm.getHalfword(translated), false);
				break;
			case 8:
				setReg(vm, args.rd, vm.getWord(translated), false);
				break;
			default:
				throw std::runtime_error("Invalid pop size: " + std::to_string(args.immediate));
		}

		setReg(vm, vm.sp(), vm.sp() + args.immediate, false);
		vm.increment();
	}

	void msOp(RArgs &args) {
		// This is awfully slow.
		bool success;
		for (Word i = 0; i < args.rs; ++i) {
			const Word translated = args.vm.translateAddress(args.rd + i, &success);
			if (!success) {
				args.vm.intPfault();
				return;
			} else if (args.vm.checkWritable()) {
				args.vm.bufferChange<MemoryChange>(args.vm, translated, args.rt & 0xff, Size::Byte);
				args.vm.setByte(translated, args.rt & 0xff);
			} else {
				args.vm.intBwrite(translated);
				return;
			}
		}

		args.vm.increment();
	}

	void transOp(RArgs &args) {
		VM &vm = args.vm;
		if (vm.pagingOn) {
			bool success;
			const Word translated = vm.translateAddress(args.rs, &success);
			if (!success) {
				vm.intPfault();
			} else {
				setReg(vm, args.rd, translated, false);
				vm.increment();
			}
		} else {
			setReg(vm, args.rd, args.rs, false);
			vm.increment();
		}
	}

	void liOp(IArgs &args) {
		// TODO!: typechecking and sizes
		VM &vm = args.vm;
		bool success;
		const Word translated = vm.translateAddress(args.immediate, &success);
		if (!success) {
			vm.intPfault();
		} else {
			setReg(args.vm, args.rd, vm.getWord(translated), false);
			args.vm.increment();
		}
	}

	void siOp(IArgs &args) {
		// TODO!: typechecking and sizes
		VM &vm = args.vm;
		bool success;
		const Word translated = vm.translateAddress(args.immediate, &success);
		if (!success) {
			vm.intPfault();
		} else if (vm.checkWritable()) {
			vm.bufferChange<MemoryChange>(vm, translated, args.rs, Size::Word);
			vm.setWord(translated, args.rs);
			args.vm.increment();
		} else
			vm.intBwrite(translated);
	}

	void setOp(IArgs &args) {
		setReg(args.vm, args.rd, args.immediate, false);
		args.vm.increment();
	}

	void lniOp(IArgs &args) {
		// TODO!: typechecking and sizes
		VM &vm = args.vm;
		bool success;
		const Word translated = vm.translateAddress(args.immediate, &success);
		if (!success) {
			vm.intPfault();
		} else {
			const Word value = vm.getWord(translated);
			vm.bufferChange<MemoryChange>(vm, args.rd, value, Size::Word);
			vm.setWord(args.rd, value);
			args.vm.increment();
		}
	}

	void intOp(IArgs &args) {
		args.vm.interrupt(args.immediate, false);
	}

	void ritOp(IArgs &args) {
		VM &vm = args.vm;
		if (vm.checkRing(Ring::Zero)) {
			bool success;
			const Word translated = vm.translateAddress(args.immediate, &success);
			if (!success) {
				vm.intPfault();
			} else {
				vm.bufferChange<InterruptTableChange>(vm, translated);
				vm.interruptTableAddress = translated;
				vm.onInterruptTableChange();
				vm.increment();
			}
		} else
			vm.intProtec();
	}

	void timeOp(RArgs &args) {
		if (args.vm.checkRing(Ring::Zero)) {
			args.vm.setTimer(static_cast<UWord>(args.rs));
			args.vm.increment();
		} else
			args.vm.intProtec();
	}

	void svtimeOp(RArgs &args) {
		if (args.vm.checkRing(Ring::Zero)) {
			setReg(args.vm, args.rd, args.vm.timerTicks);
			args.vm.increment();
		} else
			args.vm.intProtec();
	}

	void timeiOp(IArgs &args) {
		if (args.vm.checkRing(Ring::Zero)) {
			args.vm.setTimer(static_cast<UWord>(args.immediate));
			args.vm.increment();
		} else
			args.vm.intProtec();
	}

	void ringOp(RArgs &args) {
		if (args.vm.changeRing(static_cast<Ring>(args.rs)))
			args.vm.increment();
	}

	void svringOp(RArgs &args) {
		setReg(args.vm, args.rd, Word(args.vm.ring), false);
		args.vm.increment();
	}

	void ringiOp(IArgs &args) {
		if (args.vm.changeRing(static_cast<Ring>(args.immediate)))
			args.vm.increment();
	}

	void prOp(RArgs &args) {
		VM &vm = args.vm;
		auto &rs = args.rs;
		std::stringstream ss;
		if (rs < 0) {
			ss << Why::coloredRegister(vm.registerID(rs)) << ": " << rs << " / " << UWord(rs) << " / " << std::hex
			   << rs;
		} else {
			ss << Why::coloredRegister(vm.registerID(rs)) << ": " // << "0x" << std::hex << rs << " \e[2m/\e[22m " << std::dec
			<< rs << " / " << std::hex << rs;
		}
		// vm.onPrint(ss.str() + "\n");
		// DBG(ss.str());
		std::cerr << ss.str() << '\n';
		args.vm.increment();
	}

	void haltOp(RArgs &args) {
		args.vm.recordChange<HaltChange>();
		args.vm.stop();
	}

	void evalOp(RArgs &args) {
		warn() << "<eval> not implemented\n";
		args.vm.increment();
	}

	void prcOp(RArgs &args) {
		args.vm.onPrint(std::string(1, static_cast<char>(args.rs)));
		args.vm.increment();
	}

	void prdOp(RArgs &args) {
		args.vm.onPrint(std::to_string(args.rs));
		args.vm.increment();
	}

	void prxOp(RArgs &args) {
		std::stringstream ss;
		ss << std::hex << args.rs;
		args.vm.onPrint(ss.str());
		args.vm.increment();
	}

	void sleepOp(RArgs &args) {
		usleep(args.rs);
		args.vm.increment();
	}

	void prbOp(RArgs &args) {
		std::stringstream ss;
		ss << std::bitset<64>(args.rs);
		args.vm.onPrint(ss.str());
		args.vm.increment();
	}

	void restOp(RArgs &args) {
		args.vm.increment();
		args.vm.rest();
	}

	void ioOp(RArgs &args) {
		VM &vm = args.vm;
		if (vm.checkRing(Ring::Two)) {
			const Word &a0 = vm.registers[Why::argumentOffset],
			           &a1 = vm.registers[Why::argumentOffset + 1],
			           &a2 = vm.registers[Why::argumentOffset + 2],
			           &a3 = vm.registers[Why::argumentOffset + 3];
			Word &r0 = vm.registers[Why::returnValueOffset], &e0 = vm.registers[Why::exceptionOffset];

			const size_t device_id = size_t(a1);
			const bool valid_id = device_id < vm.drives.size();

			switch (a0) {
				case IO_DEVCOUNT:
					setReg(vm, e0, 0, false);
					setReg(vm, r0, vm.drives.size(), false);
					break;

				case IO_SEEKABS:
					if (!valid_id)
						setReg(vm, e0, 1, false);
					else if (lseek(vm.drives.at(device_id).fd, a2, SEEK_SET) == -1)
						setReg(vm, e0, 2, false);
					else
						setReg(vm, e0, 0, false);
					break;

				case IO_SEEKREL:
					if (!valid_id) {
						setReg(vm, e0, 1, false);
					} else {
						const ssize_t result = lseek(vm.drives.at(device_id).fd, a2, SEEK_CUR);
						if (result == -1) {
							setReg(vm, e0, 2, false);
						} else {
							setReg(vm, e0, 0, false);
							setReg(vm, r0, result, false);
						}
					}

					break;

				case IO_READ: { // TODO: verify. This code is suspicious.
					if (!valid_id) {
						setReg(vm, e0, 1, false);
					} else {
						size_t address = a2, remaining = a3, total_bytes_read = 0;
						const int fd = vm.drives.at(device_id).fd;
						const size_t memsize = vm.getMemorySize();
						setReg(vm, e0, 0, false);

						while (0 < remaining) {
							const size_t mod = address % VM::PAGE_SIZE; // Especially this.

							bool translate_success;
							const size_t translated = size_t(vm.translateAddress(address, &translate_success));
							if (!translate_success) {
								setReg(vm, e0, 2, false);
								vm.intPfault();
								return;
							}

							if (!vm.checkWritable()) {
								setReg(vm, e0, 3, false);
								vm.intBwrite(translated);
								return;
							}

							if (memsize <= translated) {
								break;
							} else {
								const size_t diff = memsize - translated;
								if (diff < remaining)
									remaining = diff;
							}

							const size_t to_read = std::min(mod? mod : VM::PAGE_SIZE, remaining); // And this.
							const ssize_t bytes_read = ::read(fd, &vm.memory[translated], to_read);

							if (bytes_read < 0)
								setReg(vm, e0, errno + 3, false);
							if (bytes_read <= 0)
								break;
							remaining -= size_t(bytes_read);
							address += size_t(bytes_read);
							total_bytes_read += size_t(bytes_read);
						}

						setReg(vm, r0, total_bytes_read, false);
					}

					break;
				}

				case IO_WRITE: { // TODO: verify this too.
					if (!valid_id) {
						setReg(vm, e0, 1, false);
					} else {
						size_t address = a2, remaining = a3, total_bytes_written = 0;
						const int fd = vm.drives.at(device_id).fd;
						const size_t memsize = vm.getMemorySize();
						setReg(vm, e0, 0, false);

						while (0 < remaining) {
							const size_t mod = address % VM::PAGE_SIZE;

							bool translate_success;
							const size_t translated = size_t(vm.translateAddress(address, &translate_success));
							if (!translate_success) {
								setReg(vm, e0, 2, false);
								vm.intPfault();
								return;
							}

							if (memsize <= translated) {
								break;
							} else {
								const size_t diff = memsize - translated;
								if (diff < remaining)
									remaining = diff;
							}

							const size_t to_write = std::min(mod? mod : VM::PAGE_SIZE, remaining); // And this.
							const ssize_t bytes_written = ::write(fd, &vm.memory[translated], to_write);

							if (bytes_written < 0)
								setReg(vm, e0, errno + 2, false);
							if (bytes_written <= 0)
								break;
							remaining -= size_t(bytes_written);
							address += size_t(bytes_written);
							total_bytes_written += size_t(bytes_written);
						}

						setReg(vm, r0, total_bytes_written, false);
					}

					break;
				}

				case IO_GETSIZE: {
					if (!valid_id) {
						setReg(vm, e0, 1, false);
					} else {
						const int fd = vm.drives.at(device_id).fd;
						const off_t old_cursor = lseek(fd, 0, SEEK_CUR);
						if (old_cursor == -1) {
							setReg(vm, e0, errno + 1, false);
							break;
						}

						const off_t end_cursor = lseek(fd, 0, SEEK_END);
						if (end_cursor == -1) {
							setReg(vm, e0, errno + 1, false);
							break;
						}

						setReg(vm, r0, Word(end_cursor), false);
	
						const off_t result = lseek(fd, old_cursor, SEEK_SET);
						setReg(vm, e0, result == -1? errno + 1 : 0, false);
					}

					break;
				}

				case IO_GETCURSOR: {
					if (!valid_id) {
						setReg(vm, e0, 1, false);
					} else {
						const off_t cursor = lseek(vm.drives.at(device_id).fd, 0, SEEK_CUR);
						setReg(vm, e0, cursor == -1? errno + 1 : 0, false);
						if (cursor != -1)
							setReg(vm, r0, cursor, false);
					}
					break;
				}

				case IO_GETNAME: {
					if (!valid_id) {
						setReg(vm, e0, 1, false);
					} else {
						const std::string &name = vm.drives.at(a1).name;
						const char *c_str = name.c_str();
						size_t address = a2, remaining = std::min(size_t(a2), name.size() + 1), total_bytes_read = 0;
						const size_t memsize = vm.getMemorySize();
						setReg(vm, e0, 0, false);

						while (0 < remaining) {
							const size_t mod = address % VM::PAGE_SIZE;

							bool translate_success;
							const size_t translated = size_t(vm.translateAddress(address, &translate_success));
							if (!translate_success) {
								setReg(vm, e0, 2, false);
								vm.intPfault();
								return;
							}

							if (!vm.checkWritable()) {
								setReg(vm, e0, 3, false);
								vm.intBwrite(translated);
								return;
							}

							if (memsize <= translated) {
								break;
							} else {
								const size_t diff = memsize - translated;
								if (diff < remaining)
									remaining = diff;
							}

							const size_t to_read = std::min(mod? mod : VM::PAGE_SIZE, remaining);
							std::memcpy(&vm.memory[translated], c_str + total_bytes_read, to_read);

							remaining -= to_read;
							address += to_read;
							total_bytes_read += to_read;
						}

						// Don't include the null terminator in the byte count
						setReg(vm, r0, 0 < total_bytes_read? total_bytes_read - 1 : 0, false);
					}

					break;
				}

				default:
					setReg(vm, e0, 666, false);
			}

			args.vm.increment();
		} else
			vm.intProtec();
	}

	void selOp(RArgs &args) {
		setReg(args.vm, args.rd, args.vm.checkConditions(args.conditions)? args.rs : args.rt, false);
		args.vm.increment();
	}

	void pgoffOp(RArgs &args) {
		VM &vm = args.vm;
		if (vm.checkRing(Ring::Zero)) {
			vm.bufferChange<PagingChange>(vm.pagingOn, false);
			vm.pagingOn = false;
			std::cerr << "Paging disabled (PC: " << vm.programCounter << ").\n";
			vm.onPagingChange(false);
			vm.increment();
		} else
			vm.intProtec();
	}

	void pgonOp(RArgs &args) {
		VM &vm = args.vm;
		if (vm.checkRing(Ring::Zero)) {
			vm.bufferChange<PagingChange>(vm.pagingOn, true);
			vm.pagingOn = true;
			std::cerr << "Paging enabled (PC: " << vm.programCounter << ").\n";
			vm.onPagingChange(true);
			args.vm.increment();
		} else
			vm.intProtec();
	}

	void setptOp(RArgs &args) {
		VM &vm = args.vm;
		if (vm.checkRing(Ring::Zero)) {
			vm.bufferChange<P0Change>(vm.p0, args.rs);
			vm.p0 = args.rs;
			std::cerr << "Page table address set to " << vm.p0 << " (PC: " << vm.programCounter << ").\n";
			vm.onP0Change(args.rs);
			if (args.rt != 0) {
				const auto reg_id = vm.registerID(args.rt);
				// Reenable interrupts if jumping to $e0.
				if (reg_id == Why::exceptionOffset && vm.checkRing(Ring::Zero)) {
					vm.hardwareInterruptsEnabled = true;
					vm.wakeRest();
				}
				vm.jump(args.rt, false);
			} else
				args.vm.increment();
		} else
			vm.intProtec();
	}

	void svpgOp(RArgs &args) {
		setReg(args.vm, args.rd, args.vm.pagingOn? 1 : 0, false);
		args.vm.increment();
	}

	void ppushOp(RArgs &args) {
		if (args.vm.checkRing(Ring::Zero)) {
			args.vm.pagingStack.emplace_back(args.vm);
			args.vm.increment();
		} else
			args.vm.intProtec();
	}

	void ppopOp(RArgs &args) {
		VM &vm = args.vm;
		if (vm.checkRing(Ring::Zero)) {
			if (!vm.pagingStack.empty()) {
				const auto &back = vm.pagingStack.back();
				vm.bufferChange<PagingChange>(vm.pagingOn, back.enabled);
				vm.bufferChange<P0Change>(vm.p0, back.p0);
				vm.pagingOn = back.enabled;
				vm.p0 = back.p0;
				vm.pagingStack.pop_back();
			}
			if (args.rs != 0) {
				const auto reg_id = vm.registerID(args.rs);
				// Reenable interrupts if jumping to $e0.
				if (reg_id == Why::exceptionOffset && vm.checkRing(Ring::Zero)) {
					vm.hardwareInterruptsEnabled = true;
					vm.wakeRest();
				}
				vm.jump(args.rs, false);
			} else
				args.vm.increment();
		} else
			vm.intProtec();
	}

	void qmOp(RArgs &args) {
		setReg(args.vm, args.rd, args.vm.getMemorySize(), false);
		args.vm.increment();
	}

	void diOp(RArgs &args) {
		if (args.vm.checkRing(Ring::Zero)) {
			args.vm.hardwareInterruptsEnabled = false;
			args.vm.increment();
		} else
			args.vm.intProtec();
	}

	void eiOp(RArgs &args) {
		if (args.vm.checkRing(Ring::Zero)) {
			args.vm.hardwareInterruptsEnabled = true;
			args.vm.increment();
		} else
			args.vm.intProtec();
	}
}
