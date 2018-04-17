/***********************************************
 * Contains functions that execute operations. *
 ***********************************************/

#include <stdio.h>
#include "ops.h"
#include "instruction.h"
#include "registers.h"

#define RRS() reg_t rs = wvm_r_rs(instruction);
#define RRT() reg_t rt = wvm_r_rt(instruction);
#define RRD() reg_t rd = wvm_r_rd(instruction);
#define RREGS() reg_t rs, rt, rd; wvm_r_regs(instruction, &rs, &rt, &rd);

#define IRS() reg_t rs = wvm_i_rs(instruction);
#define IRD() reg_t rd = wvm_i_rd(instruction);
#define IIMM() imm_t imm = wvm_i_imm(instruction);
#define IREGS() IRS(); IRD(); IIMM();

#define JRS() reg_t rs = wvm_j_rs(instruction);
#define JADDR() imm_t addr = wvm_j_addr(instruction);
#define JREGS() JRS(); JADDR();

#define INC() wvm_increment_pc();

#define rsv registers[rs]
#define rtv registers[rt]
#define rdv registers[rd]

void op_nop(word instruction) {
	INC();
}

void op_add(word instruction) {
	RREGS();
	rdv = rsv + rtv;
	INC();
}

void op_sub(word instruction) {
	RREGS();
	rdv = rsv - rtv;
}

