#include <iomanip>
#include <iostream>
#include <sstream>

#include <unistd.h>

#include "lib/ansi.h"
#include "mult.h"
#include "Operations.h"
#include "Util.h"
#include "VM.h"

namespace WVM::Operations {
	std::set<int> RSet {
		OP_ADD, OP_SUB, OP_MULT, OP_MULTU, OP_SLL, OP_SRL, OP_SRA, OP_MOD, OP_DIV, OP_DIVU, OP_AND, OP_NAND, OP_NOR,
		OP_NOT, OP_OR, OP_XNOR, OP_XOR, OP_LAND, OP_LNAND, OP_LNOR, OP_LNOT, OP_LOR, OP_LXNOR, OP_LXOR, OP_CMP, OP_SL,
		OP_SLE, OP_SEQ, OP_SLU, OP_SLEU, OP_JR, OP_JRC, OP_JRL, OP_JRLC, OP_C, OP_L, OP_S, OP_CB, OP_LB, OP_SB,
		OP_SPUSH, OP_SPOP, OP_CH, OP_LH, OP_SH, OP_TIME, OP_RING, OP_PR, OP_HALT, OP_EVAL, OP_PRC, OP_PRD, OP_PRX,
		OP_SLEEP, OP_SEL,
	};

	std::set<int> ISet {
		OP_ADDI, OP_SUBI, OP_MULTI, OP_MULTUI, OP_SLLI, OP_SRLI, OP_SRAI, OP_MODI, OP_DIVI, OP_DIVUI, OP_DIVII,
		OP_DIVUII, OP_ANDI, OP_NANDI, OP_NORI, OP_ORI, OP_XNORI, OP_XORI, OP_LUI, OP_SLI, OP_SLEI, OP_CMPI, OP_SEQI,
		OP_SLUI, OP_SLEUI, OP_SGI, OP_SGEI, OP_LI, OP_SI, OP_SET, OP_LBI, OP_SBI, OP_LNI, OP_LBNI, OP_INT, OP_RIT,
		OP_TIMEI, OP_RINGI,
	};

	std::set<int> JSet {OP_J, OP_JC};

	void execute(VM &vm, UWord instruction) {
		int opcode = (instruction >> 52) & 0xfff;
		if (opcode == OP_NOP) {
			vm.increment();
		} else if (RSet.count(opcode) == 1) {
			int rs, rt, rd;
			Conditions conditions;
			int flags, funct;
			decodeRType(instruction, rs, rt, rd, conditions, flags, funct);
			executeRType(opcode, vm, vm.registers[rs], vm.registers[rt], vm.registers[rd], conditions, flags, funct);
		} else if (ISet.count(opcode) == 1) {
			int rs, rd;
			Conditions conditions;
			int flags;
			HWord immediate;
			decodeIType(instruction, rs, rd, conditions, flags, immediate);
			executeIType(opcode, vm, vm.registers[rs], vm.registers[rd], conditions, flags, immediate);
		} else if (JSet.count(opcode) == 1) {
			int rs;
			bool link;
			Conditions conditions;
			int flags;
			HWord address;
			decodeJType(instruction, rs, link, conditions, flags, address);
			executeJType(opcode, vm, vm.registers[rs], link, conditions, flags, address);
		} else throw std::runtime_error("Unknown opcode: " + std::to_string(opcode));
	}

	void executeRType(int opcode, VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags, int funct) {
		switch (opcode) {
			case OP_RMATH:
				switch (funct) {
					case FN_ADD:     addOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_SUB:     subOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_MULT:   multOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_MULTU: multuOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_SLL:     sllOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_SRL:     srlOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_SRA:     sraOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_MOD:     modOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_DIV:     divOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_DIVU:   divuOp(vm, rs, rt, rd, conditions, flags); return;
				}
				break;
			case OP_RLOGIC:
				switch (funct) {
					case FN_AND:     andOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_NAND:   nandOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_NOR:     norOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_NOT:     notOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_OR:       orOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_XNOR:   xnorOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_XOR:     xorOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_LAND:   landOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_LNAND: lnandOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_LNOR:   lnorOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_LNOT:   lnotOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_LOR:     lorOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_LXNOR: lxnorOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_LXOR:   lxorOp(vm, rs, rt, rd, conditions, flags); return;
				}
				break;
			case OP_RCOMP:
				switch (funct) {
					case FN_SL:     slOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_SLE:   sleOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_SEQ:   seqOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_SLU:   sluOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_SLEU: sleuOp(vm, rs, rt, rd, conditions, flags); return;
				}
				break;
			case OP_RJUMP:
				switch (funct) {
					case FN_JR:     jrOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_JRC:   jrcOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_JRL:   jrlOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_JRLC: jrlcOp(vm, rs, rt, rd, conditions, flags); return;
				}
				break;
			case OP_RMEM:
				switch (funct) {
					case FN_C:         cOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_L:         lOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_S:         sOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_CB:       cbOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_LB:       lbOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_SB:       sbOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_SPUSH: spushOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_SPOP:   spopOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_CH:       chOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_LH:       lhOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_SH:       shOp(vm, rs, rt, rd, conditions, flags); return;
				}
				break;
			case OP_REXT:
				switch (funct) {
					case FN_PR:       prOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_HALT:   haltOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_EVAL:   evalOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_PRC:     prcOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_PRD:     prdOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_PRX:     prxOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_SLEEP: sleepOp(vm, rs, rt, rd, conditions, flags); return;
				}
				break;
			case OP_TIME: timeOp(vm, rs, rt, rd, conditions, flags); return;
			case OP_RING: ringOp(vm, rs, rt, rd, conditions, flags); return;
			case OP_SEL:   selOp(vm, rs, rt, rd, conditions, flags); return;
		}

		throw std::runtime_error("Unknown R-type: " + std::to_string(opcode) + ":" + std::to_string(funct));
	}

