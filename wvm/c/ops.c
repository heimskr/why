/***********************************************
 * Contains functions that execute operations. *
 ***********************************************/

#include "ops.h"
#include "instruction.h"
#include "registers.h"

#define REGS() reg_t rs, rt, rd; wvm_r_regs(instruction, &rs, &rt, &rd);

void op_add(word instruction) {
	REGS();
	registers[rd] = registers[rs] + registers[rt];
}

void op_sub(word instruction) {
	REGS();
	registers[rd] = registers[rs] - registers[rt];
}

void op_mult(word instruction) {
	// TODO: catching overflow is hard.
}

void op_addu(word instruction) {
	// TODO: signed arithmetic and unsigned arithmetic are pretty much the same?
	// Unless they're not? It's 3:51 AM and I'm tired.
	op_add(instruction);
}

void op_subu(word instruction) {
	op_sub(instruction);
}

void op_multu(word instruction) {
	op_mult(instruction);
}

void op_sll(word instruction) {
	REGS();
	registers[rd] = registers[rs] << registers[rt];
}

void op_srl(word instruction) {
	REGS();
	registers[rd] = ((uword) registers[rs]) >> ((uword) registers[rt]);
}

void op_sra(word instruction) {
	REGS();
	registers[rd] = registers[rs] >> registers[rt];
}

void op_and(word instruction) {
	REGS();
	registers[rd] = registers[rs] & registers[rt];
}

void op_nand(word instruction) {

	REGS();
	registers[rd] = ~(registers[rs] & registers[rt]);
}

void op_nor(word instruction) {
	REGS();
	registers[rd] = ~(registers[rs] | registers[rt]);

}

void op_not(word instruction) {
	reg_t rs = wvm_r_rs(instruction);
	reg_t rd = wvm_r_rd(instruction);
	registers[rd] = ~registers[rs];
}

void op_or(word instruction) {
	REGS();
	registers[rd] = registers[rs] | registers[rt];

}

void op_xnor(word instruction) {
	REGS();
	registers[rd] = ~(registers[rs] ^ registers[rt]);
}

void op_xor(word instruction) {
	REGS();
	registers[rd] = registers[rs] ^ registers[rt];
}

void op_addi(word instruction) {
	
}

void op_subi(word instruction) {

}

void op_multi(word instruction) {

}

void op_addui(word instruction) {

}

void op_subui(word instruction) {

}

void op_multui(word instruction) {

}

void op_slli(word instruction) {

}

void op_srli(word instruction) {

}

void op_srai(word instruction) {

}

void op_andi(word instruction) {

}

void op_nandi(word instruction) {

}

void op_nori(word instruction) {

}

void op_ori(word instruction) {

}

void op_xnori(word instruction) {

}

void op_xori(word instruction) {

}

void op_lui(word instruction) {

}

void op_sl(word instruction) {

}

void op_sle(word instruction) {

}

void op_seq(word instruction) {

}

void op_slu(word instruction) {

}

void op_sleu(word instruction) {

}

void op_sli(word instruction) {

}

void op_slei(word instruction) {

}

void op_seqi(word instruction) {

}

void op_slui(word instruction) {

}

void op_sleui(word instruction) {

}

void op_j(word instruction) {

}

void op_jc(word instruction) {

}

void op_jl(word instruction) {

}

void op_jlc(word instruction) {

}

void op_jr(word instruction) {

}

void op_jrc(word instruction) {

}

void op_jrl(word instruction) {

}

void op_jrlc(word instruction) {

}

void op_c(word instruction) {

}

void op_l(word instruction) {

}

void op_s(word instruction) {

}

void op_cb(word instruction) {

}

void op_lb(word instruction) {

}

void op_sb(word instruction) {

}

void op_li(word instruction) {

}

void op_si(word instruction) {

}

void op_lbi(word instruction) {

}

void op_sbi(word instruction) {

}

void op_set(word instruction) {

}