void op_mult(word instruction) {
	// TODO: catching overflow is hard.
	INC();
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

void op_mod(word instruction) {
	RREGS();
	rdv = rsv % rtv;
}

void op_sll(word instruction) {
	RREGS();
	rdv = rsv << rtv;
	INC();
}

void op_srl(word instruction) {
	RREGS();
	rdv = ((uword) rsv) >> (uword) rtv;
	INC();
}

void op_sra(word instruction) {
	RREGS();
	rdv = rsv >> rtv;
	INC();
}

void op_and(word instruction) {
	RREGS();
	rdv = rsv & rtv;
	INC();
}

void op_nand(word instruction) {
	RREGS();
	rdv = ~(rsv & rtv);
	INC();
}

void op_nor(word instruction) {
	RREGS();
	rdv = ~(rsv | rtv);
	INC();
}

void op_not(word instruction) {
	reg_t rs = wvm_r_rs(instruction);
	reg_t rd = wvm_r_rd(instruction);
	rdv = ~rsv;
	INC();
}

void op_or(word instruction) {
	RREGS();
	rdv = rsv | rtv;
	INC();
}

void op_xnor(word instruction) {
	RREGS();
	rdv = ~(rsv ^ rtv);
	INC();
}

void op_xor(word instruction) {
	RREGS();
	rdv = rsv ^ rtv;
	INC();
}

void op_land(word instruction) {
	RREGS();
	rdv = rsv && rtv;
	INC();
}

void op_lnand(word instruction) {
	RREGS();
	rdv = !(rsv && rtv);
	INC();
}

void op_lnor(word instruction) {
	RREGS();
	rdv = !(rsv || rtv);
	INC();
}

void op_lnot(word instruction) {
	reg_t rs = wvm_r_rs(instruction);
	reg_t rd = wvm_r_rd(instruction);
	rdv = !rsv;
	INC();
}

void op_lor(word instruction) {
	RREGS();
	rdv = rsv || rtv;
	INC();
}

void op_lxnor(word instruction) {
	RREGS();
	rdv = !(rsv ^ rtv);
	INC();
}

void op_lxor(word instruction) {
	RREGS();
	rdv = !!(rsv ^ rtv);
	INC();
}

void op_addi(word instruction) {
	IREGS();
	rdv = rsv + (word) imm;
	INC();
}

void op_subi(word instruction) {
	IREGS();
	rdv = rsv - (word) imm;
	INC();
}

void op_multi(word instruction) {
	// TODO: see op_mult.
	INC();
}

void op_addui(word instruction) {
	IREGS();
	// Does this do anything?
	rdv = ((uword) rsv) + (uword) imm;
	INC();
}

void op_subui(word instruction) {
	IREGS();
	rdv = ((uword) rsv) - (uword) imm;
	INC();
}

void op_multui(word instruction) {
	// TODO
	INC();
}

void op_slli(word instruction) {
	IREGS();
	rdv = rsv << (word) imm;
	INC();
}

void op_srli(word instruction) {
	IREGS();
	rdv = ((uword) rsv) >> (uword) imm;
	INC();
}

void op_srai(word instruction) {
	IREGS();
	rdv = rsv >> (word) imm;
	INC();
}

void op_modi(word instruction) {
	IREGS();
	rdv = rsv % (word) imm;
	INC();
}

void op_andi(word instruction) {
	IREGS();
	rdv = rsv & (word) imm;
	INC();
}

void op_nandi(word instruction) {
	IREGS();
	rdv = ~(rsv & (word) imm);
	INC();
}

void op_nori(word instruction) {
	IREGS();
	rdv = ~(rsv | (word) imm);
	INC();
}

void op_ori(word instruction) {
	IREGS();
	rdv = rsv | (word) imm;
	INC();
}

void op_xnori(word instruction) {
	IREGS();
	rdv = ~(rsv ^ (word) imm);
	INC();
}

void op_xori(word instruction) {
	IREGS();
	rdv = rsv ^ (word) imm;
	INC();
}

void op_lui(word instruction) {
	IRD(); IIMM();
	rdv = ((word) imm) << 32;
	INC();
}

void op_sl(word instruction) {
	RREGS();
	rdv = rsv < rtv;
	INC();
}

void op_sle(word instruction) {
	RREGS();
	rdv = rsv <= rtv;
	INC();
}

void op_seq(word instruction) {
	RREGS();
	rdv = rsv == rtv;
	INC();
}

void op_slu(word instruction) {
	// TODO: ???
	RREGS();
	rdv = ((uword) rsv) < (uword) rtv;
	INC();
}

void op_sleu(word instruction) {
	// Does this actually do anything differently from op_sle?
	RREGS();
	rdv = ((uword) rsv) <= (uword) rtv;
	INC();
}

void op_sli(word instruction) {
	IREGS();
	rdv = rsv < (word) imm;
	INC();
}

void op_slei(word instruction) {
	IREGS();
	rdv = rsv <= (word) imm;
	INC();
}

void op_seqi(word instruction) {
	IREGS();
	rdv = rsv == (word) imm;
	INC();
}

void op_slui(word instruction) {
	IREGS();
	rdv = ((uword) rsv) < (uword) imm;
	INC();
}

void op_sleui(word instruction) {
	IREGS();
	rdv = ((uword) rsv) <= (uword) imm;
	INC();
}

void op_j(word instruction) {
	JADDR();
	wvm_jump(addr);
}

void op_jc(word instruction) {
	JREGS();
	if (rsv)
		wvm_jump(addr);
	else INC();
}

void op_jl(word instruction) {
	JADDR();
	wvm_link();
	wvm_jump(addr);
}

void op_jlc(word instruction) {
	JREGS();
	if (rsv) {
		wvm_link();
		wvm_jump(addr);
	} else INC();
}

void op_jr(word instruction) {
	RREGS();
	wvm_jump(rdv);
}

void op_jrc(word instruction) {
	RREGS();
	if (rsv)
		wvm_jump(rdv);
	else INC();
}

void op_jrl(word instruction) {
	RREGS();
	wvm_link();
	wvm_jump(rdv);
}

void op_jrlc(word instruction) {
	RREGS();
	if (rsv) {
		wvm_link();
		wvm_jump(rdv);
	} else INC();
}

void op_c(word instruction) {
	RREGS();
	wvm_set_word(rdv, wvm_get_word(rsv));
	INC();
}

void op_l(word instruction) {
	RREGS();
	rdv = wvm_get_word(rsv);
	INC();
}

void op_s(word instruction) {
	RREGS();
	wvm_set_word(rdv, rsv);
	INC();
}

void op_cb(word instruction) {
	RREGS();
	wvm_set_byte(rdv, wvm_get_byte(rsv));
	INC();
}

void op_lb(word instruction) {
	RREGS();
	rdv = wvm_get_byte(rsv);
	INC();
}

void op_sb(word instruction) {
	RREGS();
	wvm_set_byte(rdv, rsv);
	INC();
}

void op_li(word instruction) {
	IRD(); IIMM();
	rdv = wvm_get_word(imm);
	INC();
}

void op_si(word instruction) {
	IRS(); IIMM();
	wvm_set_word(imm, rsv);
	INC();
}

void op_lbi(word instruction) {
	IRD(); IIMM();
	rdv = wvm_get_byte(imm);
	INC();
}

void op_sbi(word instruction) {
	IRS(); IIMM();
	wvm_set_byte(imm, rsv);
	INC();
}

void op_set(word instruction) {
	IRD(); IIMM();
	rdv = imm;
	INC();
}

void op_pr(word instruction) {
	RRS();
	printf("$%s: 0x%llx / %lld\n", wvm_decode_reg(rs), rsv, rsv);
	INC();
}

void op_halt(word instruction) {
	alive = 0;
}

void op_eval(word instruction) {
	// This isn't javascript.
	fprintf(stderr, "op_eval not implemented.\n");
	exit(1);
}

void op_printc(word instruction) {
	RRS();
	printf("%c", (char) rsv);
	INC();
}
