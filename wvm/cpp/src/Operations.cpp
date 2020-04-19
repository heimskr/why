#include "Operations.h"
#include "VM.h"

namespace WVM {
	std::set<int> RSet {
		OP_ADD, OP_SUB, OP_MULT, OP_MULTU, OP_SLL, OP_SRL, OP_SRA, OP_MOD, OP_DIV, OP_DIVU, OP_AND, OP_NAND, OP_NOR,
		OP_NOT, OP_OR, OP_XNOR, OP_XOR, OP_LAND, OP_LNAND, OP_LNOR, OP_LNOT, OP_LOR, OP_LXNOR, OP_LXOR, OP_SL, OP_SLE,
		OP_SEQ, OP_SLU, OP_SLEU, OP_JR, OP_JRC, OP_JRL, OP_JRLC, OP_C, OP_L, OP_S, OP_CB, OP_LB, OP_SB, OP_SPUSH,
		OP_SPOP, OP_CH, OP_LH, OP_SH, OP_TIME, OP_RING, OP_PR, OP_HALT, OP_EVAL, OP_PRC, OP_PRD, OP_PRX, OP_SLEEP,
	};

	std::set<int> ISet {
		OP_ADDI, OP_SUBI, OP_MULTI, OP_MULTUI, OP_SLLI, OP_SRLI, OP_SRAI, OP_MODI, OP_DIVI, OP_DIVUI, OP_DIVII,
		OP_DIVUII, OP_ANDI, OP_NANDI, OP_NORI, OP_ORI, OP_XNORI, OP_XORI, OP_LUI, OP_SLI, OP_SLEI, OP_SEQI, OP_SLUI,
		OP_SLEUI, OP_SGI, OP_SGEI, OP_LI, OP_SI, OP_SET, OP_LBI, OP_SBI, OP_LNI, OP_LBNI, OP_INT, OP_RIT, OP_TIMEI,
		OP_RINGI,
	};

	std::set<int> JSet {OP_J, OP_JC};

	void execute(VM &vm, Word instruction) {
		int opcode = (instruction >> 52) & 0xfff;
	}

	void decodeRType(VM &vm, UWord instr, Word *&rs, Word *&rt, Word *&rd, Conditions &conds, int &flags, int &funct) {
		rd = &vm.registers[(instr >> 31) & 0b1111111];
		rs = &vm.registers[(instr >> 38) & 0b1111111];
		rt = &vm.registers[(instr >> 45) & 0b1111111];
		conds = static_cast<Conditions>((instr >> 14) & 0b1111);
		flags = (instr >> 12) & 0b11;
		funct = instr & 0xfff;
	}

	void decodeIType(VM &vm, UWord instr, Word *&rs, Word *&rd,  Conditions &conds, int &flags, HWord &immediate) {
		rs = &vm.registers[(instr >> 39) & 0b1111111];
		rd = &vm.registers[(instr >> 32) & 0b1111111];
		conds = static_cast<Conditions>((instr >> 48) & 0b1111);
		flags = (instr >> 46) & 0b11;
		immediate = instr & 0xffffffff;
	}

	void decodeJType(VM &vm, UWord instr, Word *&rs, bool &link, Conditions &conds, int &flags, HWord &address) {
		rs = &vm.registers[(instr >> 45) & 0b1111111];
		link = (instr >> 44) & 1;
		conds = static_cast<Conditions>((instr >> 34) & 0b1111);
		flags = (instr >> 32) & 0b11;
		address = instr & 0xffffffff;
	}
}
