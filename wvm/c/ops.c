/***********************************************
 * Contains functions that execute operations. *
 ***********************************************/

#include <stdio.h>
#include <unistd.h>
#include "ops.h"
#include "instruction.h"
#include "interrupts.h"

void op_nop(word instruction) {
	INC();
}

void op_add(word instruction) {
	RREGS();
	wvm_alu_flags_update(rdv = rsv + rtv);
	if (rdv < rsv) {
		FLAG_O_SET(1);
	}

	INC();
}

void op_sub(word instruction) {
	RREGS();
	wvm_alu_flags_update(rdv = rsv - rtv);
	INC();
}

void op_mult(word instruction) {
	// TODO: catching overflow is hard.
	INC();
}

void op_multu(word instruction) {
	op_mult(instruction);
}

void op_mod(word instruction) {
	RREGS();
	wvm_alu_flags_update(rdv = rsv % rtv);
	INC();
}

void op_sll(word instruction) {
	RREGS();
	wvm_alu_flags_update(rdv = rsv << rtv);
	INC();
}

void op_srl(word instruction) {
	RREGS();
	wvm_alu_flags_update(rdv = ((uword) rsv) >> (uword) rtv);
	INC();
}

void op_sra(word instruction) {
	RREGS();
	wvm_alu_flags_update(rdv = rsv >> rtv);
	INC();
}

void op_and(word instruction) {
	RREGS();
	wvm_alu_flags_update(rdv = rsv & rtv);
	INC();
}

void op_nand(word instruction) {
	RREGS();
	wvm_alu_flags_update(rdv = ~(rsv & rtv));
	INC();
}

void op_nor(word instruction) {
	RREGS();
	wvm_alu_flags_update(rdv = ~(rsv | rtv));
	INC();
}

void op_not(word instruction) {
	RREGS();
	wvm_alu_flags_update(rdv = ~rsv);
	INC();
}

void op_or(word instruction) {
	RREGS();
	wvm_alu_flags_update(rdv = rsv | rtv);
	INC();
}

void op_xnor(word instruction) {
	RREGS();
	wvm_alu_flags_update(rdv = ~(rsv ^ rtv));
	INC();
}

void op_xor(word instruction) {
	RREGS();
	wvm_alu_flags_update(rdv = rsv ^ rtv);
	INC();
}

void op_land(word instruction) {
	RREGS();
	wvm_alu_flags_update(rdv = rsv && rtv);
	INC();
}

void op_lnand(word instruction) {
	RREGS();
	wvm_alu_flags_update(rdv = !(rsv && rtv));
	INC();
}

void op_lnor(word instruction) {
	RREGS();
	wvm_alu_flags_update(rdv = !(rsv || rtv));
	INC();
}

void op_lnot(word instruction) {
	RREGS();
	wvm_alu_flags_update(rdv = !rsv);
	INC();
}

void op_lor(word instruction) {
	RREGS();
	wvm_alu_flags_update(rdv = rsv || rtv);
	INC();
}

void op_lxnor(word instruction) {
	RREGS();
	wvm_alu_flags_update(rdv = !(rsv ^ rtv));
	INC();
}

void op_lxor(word instruction) {
	RREGS();
	wvm_alu_flags_update(rdv = !!(rsv ^ rtv));
	INC();
}

void op_addi(word instruction) {
	IREGS();
	wvm_alu_flags_update(rdv = rsv + (word) imm);
	INC();
}

void op_subi(word instruction) {
	IREGS();
	wvm_alu_flags_update(rdv = rsv - (word) imm);
	INC();
}

void op_multi(word instruction) {
	// TODO: see op_mult.
	INC();
}

void op_multui(word instruction) {
	// TODO
	INC();
}

void op_slli(word instruction) {
	IREGS();
	wvm_alu_flags_update(rdv = rsv << (word) imm);
	INC();
}

void op_srli(word instruction) {
	IREGS();
	wvm_alu_flags_update(rdv = ((uword) rsv) >> (uword) imm);
	INC();
}

