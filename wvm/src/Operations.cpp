#include <bitset>
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
	};

	std::set<int> ISet {
		OP_ADDI, OP_SUBI, OP_MULTI, OP_MULTUI, OP_SLLI, OP_SRLI, OP_SRAI, OP_MODI, OP_DIVI, OP_DIVUI, OP_DIVII,
		OP_DIVUII, OP_ANDI, OP_NANDI, OP_NORI, OP_ORI, OP_XNORI, OP_XORI, OP_LUI, OP_SLI, OP_SLEI, OP_CMPI, OP_SEQI,
		OP_SLUI, OP_SLEUI, OP_SGI, OP_SGEI, OP_LI, OP_SI, OP_SET, OP_LBI, OP_SBI, OP_LNI, OP_LBNI, OP_INT, OP_RIT,
		OP_TIMEI, OP_RINGI, OP_SSPUSH, OP_SSPOP, OP_SGEUI, OP_SGUI,
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
		} else
			throw std::runtime_error("Unknown opcode at " + std::to_string(vm.programCounter) + ": " +
				std::to_string(opcode));
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
					case FN_MS:       msOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_CS:       csOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_LS:       lsOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_SS:       ssOp(vm, rs, rt, rd, conditions, flags); return;
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
					case FN_PRB:     prbOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_REST:   restOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_IO:       ioOp(vm, rs, rt, rd, conditions, flags); return;
				}
				break;
			case OP_TIME: timeOp(vm, rs, rt, rd, conditions, flags); return;
			case OP_RING: ringOp(vm, rs, rt, rd, conditions, flags); return;
			case OP_SEL:   selOp(vm, rs, rt, rd, conditions, flags); return;
			case OP_PAGE:
				switch (funct) {
					case FN_PGOFF: pgoffOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_PGON:   pgonOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_SETPT: setptOp(vm, rs, rt, rd, conditions, flags); return;
					case FN_SVPG:   svpgOp(vm, rs, rt, rd, conditions, flags); return;
				}
				break;
			case OP_QUERY:
				switch (funct) {
					case FN_QM: qmOp(vm, rs, rt, rd, conditions, flags); return;
				}
				break;
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
			case OP_SGEUI:   sgeuiOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_SGUI:     sguiOp(vm, rs, rd, conditions, flags, immediate); return;
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
			case OP_SSPUSH: sspushOp(vm, rs, rd, conditions, flags, immediate); return;
			case OP_SSPOP:   sspopOp(vm, rs, rd, conditions, flags, immediate); return;
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

	void setReg(VM &vm, Word &rd, Word value, bool update_flags = true) {
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
		setReg(vm, rd, !(rs ^ rt));
		vm.increment();
	}

	void lxorOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		setReg(vm, rd, !!(rs ^ rt));
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
		setReg(vm, rd, static_cast<UWord>(rs) >> static_cast<UWord>(immediate));
		vm.increment();
	}

	void sraiOp(VM &vm, Word &rs, Word &rd, Conditions, int, HWord immediate) {
		setReg(vm, rd, rs >> immediate);
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
		if (vm.checkConditions(conditions)) {
			bool success;
			const Word translated = vm.translateAddress(address, &success);
			if (!success)
				vm.intPfault();
			else
				vm.jump(translated, link);
		} else vm.increment();
	}

	void jcOp(VM &vm, Word &rs, bool link, Conditions, int, HWord address) {
		if (rs != 0) {
			bool success;
			const Word translated = vm.translateAddress(address, &success);
			if (!success)
				vm.intPfault();
			else
				vm.jump(translated, link);
		} else vm.increment();
	}

	void jrOp(VM &vm, Word &, Word &, Word &rd, Conditions conditions, int) {
		if (vm.checkConditions(conditions)) {
			bool success;
			const Word translated = vm.translateAddress(rd, &success);
			if (!success)
				vm.intPfault();
			else
				vm.jump(translated, false);
		} else vm.increment();
	}

	void jrcOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		if (rs) {
			bool success;
			const Word translated = vm.translateAddress(rd, &success);
			if (!success)
				vm.intPfault();
			else
				vm.jump(translated, false);
		} else vm.increment();
	}

	void jrlOp(VM &vm, Word &, Word &, Word &rd, Conditions conditions, int) {
		if (vm.checkConditions(conditions)) {
			bool success;
			const Word translated = vm.translateAddress(rd, &success);
			if (!success)
				vm.intPfault();
			else
				vm.jump(translated, true);
		} else vm.increment();
	}

	void jrlcOp(VM &vm, Word &rs, Word &, Word &rd, Conditions, int) {
		if (rs) {
			bool success;
			const Word translated = vm.translateAddress(rd, &success);
			if (!success)
				vm.intPfault();
			else
				vm.jump(translated, true);
		} else vm.increment();
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
				vm.intBwrite();
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
			vm.intBwrite();
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
				vm.intBwrite();
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
			vm.intBwrite();
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
			vm.intBwrite();
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
			vm.intBwrite();
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
				vm.intBwrite();
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
			vm.intBwrite();
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
				vm.intBwrite();
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
				vm.intBwrite();
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
			vm.intBwrite();
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
			vm.intBwrite();
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
			vm.intBwrite();
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
		if (vm.interrupt(immediate, false))
			vm.increment();
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
		}
	}

	void timeOp(VM &vm, Word &rs, Word &, Word &, Conditions, int) {
		if (vm.checkRing(Ring::Zero)) {
			vm.setTimer(static_cast<UWord>(rs));
			vm.increment();
		}
	}

	void timeiOp(VM &vm, Word &, Word &, Conditions, int, HWord immediate) {
		if (vm.checkRing(Ring::Zero)) {
			vm.setTimer(static_cast<UWord>(immediate));
			vm.increment();
		}
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
		   << rs << "\n";
		vm.onPrint(ss.str());
		DBG(ss.str());
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
								vm.intBwrite();
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
								vm.intBwrite();
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
		}
	}

	void selOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int) {
		setReg(vm, rd, vm.checkConditions(conditions)? rs : rt, false);
		vm.increment();
	}

	void pgoffOp(VM &vm, Word &, Word &, Word &, Conditions, int) {
		if (vm.checkRing(Ring::Zero)) {
			vm.recordChange<PagingChange>(vm.pagingOn, false);
			vm.pagingOn = false;
			std::cerr << "Paging disabled (PC: " << vm.programCounter << ").\n";
			vm.onPagingChange(false);
			vm.increment();
		}
	}

	void pgonOp(VM &vm, Word &, Word &, Word &, Conditions, int) {
		if (vm.checkRing(Ring::Zero)) {
			vm.recordChange<PagingChange>(vm.pagingOn, true);
			vm.pagingOn = true;
			std::cerr << "Paging enabled (PC: " << vm.programCounter << ").\n";
			vm.onPagingChange(true);
			vm.increment();
		}
	}

	void setptOp(VM &vm, Word &rs, Word &, Word &, Conditions, int) {
		if (vm.checkRing(Ring::Zero)) {
			vm.recordChange<P0Change>(vm.p0, rs);
			vm.p0 = rs;
			std::cerr << "Page table address set to " << vm.p0 << " (PC: " << vm.programCounter << ").\n";
			vm.onP0Change(rs);
			vm.increment();
		}
	}

	void svpgOp(VM &vm, Word &, Word &, Word &rd, Conditions, int) {
		setReg(vm, rd, vm.pagingOn? 1 : 0, false);
		vm.increment();
	}

	void qmOp(VM &vm, Word &, Word &, Word &rd, Conditions, int) {
		setReg(vm, rd, vm.getMemorySize(), false);
		vm.increment();
	}
}
