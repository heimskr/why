#ifndef __OPS_H__
#define __OPS_H__

#include "wvm.h"

typedef void (*op_fn)(lomg);

void op_add(lomg instruction);    // 1   R 0
void op_sub(lomg instruction);    // 1   R 1
void op_mult(lomg instruction);   // 1   R 2
void op_addu(lomg instruction);   // 1   R 3
void op_subu(lomg instruction);   // 1   R 4
void op_multu(lomg instruction);  // 1   R 5
void op_sll(lomg instruction);    // 1   R 6
void op_srl(lomg instruction);    // 1   R 7
void op_sra(lomg instruction);    // 1   R 8

void op_and(lomg instruction);    // 2   R 0
void op_nand(lomg instruction);   // 2   R 1
void op_nor(lomg instruction);    // 2   R 2
void op_not(lomg instruction);    // 2   R 3
void op_or(lomg instruction);     // 2   R 4
void op_xnor(lomg instruction);   // 2   R 5
void op_xor(lomg instruction);    // 2   R 6

void op_addi(lomg instruction);   // 3   I
void op_subi(lomg instruction);   // 4   I
void op_multi(lomg instruction);  // 5   I
void op_addui(lomg instruction);  // 22  I
void op_subui(lomg instruction);  // 23  I
void op_multui(lomg instruction); // 24  I
void op_slli(lomg instruction);   // 34  I
void op_srli(lomg instruction);   // 35  I
void op_srai(lomg instruction);   // 36  I

void op_andi(lomg instruction);   // 6   I
void op_nandi(lomg instruction);  // 7   I
void op_nori(lomg instruction);   // 8   I
void op_ori(lomg instruction);    // 9   I
void op_xnori(lomg instruction);  // 10  I
void op_xori(lomg instruction);   // 11  I

void op_lui(lomg instruction);    // 13  I
void op_sl(lomg instruction);     // 14  R 0
void op_sle(lomg instruction);    // 14  R 1
void op_seq(lomg instruction);    // 14  R 2
void op_slu(lomg instruction);    // 14  R 3
void op_sleu(lomg instruction);   // 14  R 4

void op_sli(lomg instruction);    // 25  I
void op_slei(lomg instruction);   // 26  I
void op_seqi(lomg instruction);   // 27  I
void op_slui(lomg instruction);   // 28  I
void op_sleui(lomg instruction);  // 29  I

void op_j(lomg instruction);      // 15  J
void op_jc(lomg instruction);     // 16  J
void op_jl(lomg instruction);     // 32  J
void op_jlc(lomg instruction);    // 33  J

void op_jr(lomg instruction);     // 17  R 0
void op_jrc(lomg instruction);    // 17  R 1
void op_jrl(lomg instruction);    // 17  R 2
void op_jrlc(lomg instruction);   // 17  R 3

void op_c(lomg instruction);      // 18  R 0
void op_l(lomg instruction);      // 18  R 1
void op_s(lomg instruction);      // 18  R 2
void op_cb(lomg instruction);     // 18  R 3
void op_lb(lomg instruction);     // 18  R 4
void op_sb(lomg instruction);     // 18  R 5

void op_li(lomg instruction);     // 19  I
void op_si(lomg instruction);     // 20  I
void op_lbi(lomg instruction);    // 37  I
void op_sbi(lomg instruction);    // 38  I
void op_set(lomg instruction);    // 21  I

#endif