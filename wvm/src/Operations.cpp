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
		OP_ADDI, OP_SUBI, OP_MULTI, OP_MULTUI, OP_SLLI, OP_SRLI, OP_SRAI, OP_MODI, OP_DIVI, OP_DIVUI, OP_DIVII,
		OP_DIVUII, OP_ANDI, OP_NANDI, OP_NORI, OP_ORI, OP_XNORI, OP_XORI, OP_LUI, OP_SLI, OP_SLEI, OP_CMPI, OP_SEQI,
		OP_SLUI, OP_SLEUI, OP_SGI, OP_SGEI, OP_LI, OP_SI, OP_SET, OP_LBI, OP_SBI, OP_LNI, OP_LBNI, OP_INT, OP_RIT,
		OP_TIMEI, OP_RINGI, OP_SSPUSH, OP_SSPOP, OP_SGEUI, OP_SGUI, OP_MODUI, OP_SLLII, OP_SRLII, OP_SRAII,
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
					case FN_MULTU:   multuOp(args); return;
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
					case FN_SLU:   sluOp(args); return;
					case FN_SLEU: sleuOp(args); return;
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
					case FN_CB:       cbOp(args); return;
					case FN_LB:       lbOp(args); return;
					case FN_SB:       sbOp(args); return;
					case FN_SPUSH: spushOp(args); return;
					case FN_SPOP:   spopOp(args); return;
					case FN_CH:       chOp(args); return;
					case FN_LH:       lhOp(args); return;
					case FN_SH:       shOp(args); return;
					case FN_MS:       msOp(args); return;
					case FN_CS:       csOp(args); return;
					case FN_LS:       lsOp(args); return;
					case FN_SS:       ssOp(args); return;
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
			case OP_LBI:       lbiOp(args); break;
			case OP_SBI:       sbiOp(args); break;
			case OP_LNI:       lniOp(args); break;
			case OP_LBNI:     lbniOp(args); break;
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
	}

	void decodeJType(Wasmc::TypedInstruction typed, int &rs, bool &link, Conditions &conds, int &flags,
	                 HWord &address) {
		auto [instruction, types] = typed;
		rs = (instruction >> 45) & 0b1111111;
		link = (instruction >> 44) & 1;
		conds = static_cast<Conditions>((instruction >> 34) & 0b1111);
		flags = (instruction >> 32) & 0b11;
		address = instruction & 0xffffffff;
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

	void addOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, rs + rt);
		vm.increment();
	}

	void subOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, rs - rt);
		vm.increment();
	}

	void multOp(VM &vm, Word &rs, Word &rt, Word &, Conditions, int) {
		Word old_hi = vm.hi(), old_lo = vm.lo();
		CAT_MUL128(vm.hi(), vm.lo(), rs, rt);
		vm.onRegisterChange(Why::loOffset);
		vm.onRegisterChange(Why::hiOffset);
		vm.bufferChange<RegisterChange>(Why::hiOffset, old_hi, vm.hi());
		vm.bufferChange<RegisterChange>(Why::loOffset, old_lo, vm.lo());
		vm.increment();
	}

	void multuOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {
		// TODO: mult and multu shouldn't be the same.
		multOp(vm, rs, rt, rd, conditions, flags);
	}

	void sllOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, rs << rt);
		vm.increment();
	}

	void srlOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, static_cast<UWord>(rs) >> static_cast<UWord>(rt));
		vm.increment();
	}

	void sraOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, rs >> rt);
		vm.increment();
	}

	void modOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, rs % rt);
		vm.increment();
	}

	void divOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, rs / rt);
		vm.increment();
	}

	void divuOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, static_cast<UWord>(rs) / static_cast<UWord>(rt));
		vm.increment();
	}

	void moduOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		if (rt == 0) {
			vm.recordChange<HaltChange>();
			vm.stop();
		} else {
			setReg(vm, rd, static_cast<UWord>(rs) % static_cast<UWord>(rt));
			vm.increment();
		}
	}

	void sext32Op(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		if ((rs & 0x80000000) != 0)
			setReg(vm, rd, 0xffffffff00000000ul | rs);
		else
			setReg(vm, rd, 0x00000000fffffffful & rs);
		vm.increment();
	}

	void sext16Op(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		if ((rs & 0x8000) != 0)
			setReg(vm, rd, 0xffffffffffff0000ul | rs);
		else
			setReg(vm, rd, 0x000000000000fffful & rs);
		vm.increment();
	}

	void sext8Op(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		if ((rs & 0x80) != 0)
			setReg(vm, rd, 0xffffffffffffff00ul | rs);
		else
			setReg(vm, rd, 0x00000000000000fful & rs);
		vm.increment();
	}

	void andOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, rs & rt);
		vm.increment();
	}

	void nandOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, ~(rs & rt));
		vm.increment();
	}

	void norOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, ~(rs | rt));
		vm.increment();
	}

	void notOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		setReg(vm, rd, ~rs);
		vm.increment();
	}

	void orOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, rs | rt);
		vm.increment();
	}

	void xnorOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, ~(rs ^ rt));
		vm.increment();
	}

	void xorOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, rs ^ rt);
		vm.increment();
	}

	void landOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, rs && rt);
		vm.increment();
	}

	void lnandOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, !(rs && rt));
		vm.increment();
	}

	void lnorOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, !(rs || rt));
		vm.increment();
	}

	void lnotOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		setReg(vm, rd, !rs);
		vm.increment();
	}

	void lorOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, rs || rt);
		vm.increment();
	}

	void lxnorOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, !(!!rs ^ !!rt));
		vm.increment();
	}

	void lxorOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, !!(!!rs ^ !!rt));
		vm.increment();
	}

	void addiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, rs + immediate);
		vm.increment();
	}

	void subiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, rs - immediate);
		vm.increment();
	}

	void multiOp(VM &vm, Word &rs, Word &, Conditions, int, HWord immediate) {
		Word old_hi = vm.hi(), old_lo = vm.lo();
		CAT_MUL128(vm.hi(), vm.lo(), rs, immediate);
		vm.onRegisterChange(Why::hiOffset);
		vm.onRegisterChange(Why::loOffset);
		vm.bufferChange<RegisterChange>(Why::hiOffset, old_hi, vm.hi());
		vm.bufferChange<RegisterChange>(Why::loOffset, old_lo, vm.lo());
		vm.increment();
	}

	void multuiOp(VM &vm, Word &rs, Word &, Conditions, int, HWord immediate) {
		Word old_hi = vm.hi(), old_lo = vm.lo();
		CAT_MUL128(vm.hi(), vm.lo(), rs, static_cast<UWord>(immediate));
		vm.onRegisterChange(Why::hiOffset);
		vm.onRegisterChange(Why::loOffset);
		vm.bufferChange<RegisterChange>(Why::hiOffset, old_hi, vm.hi());
		vm.bufferChange<RegisterChange>(Why::loOffset, old_lo, vm.lo());
		vm.increment();
	}

	void slliOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, rs << immediate);
		vm.increment();
	}

	void srliOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, UWord(rs) >> UWord(immediate));
		vm.increment();
	}

	void sraiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, Word(rs) >> Word(immediate));
		vm.increment();
	}

	void modiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, rs % immediate);
		vm.increment();
	}

	void diviOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, rs / immediate);
		vm.increment();
	}

	void divuiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, static_cast<UWord>(rs) / static_cast<UWord>(immediate));
		vm.increment();
	}

	void moduiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, static_cast<UWord>(rs) % static_cast<UWord>(immediate));
		vm.increment();
	}

	void slliiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, immediate << rs);
		vm.increment();
	}

	void srliiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, UWord(immediate) >> UWord(rs));
		vm.increment();
	}

	void sraiiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, Word(immediate) >> Word(rs));
		vm.increment();
	}

	void diviiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, immediate / rs);
		vm.increment();
	}

	void divuiiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, static_cast<UWord>(immediate) / static_cast<UWord>(rs));
		vm.increment();
	}

	void andiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, rs & immediate);
		vm.increment();
	}

	void nandiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, ~(rs & immediate));
		vm.increment();
	}

	void noriOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, ~(rs | immediate));
		vm.increment();
	}

	void oriOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, rs | immediate);
		vm.increment();
	}

	void xnoriOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, ~(rs ^ immediate));
		vm.increment();
	}

	void xoriOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, rs ^ immediate);
		vm.increment();
	}

	void luiOp(VM &vm, Word &, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, (rd & 0xffffffff) | (static_cast<UWord>(immediate) << 32));
		vm.increment();
	}

	void slOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, rs < rt, false);
		vm.increment();
	}

	void sleOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, rs <= rt, false);
		vm.increment();
	}

	void seqOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, rs == rt, false);
		vm.increment();
	}

	void sluOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, static_cast<UWord>(rs) < static_cast<UWord>(rt), false);
		vm.increment();
	}

	void sleuOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, static_cast<UWord>(rs) <= static_cast<UWord>(rt), false);
		vm.increment();
	}

	void cmpOp(VM &vm, Word &rs, Word &rt, Word &, Conditions, int) {
		vm.updateFlags(rs - rt);
		vm.increment();
	}

	void sliOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, rs < immediate, false);
		vm.increment();
	}

	void sleiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, rs <= immediate, false);
		vm.increment();
	}

	void seqiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, rs == immediate, false);
		vm.increment();
	}

	void sluiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, static_cast<UWord>(rs) < static_cast<UWord>(immediate), false);
		vm.increment();
	}

	void sleuiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, static_cast<UWord>(rs) <= static_cast<UWord>(immediate), false);
		vm.increment();
	}

	void sgiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, rs > immediate, false);
		vm.increment();
	}

	void sgeiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, rs >= immediate, false);
		vm.increment();
	}

	void sgeuiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, static_cast<UWord>(rs) >= static_cast<UWord>(immediate), false);
		vm.increment();
	}

	void sguiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, static_cast<UWord>(rs) > static_cast<UWord>(immediate), false);
		vm.increment();
	}

	void cmpiOp(VM &vm, Word &rs, Word &, Conditions, int, HWord immediate) {
		vm.updateFlags(rs - immediate);
		vm.increment();
	}

	void jOp(VM &vm, Word &, bool link, Conditions conditions, int, HWord address) {
		if (vm.checkConditions(conditions))
			vm.jump(address, link);
		else
			vm.increment();
	}

	void jcOp(VM &vm, Word &rs, bool link, Conditions, int, HWord address) {
		if (rs != 0)
			vm.jump(address, link);
		else
			vm.increment();
	}

	void jrOp(VM &vm, Word &, Word &, Word &rd, Conditions conditions, int) {
		if (vm.checkConditions(conditions)) {
			const auto reg_id = vm.registerID(rd);
			// Reenable interrupts if jumping to $e0.
			if (reg_id == Why::exceptionOffset && vm.checkRing(Ring::Zero)) {
				vm.hardwareInterruptsEnabled = true;
				vm.wakeRest();
			}
			vm.jump(rd, false, reg_id == Why::returnAddressOffset);
		} else
			vm.increment();
	}

	void jrcOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		if (rs) {
			const auto reg_id = vm.registerID(rd);
			if (reg_id == Why::exceptionOffset && vm.checkRing(Ring::Zero))
				vm.hardwareInterruptsEnabled = true;
			vm.jump(rd, false, reg_id == Why::returnAddressOffset);
		} else
			vm.increment();
	}

	void jrlOp(VM &vm, Word &, Word &, Word &rd, Conditions conditions, int) {
		if (vm.checkConditions(conditions))
			vm.jump(rd, true);
		else
			vm.increment();
	}

	void jrlcOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		if (rs) {
			bool success;
			const Word translated = vm.translateAddress(rd, &success);
			if (!success)
				vm.intPfault();
			else
				vm.jump(translated, true);
		} else
			vm.increment();
	}

	void cOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		bool success;
		const Word translated_source = vm.translateAddress(rs, &success);
		if (!success) {
			vm.intPfault();
		} else {
			const Word value = vm.getWord(translated_source);
			const Word translated_destination = vm.translateAddress(rd, &success);
			if (!success) {
				vm.intPfault();
			} else if (vm.checkWritable()) {
				vm.bufferChange<MemoryChange>(vm, translated_destination, value, Size::Word);
				vm.setWord(translated_destination, value);
				vm.increment();
			} else
				vm.intBwrite(translated_destination);
		}
	}

	void lOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		bool success;
		const Word translated = vm.translateAddress(rs, &success);
		if (!success) {
			vm.intPfault();
		} else {
			setReg(vm, rd, vm.getWord(translated), false);
			vm.increment();
		}
	}

	void sOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		bool success;
		const Word translated = vm.translateAddress(rd, &success);
		if (!success) {
			vm.intPfault();
		} else if (vm.checkWritable()) {
			vm.bufferChange<MemoryChange>(vm, translated, rs, Size::Word);
			vm.setWord(translated, rs);
			vm.increment();
		} else
			vm.intBwrite(translated);
	}

	void cbOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		bool success;
		const Word translated_source = vm.translateAddress(rs, &success);
		if (!success) {
			vm.intPfault();
		} else {
			const Byte value = vm.getByte(translated_source);
			const Word translated_destination = vm.translateAddress(rd, &success);
			if (!success) {
				vm.intPfault();
			} else if (vm.checkWritable()) {
				vm.bufferChange<MemoryChange>(vm, translated_destination, value, Size::Byte);
				vm.setByte(translated_destination, value);
				vm.increment();
			} else
				vm.intBwrite(translated_destination);
		}
	}

	void lbOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		bool success;
		const Word translated = vm.translateAddress(rs, &success);
		if (!success) {
			vm.intPfault();
		} else {
			setReg(vm, rd, vm.getByte(translated), false);
			vm.increment();
		}
	}

	void sbOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		bool success;
		const Word translated = vm.translateAddress(rd, &success);
		if (!success) {
			vm.intPfault();
		} else if (vm.checkWritable()) {
			vm.bufferChange<MemoryChange>(vm, translated, rs, Size::Byte);
			vm.setByte(translated, rs);
			vm.increment();
		} else
			vm.intBwrite(translated);
	}

	void spushOp(VM &vm, Word &rs, Word &, Word &, Conditions, int) {
		setReg(vm, vm.sp(), vm.sp() - 8, false);
		bool success;
		const Word translated = vm.translateAddress(vm.sp(), &success);
		if (!success) {
			vm.intPfault();
		} else if (vm.checkWritable()) {
			vm.bufferChange<MemoryChange>(vm, translated, rs, Size::Word);
			vm.setWord(translated, rs);
			vm.increment();
		} else
			vm.intBwrite(translated);
	}

	void spopOp(VM &vm, Word &, Word &, Word &rd, Conditions, int) {
		bool success;
		const Word translated = vm.translateAddress(vm.sp(), &success);
		if (!success) {
			vm.intPfault();
		} else {
			setReg(vm, rd, vm.getWord(translated), false);
			setReg(vm, vm.sp(), vm.sp() + 8, false);
			vm.increment();
		}
	}

	void sspushOp(VM &vm, Word &rs, Word &, Conditions, int, HWord immediate) {
		setReg(vm, vm.sp(), vm.sp() - immediate, false);
		bool success;
		const Word translated = vm.translateAddress(vm.sp(), &success);
		if (!success) {
			vm.intPfault();
		} else if (vm.checkWritable()) {
			switch (immediate) {
				case 1:
					vm.bufferChange<MemoryChange>(vm, translated, rs, Size::Byte);
					vm.setByte(translated, rs);
					break;
				case 2:
					vm.bufferChange<MemoryChange>(vm, translated, rs, Size::QWord);
					vm.setQuarterword(translated, rs);
					break;
				case 4:
					vm.bufferChange<MemoryChange>(vm, translated, rs, Size::HWord);
					vm.setHalfword(translated, rs);
					break;
				case 8:
					vm.bufferChange<MemoryChange>(vm, translated, rs, Size::Word);
					vm.setWord(translated, rs);
					break;
				default:
					throw std::runtime_error("Invalid push size: " + std::to_string(immediate));
			}

			vm.increment();
		} else
			vm.intBwrite(translated);
	}

	void sspopOp(VM &vm, Word &, Word &rd, Conditions, int, HWord immediate) {
		bool success;
		const Word translated = vm.translateAddress(vm.sp(), &success);
		if (!success) {
			vm.intPfault();
		} else {
			switch (immediate) {
				case 1:
					setReg(vm, rd, vm.getByte(translated), false);
					break;
				case 2:
					setReg(vm, rd, vm.getQuarterword(translated), false);
					break;
				case 4:
					setReg(vm, rd, vm.getHalfword(translated), false);
					break;
				case 8:
					setReg(vm, rd, vm.getWord(translated), false);
					break;
				default:
					throw std::runtime_error("Invalid pop size: " + std::to_string(immediate));
			}

			setReg(vm, vm.sp(), vm.sp() + immediate, false);
			vm.increment();
		}
	}

	void chOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		bool success;
		const Word translated_source = vm.translateAddress(rs, &success);
		if (!success) {
			vm.intPfault();
		} else {
			const HWord value = vm.getHalfword(translated_source);
			const Word translated_destination = vm.translateAddress(rd, &success);
			if (!success) {
				vm.intPfault();
			} else if (vm.checkWritable()) {
				vm.bufferChange<MemoryChange>(vm, translated_destination, value, Size::HWord);
				vm.setHalfword(translated_destination, value);
				vm.increment();
			} else
				vm.intBwrite(translated_destination);
		}
	}

	void lhOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		bool success;
		const Word translated = vm.translateAddress(rs, &success);
		if (!success) {
			vm.intPfault();
		} else {
			setReg(vm, rd, vm.getHalfword(translated), false);
			vm.increment();
		}
	}

	void shOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		bool success;
		const Word translated = vm.translateAddress(rd, &success);
		if (!success) {
			vm.intPfault();
		} else if (vm.checkWritable()) {
			vm.bufferChange<MemoryChange>(vm, translated, rs, Size::HWord);
			vm.setHalfword(translated, rs);
			vm.increment();
		} else
			vm.intBwrite(translated);
	}

	void msOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		// This is awfully slow.
		bool success;
		for (Word i = 0; i < rs; ++i) {
			const Word translated = vm.translateAddress(rd + i, &success);
			if (!success) {
				vm.intPfault();
				return;
			} else if (vm.checkWritable()) {
				vm.bufferChange<MemoryChange>(vm, translated, rt & 0xff, Size::Byte);
				vm.setByte(translated, rt & 0xff);
			} else {
				vm.intBwrite(translated);
				return;
			}
		}

		vm.increment();
	}

	void csOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		bool success;
		const Word translated_source = vm.translateAddress(rs, &success);
		if (!success) {
			vm.intPfault();
		} else {
			const QWord value = vm.getQuarterword(translated_source);
			const Word translated_destination = vm.translateAddress(rd, &success);
			if (!success) {
				vm.intPfault();
			} else if (vm.checkWritable()) {
				vm.bufferChange<MemoryChange>(vm, translated_destination, value, Size::QWord);
				vm.setQuarterword(translated_destination, value);
				vm.increment();
			} else
				vm.intBwrite(translated_destination);
		}
	}

	void lsOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		bool success;
		const Word translated = vm.translateAddress(rs, &success);
		if (!success) {
			vm.intPfault();
		} else {
			setReg(vm, rd, vm.getQuarterword(translated), false);
			vm.increment();
		}
	}

	void ssOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		bool success;
		const Word translated = vm.translateAddress(rd, &success);
		if (!success) {
			vm.intPfault();
		} else if (vm.checkWritable()) {
			vm.bufferChange<MemoryChange>(vm, translated, rs, Size::QWord);
			vm.setQuarterword(translated, rs);
			vm.increment();
		} else
			vm.intBwrite(translated);
	}

	void transOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		if (vm.pagingOn) {
			bool success;
			const Word translated = vm.translateAddress(rs, &success);
			if (!success) {
				vm.intPfault();
			} else {
				setReg(vm, rd, translated, false);
				vm.increment();
			}
		} else {
			setReg(vm, rd, rs, false);
			vm.increment();
		}
	}

	void liOp(VM &vm, Word &, Word &rd, Conditions, int, HWord immediate) {
		bool success;
		const Word translated = vm.translateAddress(immediate, &success);
		if (!success) {
			vm.intPfault();
		} else {
			setReg(vm, rd, vm.getWord(translated), false);
			vm.increment();
		}
	}

	void siOp(VM &vm, Word &rs, Word &, Conditions, int, HWord immediate) {
		bool success;
		const Word translated = vm.translateAddress(immediate, &success);
		if (!success) {
			vm.intPfault();
		} else if (vm.checkWritable()) {
			vm.bufferChange<MemoryChange>(vm, translated, rs, Size::Word);
			vm.setWord(translated, rs);
			vm.increment();
		} else
			vm.intBwrite(translated);
	}

	void setOp(VM &vm, Word &, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, immediate, false);
		vm.increment();
	}

	void lbiOp(VM &vm, Word &, Word &rd, Conditions, int, HWord immediate) {
		bool success;
		const Word translated = vm.translateAddress(immediate, &success);
		if (!success) {
			vm.intPfault();
		} else {
			setReg(vm, rd, vm.getByte(translated), false);
			vm.increment();
		}
	}

	void sbiOp(VM &vm, Word &rs, Word &, Conditions, int, HWord immediate) {
		bool success;
		const Word translated = vm.translateAddress(immediate, &success);
		if (!success) {
			vm.intPfault();
		} else if (vm.checkWritable()) {
			vm.bufferChange<MemoryChange>(vm, translated, rs, Size::Byte);
			vm.setByte(translated, rs);
			vm.increment();
		} else
			vm.intBwrite(translated);
	}

	void lniOp(VM &vm, Word &, Word &rd, Conditions, int, HWord immediate) {
		bool success;
		const Word translated = vm.translateAddress(immediate, &success);
		if (!success) {
			vm.intPfault();
		} else {
			const Word value = vm.getWord(translated);
			vm.bufferChange<MemoryChange>(vm, rd, value, Size::Word);
			vm.setWord(rd, value);
			vm.increment();
		}
	}

	void lbniOp(VM &vm, Word &, Word &rd, Conditions, int, HWord immediate) {
		bool success;
		const Word translated = vm.translateAddress(immediate, &success);
		if (!success) {
			vm.intPfault();
		} else {
			const Byte value = vm.getByte(translated);
			vm.bufferChange<MemoryChange>(vm, rd, value, Size::Byte);
			vm.setByte(rd, value);
			vm.increment();
		}
	}

	void intOp(VM &vm, Word &, Word &, Conditions, int, HWord immediate) {
		vm.interrupt(immediate, false);
	}

	void ritOp(VM &vm, Word &, Word &, Conditions, int, HWord immediate) {
		if (vm.checkRing(Ring::Zero)) {
			bool success;
			const Word translated = vm.translateAddress(immediate, &success);
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

	void timeOp(VM &vm, Word &rs, Word &, Word &, Conditions, int) {
		if (vm.checkRing(Ring::Zero)) {
			vm.setTimer(static_cast<UWord>(rs));
			vm.increment();
		} else
			vm.intProtec();
	}

	void svtimeOp(VM &vm, Word &, Word &, Word &rd, Conditions, int) {
		if (vm.checkRing(Ring::Zero)) {
			setReg(vm, rd, vm.timerTicks);
			vm.increment();
		} else
			vm.intProtec();
	}

	void timeiOp(VM &vm, Word &, Word &, Conditions, int, HWord immediate) {
		if (vm.checkRing(Ring::Zero)) {
			vm.setTimer(static_cast<UWord>(immediate));
			vm.increment();
		} else
			vm.intProtec();
	}

	void ringOp(VM &vm, Word &rs, Word &, Word &, Conditions, int) {
		if (vm.changeRing(static_cast<Ring>(rs)))
			vm.increment();
	}

	void svringOp(VM &vm, Word &, Word &, Word &rd, Conditions, int) {
		setReg(vm, rd, Word(vm.ring), false);
		vm.increment();
	}

	void ringiOp(VM &vm, Word &, Word &, Conditions, int, HWord immediate) {
		if (vm.changeRing(static_cast<Ring>(immediate)))
			vm.increment();
	}

	void prOp(VM &vm, Word &rs, Word &, Word &, Conditions, int) {
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
		vm.increment();
	}

	void haltOp(VM &vm, Word &, Word &, Word &, Conditions, int) {
		vm.recordChange<HaltChange>();
		vm.stop();
	}

	void evalOp(VM &vm, Word &, Word &, Word &, Conditions, int) {
		warn() << "<eval> not implemented\n";
		vm.increment();
	}

	void prcOp(VM &vm, Word &rs, Word &, Word &, Conditions, int) {
		vm.onPrint(std::string(1, static_cast<char>(rs)));
		vm.increment();
	}

	void prdOp(VM &vm, Word &rs, Word &, Word &, Conditions, int) {
		vm.onPrint(std::to_string(rs));
		vm.increment();
	}

	void prxOp(VM &vm, Word &rs, Word &, Word &, Conditions, int) {
		std::stringstream ss;
		ss << std::hex << rs;
		vm.onPrint(ss.str());
		vm.increment();
	}

	void sleepOp(VM &vm, Word &rs, Word &, Word &, Conditions, int) {
		usleep(rs);
		vm.increment();
	}

	void prbOp(VM &vm, Word &rs, Word &, Word &, Conditions, int) {
		std::stringstream ss;
		ss << std::bitset<64>(rs);
		vm.onPrint(ss.str());
		vm.increment();
	}

	void restOp(VM &vm, Word &, Word &, Word &, Conditions, int) {
		vm.increment();
		vm.rest();
	}

	void ioOp(VM &vm, Word &, Word &, Word &, Conditions, int) {
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

			vm.increment();
		} else
			vm.intProtec();
	}

	void selOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int) {
		setReg(vm, rd, vm.checkConditions(conditions)? rs : rt, false);
		vm.increment();
	}

	void pgoffOp(VM &vm, Word &, Word &, Word &, Conditions, int) {
		if (vm.checkRing(Ring::Zero)) {
			vm.bufferChange<PagingChange>(vm.pagingOn, false);
			vm.pagingOn = false;
			std::cerr << "Paging disabled (PC: " << vm.programCounter << ").\n";
			vm.onPagingChange(false);
			vm.increment();
		} else
			vm.intProtec();
	}

	void pgonOp(VM &vm, Word &, Word &, Word &, Conditions, int) {
		if (vm.checkRing(Ring::Zero)) {
			vm.bufferChange<PagingChange>(vm.pagingOn, true);
			vm.pagingOn = true;
			std::cerr << "Paging enabled (PC: " << vm.programCounter << ").\n";
			vm.onPagingChange(true);
			vm.increment();
		} else
			vm.intProtec();
	}

	void setptOp(VM &vm, Word &rs, Word &rt, Word &, Conditions, int) {
		if (vm.checkRing(Ring::Zero)) {
			vm.bufferChange<P0Change>(vm.p0, rs);
			vm.p0 = rs;
			std::cerr << "Page table address set to " << vm.p0 << " (PC: " << vm.programCounter << ").\n";
			vm.onP0Change(rs);
			if (rt != 0) {
				const auto reg_id = vm.registerID(rt);
				// Reenable interrupts if jumping to $e0.
				if (reg_id == Why::exceptionOffset && vm.checkRing(Ring::Zero)) {
					vm.hardwareInterruptsEnabled = true;
					vm.wakeRest();
				}
				vm.jump(rt, false);
			} else
				vm.increment();
		} else
			vm.intProtec();
	}

	void svpgOp(VM &vm, Word &, Word &, Word &rd, Conditions, int) {
		setReg(vm, rd, vm.pagingOn? 1 : 0, false);
		vm.increment();
	}

	void ppushOp(VM &vm, Word &, Word &, Word &, Conditions, int) {
		if (vm.checkRing(Ring::Zero)) {
			vm.pagingStack.emplace_back(vm);
			vm.increment();
		} else
			vm.intProtec();
	}

	void ppopOp(VM &vm, Word &rs, Word &, Word &, Conditions, int) {
		if (vm.checkRing(Ring::Zero)) {
			if (!vm.pagingStack.empty()) {
				const auto &back = vm.pagingStack.back();
				vm.bufferChange<PagingChange>(vm.pagingOn, back.enabled);
				vm.bufferChange<P0Change>(vm.p0, back.p0);
				vm.pagingOn = back.enabled;
				vm.p0 = back.p0;
				vm.pagingStack.pop_back();
			}
			if (rs != 0) {
				const auto reg_id = vm.registerID(rs);
				// Reenable interrupts if jumping to $e0.
				if (reg_id == Why::exceptionOffset && vm.checkRing(Ring::Zero)) {
					vm.hardwareInterruptsEnabled = true;
					vm.wakeRest();
				}
				vm.jump(rs, false);
			} else
				vm.increment();
		} else
			vm.intProtec();
	}

	void qmOp(VM &vm, Word &, Word &, Word &rd, Conditions, int) {
		setReg(vm, rd, vm.getMemorySize(), false);
		vm.increment();
	}

	void diOp(VM &vm, Word &, Word &, Word &, Conditions, int) {
		if (vm.checkRing(Ring::Zero)) {
			vm.hardwareInterruptsEnabled = false;
			vm.increment();
		} else
			vm.intProtec();
	}

	void eiOp(VM &vm, Word &, Word &, Word &, Conditions, int) {
		if (vm.checkRing(Ring::Zero)) {
			vm.hardwareInterruptsEnabled = true;
			vm.increment();
		} else
			vm.intProtec();
	}
}
