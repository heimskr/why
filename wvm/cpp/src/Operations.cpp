#include "mult.h"
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

	void addOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = rs + rt);
		vm.increment();
	}

	void subOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = rs - rt);
		vm.increment();
	}

	void multOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		CAT_MUL128(vm.registers[Why::hiOffset], vm.registers[Why::loOffset], rs, rt);
		vm.increment();
	}

	void multuOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {
		// TODO: mult and multu shouldn't be the same.
		multOp(vm, rs, rt, rd, conditions, flags);
	}

	void sllOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = rs << rt);
		vm.increment();
	}

	void srlOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = static_cast<UWord>(rs) >> static_cast<UWord>(rt));
		vm.increment();
	}

	void sraOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = rs >> rt);
		vm.increment();
	}

	void modOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = rs % rt);
		vm.increment();
	}

	void divOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = rs / rt);
		vm.increment();
	}

	void divuOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = static_cast<UWord>(rs) / static_cast<UWord>(rt));
		vm.increment();
	}

	void andOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = rs & rt);
		vm.increment();
	}

	void nandOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = ~(rs & rt));
		vm.increment();
	}

	void norOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = ~(rs | rt));
		vm.increment();
	}

	void notOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = ~rs);
		vm.increment();
	}

	void orOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = rs | rt);
		vm.increment();
	}

	void xnorOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = ~(rs ^ rt));
		vm.increment();
	}

	void xorOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = rs ^ rt);
		vm.increment();
	}

	void landOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = rs && rt);
		vm.increment();
	}

	void lnandOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = !(rs && rt));
		vm.increment();
	}

	void lnorOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = !(rs || rt));
		vm.increment();
	}

	void lnotOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = !rs);
		vm.increment();
	}

	void lorOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = rs || rt);
		vm.increment();
	}

	void lxnorOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = !(rs ^ rt));
		vm.increment();
	}

	void lxorOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions, int) {
		vm.updateFlags(rd = !!(rs ^ rt));
		vm.increment();
	}

	void addiOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void subiOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void multiOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void multuiOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void slliOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void srliOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void sraiOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void modiOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void diviOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void divuiOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void diviiOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void divuiiOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void andiOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void nandiOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void noriOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void oriOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void xnoriOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void xoriOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void luiOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void slOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void sleOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void seqOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void sluOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void sleuOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void sliOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void sleiOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void seqiOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void sluiOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void sleuiOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void sgiOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void sgeiOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void jOp(VM &vm, Word &rs, bool link, Conditions conditions, int flags, HWord address) {

	}

	void jcOp(VM &vm, Word &rs, bool link, Conditions conditions, int flags, HWord address) {

	}

	void jrOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void jrcOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void jrlOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void jrlcOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void cOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void lOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void sOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void cbOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void lbOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void sbOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void spushOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void spopOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void chOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void lhOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void shOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void liOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void siOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void setOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void lbiOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void sbiOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void lniOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void lbniOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void intOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void ritOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void timeOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void timeiOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void ringOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void ringiOp(VM &vm, Word &rs, Word &rd, Conditions conditions, int flags, HWord immediate) {

	}

	void prOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void haltOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void evalOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void prcOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void prdOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void prxOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}

	void sleepOp(VM &vm, Word &rs, Word &rt, Word &rd, Conditions conditions, int flags) {

	}
}