void op_srai(word instruction) {
	IREGS();
	wvm_alu_flags_update(rdv = rsv >> (word) imm);
	INC();
}

void op_modi(word instruction) {
	IREGS();
	wvm_alu_flags_update(rdv = rsv % (word) imm);
	INC();
}

void op_andi(word instruction) {
	IREGS();
	wvm_alu_flags_update(rdv = rsv & (word) imm);
	INC();
}

void op_nandi(word instruction) {
	IREGS();
	wvm_alu_flags_update(rdv = ~(rsv & (word) imm));
	INC();
}

void op_nori(word instruction) {
	IREGS();
	wvm_alu_flags_update(rdv = ~(rsv | (word) imm));
	INC();
}

void op_ori(word instruction) {
	IREGS();
	wvm_alu_flags_update(rdv = rsv | (word) imm);
	INC();
}

void op_xnori(word instruction) {
	IREGS();
	wvm_alu_flags_update(rdv = ~(rsv ^ (word) imm));
	INC();
}

void op_xori(word instruction) {
	IREGS();
	wvm_alu_flags_update(rdv = rsv ^ (word) imm);
	INC();
}

void op_lui(word instruction) {
	IRD(); IIMM();
	rdv = (((word) imm) << 32) | (rdv & 0xfffffff);
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

void op_sgi(word instruction) {
	IREGS();
	rdv = rsv > (word) imm;
	INC();
}

void op_sgei(word instruction) {
	IREGS();
	rdv = rsv >= (word) imm;
	INC();
}

void op_j(word instruction) {
	JADDR(); JLINK(); JCOND();
	if (wvm_check_condition(cond)) {
		if (link)
			wvm_link();
		wvm_jump(addr);
	} else INC();
}

void op_jc(word instruction) {
	JREGS(); JLINK();
	if (rsv) {
		if (link)
			wvm_link();
		wvm_jump(addr);
	} else INC();
}

void op_jr(word instruction) {
	RREGS(); RCOND();
	if (wvm_check_condition(cond))
		wvm_jump(rdv);
	else INC();
}

void op_jrc(word instruction) {
	RREGS();
	if (rsv)
		wvm_jump(rdv);
	else INC();
}

void op_jrl(word instruction) {
	RREGS(); RCOND();
	if (wvm_check_condition(cond)) {
		wvm_link();
		wvm_jump(rdv);
	} else INC();
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

void op_spush(word instruction) {
	RREGS();
	wvm_set_word(spv, rsv);
	spv -= 8;
	INC();
}

void op_spop(word instruction) {
	RREGS();
	spv += 8;
	rdv = wvm_get_word(spv);
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

void op_set(word instruction) {
	IRD(); IIMM();
	rdv = imm;
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

void op_lni(word instruction) {
	IRD(); IIMM();
	wvm_set_word(rdv, wvm_get_word(imm));
	INC();
}

void op_lbni(word instruction) {
	IRD(); IIMM();
	wvm_set_byte(rdv, wvm_get_byte(imm));
	INC();
}

void op_int(word instruction) {
	IIMM();
	if (wvm_interrupt(imm)) {
		INC();
	}
}

void op_rit(word instruction) {
	if (check_kernel()) {
		IIMM();
		inttab = imm;
		INC();
	}
}

void op_time(word instruction) {
	fprintf(stderr, "op_time not implemented.\n");
	exit(1);
}

void op_timei(word instruction) {
	fprintf(stderr, "op_timei not implemented.\n");
	exit(1);
}

void op_ring(word instruction) {
	RRS();
	if (wvm_change_ring(rsv)) {
		INC();
	}
}

void op_ringi(word instruction) {
	IIMM();
	if (wvm_change_ring(imm)) {
		INC();
	}
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

void op_prc(word instruction) {
	RRS();
	putchar((char) rsv);
	INC();
}

void op_prd(word instruction) {
	RRS();
	printf("%lld", rsv);
	INC();
}

void op_prx(word instruction) {
	RRS();
	printf("%llx", rsv);
	INC();
}

void op_sleep(word instruction) {
	RRS();
	usleep(rsv * 1000);
	INC();
}
