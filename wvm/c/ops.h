#ifndef __OPS_H__
#define __OPS_H__

#include "wvm.h"

typedef void (*op_fn)(word);

void op_nop(word instruction);    // 0   X

void op_add(word instruction);    // 1   R 0
void op_sub(word instruction);    // 1   R 1
void op_mult(word instruction);   // 1   R 2
void op_addu(word instruction);   // 1   R 3
void op_subu(word instruction);   // 1   R 4
void op_multu(word instruction);  // 1   R 5
void op_sll(word instruction);    // 1   R 6
void op_srl(word instruction);    // 1   R 7
void op_sra(word instruction);    // 1   R 8

void op_and(word instruction);    // 2   R 0
void op_nand(word instruction);   // 2   R 1
void op_nor(word instruction);    // 2   R 2
void op_not(word instruction);    // 2   R 3
void op_or(word instruction);     // 2   R 4
void op_xnor(word instruction);   // 2   R 5
void op_xor(word instruction);    // 2   R 6
void op_land(word instruction);   // 2   R 8
void op_lnand(word instruction);  // 2   R 9
void op_lnor(word instruction);   // 2   R 10
void op_lnot(word instruction);   // 2   R 11
void op_lor(word instruction);    // 2   R 12
void op_lxnor(word instruction);  // 2   R 13
void op_lxor(word instruction);   // 2   R 14

void op_addi(word instruction);   // 3   I
void op_subi(word instruction);   // 4   I
void op_multi(word instruction);  // 5   I
void op_addui(word instruction);  // 22  I
void op_subui(word instruction);  // 23  I
void op_multui(word instruction); // 24  I
void op_slli(word instruction);   // 34  I
void op_srli(word instruction);   // 35  I
void op_srai(word instruction);   // 36  I

void op_andi(word instruction);   // 6   I
void op_nandi(word instruction);  // 7   I
void op_nori(word instruction);   // 8   I
void op_ori(word instruction);    // 9   I
void op_xnori(word instruction);  // 10  I
void op_xori(word instruction);   // 11  I

void op_lui(word instruction);    // 13  I
void op_sl(word instruction);     // 14  R 0
void op_sle(word instruction);    // 14  R 1
void op_seq(word instruction);    // 14  R 2
void op_slu(word instruction);    // 14  R 3
void op_sleu(word instruction);   // 14  R 4

void op_sli(word instruction);    // 25  I
void op_slei(word instruction);   // 26  I
void op_seqi(word instruction);   // 27  I
void op_slui(word instruction);   // 28  I
void op_sleui(word instruction);  // 29  I

void op_j(word instruction);      // 15  J
void op_jc(word instruction);     // 16  J
void op_jl(word instruction);     // 32  J
void op_jlc(word instruction);    // 33  J

void op_jr(word instruction);     // 17  R 0
void op_jrc(word instruction);    // 17  R 1
void op_jrl(word instruction);    // 17  R 2
void op_jrlc(word instruction);   // 17  R 3

void op_c(word instruction);      // 18  R 0
void op_l(word instruction);      // 18  R 1
void op_s(word instruction);      // 18  R 2
void op_cb(word instruction);     // 18  R 3
void op_lb(word instruction);     // 18  R 4
void op_sb(word instruction);     // 18  R 5

void op_li(word instruction);     // 19  I
void op_si(word instruction);     // 20  I
void op_lbi(word instruction);    // 37  I
void op_sbi(word instruction);    // 38  I
void op_set(word instruction);    // 21  I

void op_pr(word instruction);     // 31 ?R 1
void op_halt(word instruction);   // 31 ?R 2
void op_eval(word instruction);   // 31 ?R 3
void op_printc(word instruction); // 31 ?R 4

#endif