	void executeIType(int opcode, VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {
		switch (opcode) {
			case OP_ADDI:     addiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_SUBI:     subiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_MULTI:   multiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_MULTUI: multuiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_SLLI:     slliOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_SRLI:     srliOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_SRAI:     sraiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_MODI:     modiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_DIVI:     diviOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_DIVUI:   divuiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_DIVII:   diviiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_DIVUII: divuiiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_ANDI:     andiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_NANDI:   nandiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_NORI:     noriOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_ORI:       oriOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_XNORI:   xnoriOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_XORI:     xoriOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_LUI:       luiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_SLI:       sliOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_SLEI:     sleiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_SEQI:     seqiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_SLUI:     sluiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_SLEUI:   sleuiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_SGI:       sgiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_SGEI:     sgeiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_LI:         liOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_SI:         siOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_SET:       setOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_LBI:       lbiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_SBI:       sbiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_LNI:       lniOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_LBNI:     lbniOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_INT:       intOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_RIT:       ritOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_TIMEI:   timeiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_RINGI:   ringiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_CMPI:     cmpiOp(vm, rs, rd, conditions, flags, immediate); return;
		}

		throw std::runtime_error("Unknown I-type: " + std::to_string(opcode));
	}

	void executeJType(int opcode, VM &vm, Word &rs, bool link, Conditions conditions, int flags, HWord address) {
		switch (opcode) {
			case OP_J:   jOp(vm, rs, link, conditions, flags, address); return;
			case OP_JC: jcOp(vm, rs, link, conditions, flags, address); return;
		}

		throw std::runtime_error("Unknown J-type: " + std::to_string(opcode));
	}

	void decodeRType(UWord instr, int &rs, int &rt, int &rd, Conditions &conds, int &flags, int &funct) {
		rd = (instr >> 31) & 0b1111111;
		rs = (instr >> 38) & 0b1111111;
		rt = (instr >> 45) & 0b1111111;
		conds = static_cast<Conditions>((instr >> 14) & 0b1111);
		flags = (instr >> 12) & 0b11;
		funct = instr & 0xfff;
	}

	void decodeIType(UWord instr, int &rs, int &rd,  Conditions &conds, int &flags, HWord &immediate) {
		rs = (instr >> 39) & 0b1111111;
		rd = (instr >> 32) & 0b1111111;
		conds = static_cast<Conditions>((instr >> 48) & 0b1111);
		flags = (instr >> 46) & 0b11;
		immediate = instr & 0xffffffff;
	}

	void decodeJType(UWord instr, int &rs, bool &link, Conditions &conds, int &flags, HWord &address) {
		rs = (instr >> 45) & 0b1111111;
		link = (instr >> 44) & 1;
		conds = static_cast<Conditions>((instr >> 34) & 0b1111);
		flags = (instr >> 32) & 0b11;
		address = instr & 0xffffffff;
	}

	void addOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = rs + rt);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void subOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = rs - rt);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void multOp(VM &vm, Word &rs, Word &rt, Word &, Conditions, int) {
		CAT_MUL128(vm.hi(), vm.lo(), rs, rt);
		vm.onRegisterChange(Why::loOffset);
		vm.onRegisterChange(Why::hiOffset);
		vm.increment();
	}

	void multuOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {
		// TODO: mult and multu shouldn't be the same.
		multOp(vm, rs, rt, rd, conditions, flags);
	}

	void sllOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = rs << rt);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void srlOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = static_cast<UWord>(rs) >> static_cast<UWord>(rt));
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void sraOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = rs >> rt);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void modOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = rs % rt);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void divOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = rs / rt);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void divuOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = static_cast<UWord>(rs) / static_cast<UWord>(rt));
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void andOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = rs & rt);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void nandOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = ~(rs & rt));
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void norOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = ~(rs | rt));
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void notOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		vm.updateFlags(rd = ~rs);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void orOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = rs | rt);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void xnorOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = ~(rs ^ rt));
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void xorOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = rs ^ rt);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void landOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = rs && rt);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void lnandOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = !(rs && rt));
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void lnorOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = !(rs || rt));
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void lnotOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		vm.updateFlags(rd = !rs);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void lorOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = rs || rt);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void lxnorOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = !(rs ^ rt));
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void lxorOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = !!(rs ^ rt));
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void addiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		vm.updateFlags(rd = rs + immediate);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void subiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		vm.updateFlags(rd = rs - immediate);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void multiOp(VM &vm, Word &rs, Word &, Conditions, int, HWord immediate) {
		CAT_MUL128(vm.hi(), vm.lo(), rs, immediate);
		vm.onRegisterChange(Why::hiOffset);
		vm.onRegisterChange(Why::loOffset);
		vm.increment();
	}

	void multuiOp(VM &vm, Word &rs, Word &, Conditions, int, HWord immediate) {
		CAT_MUL128(vm.hi(), vm.lo(), rs, static_cast<UWord>(immediate));
		vm.onRegisterChange(Why::hiOffset);
		vm.onRegisterChange(Why::loOffset);
		vm.increment();
	}

	void slliOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		vm.updateFlags(rd = rs << immediate);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void srliOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		vm.updateFlags(rd = static_cast<UWord>(rs) >> static_cast<UWord>(immediate));
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void sraiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		vm.updateFlags(rd = rs >> immediate);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void modiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		vm.updateFlags(rd = rs % immediate);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void diviOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		vm.updateFlags(rd = rs / immediate);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void divuiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		vm.updateFlags(rd = static_cast<UWord>(rs) / static_cast<UWord>(immediate));
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void diviiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		vm.updateFlags(rd = immediate / rs);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void divuiiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		vm.updateFlags(rd = static_cast<UWord>(immediate) / static_cast<UWord>(rs));
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void andiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		vm.updateFlags(rd = rs & immediate);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void nandiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		vm.updateFlags(rd = ~(rs & immediate));
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void noriOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		vm.updateFlags(rd = ~(rs | immediate));
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void oriOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		vm.updateFlags(rd = rs | immediate);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void xnoriOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		vm.updateFlags(rd = ~(rs ^ immediate));
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void xoriOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		vm.updateFlags(rd = rs ^ immediate);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void luiOp(VM &vm, Word &, Word &rd, Conditions, int, HWord immediate) {
		vm.updateFlags(rd = (rd & 0xffffffff) | (static_cast<UWord>(immediate) << 32));
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void slOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		rd = rs < rt;
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void sleOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		rd = rs <= rt;
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void seqOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		rd = rs == rt;
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void sluOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		rd = static_cast<UWord>(rs) < static_cast<UWord>(rt);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void sleuOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		rd = static_cast<UWord>(rs) <= static_cast<UWord>(rt);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void cmpOp(VM &vm, Word &rs, Word &rt, Word &, Conditions, int) {
		vm.updateFlags(rs - rt);
		vm.increment();
	}

	void sliOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		rd = rs < immediate;
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void sleiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		rd = rs <= immediate;
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void seqiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		rd = rs == immediate;
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void sluiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		rd = static_cast<UWord>(rs) < static_cast<UWord>(immediate);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void sleuiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		rd = static_cast<UWord>(rs) <= static_cast<UWord>(immediate);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void sgiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		rd = rs > immediate;
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void sgeiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		rd = rs >= immediate;
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void cmpiOp(VM &vm, Word &rs, Word &, Conditions, int, HWord immediate) {
		vm.updateFlags(rs - immediate);
		vm.increment();
	}

	void jOp(VM &vm, Word &, bool link, Conditions conditions, int, HWord address) {
		if (vm.checkConditions(conditions)) {
			vm.jump(address, link);
		} else vm.increment();
	}

	void jcOp(VM &vm, Word &rs, bool link, Conditions, int, HWord address) {
		if (rs != 0) {
			vm.jump(address, link);
		} else vm.increment();
	}

	void jrOp(VM &vm, Word &, Word &, Word &rd, Conditions conditions, int) {
		if (vm.checkConditions(conditions)) {
			vm.jump(rd, false);
		} else vm.increment();
	}

	void jrcOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		if (rs) {
			vm.jump(rd, false);
		} else vm.increment();
	}

	void jrlOp(VM &vm, Word &, Word &, Word &rd, Conditions conditions, int) {
		if (vm.checkConditions(conditions)) {
			vm.jump(rd, true);
		} else vm.increment();
	}

	void jrlcOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		if (rs) {
			vm.jump(rd, true);
		} else vm.increment();
	}

	void cOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		vm.setWord(rd, vm.getWord(rs));
		vm.increment();
	}

	void lOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		rd = vm.getWord(rs);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void sOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		vm.setWord(rd, rs);
		vm.increment();
	}

	void cbOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		vm.setByte(rd, vm.getByte(rs));
		vm.increment();
	}

	void lbOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		rd = vm.getByte(rs);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void sbOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		vm.setByte(rd, rs);
		vm.increment();
	}

	void spushOp(VM &vm, Word &rs, Word &, Word &, Conditions, int) {
		vm.setWord(vm.sp(), rs);
		vm.sp() -= 8;
		vm.onRegisterChange(Why::stackPointerOffset);
		vm.increment();
	}

	void spopOp(VM &vm, Word &, Word &, Word &rd, Conditions, int) {
		vm.sp() += 8;
		rd = vm.getWord(vm.sp());
		vm.onRegisterChange(vm.registerID(rd));
		vm.onRegisterChange(Why::stackPointerOffset);
		vm.increment();
	}

	void chOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		vm.setHalfword(rd, vm.getHalfword(rs));
		vm.increment();
	}

	void lhOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		rd = vm.getHalfword(rs);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void shOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		vm.setHalfword(rd, rs);
		vm.increment();
	}

	void liOp(VM &vm, Word &, Word &rd, Conditions, int, HWord immediate) {
		rd = vm.getWord(immediate);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void siOp(VM &vm, Word &rs, Word &, Conditions, int, HWord immediate) {
		vm.setWord(immediate, rs);
		vm.increment();
	}

	void setOp(VM &vm, Word &, Word &rd, Conditions, int, HWord immediate) {
		rd = immediate;
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void lbiOp(VM &vm, Word &, Word &rd, Conditions, int, HWord immediate) {
		rd = vm.getByte(immediate);
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}

	void sbiOp(VM &vm, Word &rs, Word &, Conditions, int, HWord immediate) {
		vm.setByte(immediate, rs);
		vm.increment();
	}

	void lniOp(VM &vm, Word &, Word &rd, Conditions, int, HWord immediate) {
		vm.setWord(rd, vm.getWord(immediate));
		vm.increment();
	}

	void lbniOp(VM &vm, Word &, Word &rd, Conditions, int, HWord immediate) {
		vm.setByte(rd, vm.getByte(immediate));
		vm.increment();
	}

	void intOp(VM &vm, Word &, Word &, Conditions, int, HWord immediate) {
		if (vm.interrupt(immediate))
			vm.increment();
	}

	void ritOp(VM &vm, Word &, Word &, Conditions, int, HWord immediate) {
		if (vm.checkRing(Ring::Zero)) {
			vm.interruptTableAddress = immediate;
			vm.onInterruptTableChange();
			vm.increment();
		}
	}

	void timeOp(VM &vm, Word &, Word &, Word &, Conditions, int) {
		// TODO
		vm.increment();
	}

	void timeiOp(VM &vm, Word &, Word &, Conditions, int, HWord) {
		// TODO
		vm.increment();
	}

	void ringOp(VM &vm, Word &rs, Word &, Word &, Conditions, int) {
		if (vm.changeRing(static_cast<Ring>(rs)))
			vm.increment();
	}

	void ringiOp(VM &vm, Word &, Word &, Conditions, int, HWord immediate) {
		if (vm.changeRing(static_cast<Ring>(immediate)))
			vm.increment();
	}

	void prOp(VM &vm, Word &rs, Word &, Word &, Conditions, int) {
		std::stringstream ss;
		ss << Why::coloredRegister(&rs - vm.registers) << ": " // << "0x" << std::hex << rs << " \e[2m/\e[22m " << std::dec
		   << rs;
		// vm.onPrint(ss.str());
		DBG(ss.str());
		vm.increment();
	}

	void haltOp(VM &vm, Word &, Word &, Word &, Conditions, int) {
		vm.stop();
	}

	void evalOp(VM &vm, Word &, Word &, Word &, Conditions, int) {
		warn() << "<eval> not implemented\n";
		vm.increment();
	}

	void prcOp(VM &vm, Word &rs, Word &, Word &, Conditions, int) {
		vm.onPrint(std::string(1,  static_cast<char>(rs)));
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
		usleep(rs * 1000);
		vm.increment();
	}

	void selOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int) {
		rd = vm.checkConditions(conditions)? rs : rt;
		vm.onRegisterChange(vm.registerID(rd));
		vm.increment();
	}
}
