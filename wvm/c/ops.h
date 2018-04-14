#ifndef __OPS_H__
#define __OPS_H__

typedef void (*op_fn)(lomg);

void op_add(lomg);    // 1   R 0
void op_sub(lomg);    // 1   R 1
void op_mult(lomg);   // 1   R 2
void op_addu(lomg);   // 1   R 3
void op_subu(lomg);   // 1   R 4
void op_multu(lomg);  // 1   R 5
void op_sll(lomg);    // 1   R 6
void op_srl(lomg);    // 1   R 7
void op_sra(lomg);    // 1   R 8

void op_and(lomg);    // 2   R 0
void op_nand(lomg);   // 2   R 1
void op_nor(lomg);    // 2   R 2
void op_not(lomg);    // 2   R 3
void op_or(lomg);     // 2   R 4
void op_xnor(lomg);   // 2   R 5
void op_xor(lomg);    // 2   R 6

void op_addi(lomg);   // 3   I
void op_subi(lomg);   // 4   I
void op_multi(lomg);  // 5   I
void op_addui(lomg);  // 22  I
void op_subui(lomg);  // 23  I
void op_multui(lomg); // 24  I
void op_slli(lomg);   // 34  I
void op_srli(lomg);   // 35  I
void op_srai(lomg);   // 36  I

void op_andi(lomg);   // 6   I
void op_nandi(lomg);  // 7   I
void op_nori(lomg);   // 8   I
void op_ori(lomg);    // 9   I
void op_xnori(lomg);  // 10  I
void op_xori(lomg);   // 11  I

void op_lui(lomg);    // 13  I
void op_sl(lomg);     // 14  R 0
void op_sle(lomg);    // 14  R 1
void op_seq(lomg);    // 14  R 2
void op_slu(lomg);    // 14  R 3
void op_sleu(lomg);   // 14  R 4

void op_sli(lomg);    // 25  I
void op_slei(lomg);   // 26  I
void op_seqi(lomg);   // 27  I
void op_slui(lomg);   // 28  I
void op_sleui(lomg);  // 29  I

void op_j(lomg);      // 15  J
void op_jc(lomg);     // 16  J
void op_jl(lomg);     // 32  J
void op_jlc(lomg);    // 33  J

void op_jr(lomg);     // 17  R 0
void op_jrc(lomg);    // 17  R 1
void op_jrl(lomg);    // 17  R 2
void op_jrlc(lomg);   // 17  R 3

void op_c(lomg);      // 18  R 0
void op_l(lomg);      // 18  R 1
void op_s(lomg);      // 18  R 2
void op_cb(lomg);     // 18  R 3
void op_lb(lomg);     // 18  R 4
void op_sb(lomg);     // 18  R 5

void op_li(lomg);     // 19  I
void op_si(lomg);     // 20  I
void op_lbi(lomg);    // 37  I
void op_sbi(lomg);    // 38  I
void op_set(lomg);    // 21  I

#endif