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
void op_mod(word instruction);    // 1   R 9

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
void op_modi(word instruction);   // 30  I

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
void op_spush(word instruction);  // 18  R 6
void op_spop(word instruction);   // 18  R 7

void op_li(word instruction);     // 19  I
void op_si(word instruction);     // 20  I
void op_set(word instruction);    // 21  I
void op_lbi(word instruction);    // 37  I
void op_sbi(word instruction);    // 38  I
void op_lni(word instruction);    // 39  I
void op_lbni(word instruction);   // 40  I

void op_pr(word instruction);     // 31 ?R 1
void op_halt(word instruction);   // 31 ?R 2
void op_eval(word instruction);   // 31 ?R 3
void op_prc(word instruction);    // 31 ?R 4
void op_prd(word instruction);    // 31 ?R 5
void op_prx(word instruction);    // 31 ?R 6
void op_nop(word instruction);    // 0   X

#define OP_NOP    0
#define OP_ADD    1
#define OP_SUB    1
#define OP_MULT   1
#define OP_ADDU   1
#define OP_SUBU   1
#define OP_MULTU  1
#define OP_SLL    1
#define OP_SRL    1
#define OP_MOD    1
#define OP_SRA    1
#define OP_AND    2
#define OP_NAND   2
#define OP_NOR    2
#define OP_NOT    2
#define OP_OR     2
#define OP_XNOR   2
#define OP_XOR    2
#define OP_LAND   2
#define OP_LNAND  2
#define OP_LNOR   2
#define OP_LNOT   2
#define OP_LOR    2
#define OP_LXNOR  2
#define OP_LXOR   2
#define OP_ADDI   3
#define OP_SUBI   4
#define OP_MULTI  5
#define OP_ANDI   6
#define OP_NANDI  7
#define OP_NORI   8
#define OP_ORI    9
#define OP_XNORI  10
#define OP_XORI   11
#define OP_LUI    13
#define OP_SL     14
#define OP_SLE    14
#define OP_SEQ    14
#define OP_SLU    14
#define OP_SLEU   14
#define OP_SEQU   14
#define OP_J      15
#define OP_JC     16
#define OP_JR     17
#define OP_JRC    17
#define OP_JRL    17
#define OP_JRLC   17
#define OP_C      18
#define OP_L      18
#define OP_S      18
#define OP_CB     18
#define OP_LB     18
#define OP_SB     18
#define OP_SPUSH  18
#define OP_SPOP   18
#define OP_LI     19
#define OP_SI     20
#define OP_SET    21
#define OP_ADDUI  22
#define OP_SUBUI  23
#define OP_MULTUI 24
#define OP_SLI    25
#define OP_SLEI   26
#define OP_SEQI   27
#define OP_SLUI   28
#define OP_SLEUI  29
#define OP_MODI   30
#define OP_PR     31
#define OP_HALT   31
#define OP_EVAL   31
#define OP_PRC    31
#define OP_PRD    31
#define OP_PRX    31
#define OP_JL     32
#define OP_JLC    33
#define OP_SLLI   34
#define OP_SRLI   35
#define OP_SRAI   36
#define OP_LBI    37
#define OP_SBI    38
#define OP_LNI    39
#define OP_LBNI   40

#define OPS_MATH   1
#define OPS_RLOGIC 2
#define OPS_RCOMP  14
#define OPS_RJUMP  17
#define OPS_MEM    18
#define OPS_TRAP   31

#define FUNCT_ADD    0
#define FUNCT_AND    0
#define FUNCT_C      0
#define FUNCT_JR     0
#define FUNCT_SL     0
#define FUNCT_JRC    1
#define FUNCT_L      1
#define FUNCT_NAND   1
#define FUNCT_SLE    1
#define FUNCT_SUB    1
#define FUNCT_PRINTR 1
#define FUNCT_JRL    2
#define FUNCT_MULT   2
#define FUNCT_NOR    2
#define FUNCT_S      2
#define FUNCT_SEQ    2
#define FUNCT_HALT   2
#define FUNCT_ADDU   3
#define FUNCT_CB     3
#define FUNCT_JRLC   3
#define FUNCT_NOT    3
#define FUNCT_SLU    3
#define FUNCT_EVAL   3
#define FUNCT_LB     4
#define FUNCT_OR     4
#define FUNCT_SLEU   4
#define FUNCT_SUBU   4
#define FUNCT_PRC    4
#define FUNCT_PRD    5
#define FUNCT_MULTU  5
#define FUNCT_SB     5
#define FUNCT_SEQU   5
#define FUNCT_XNOR   5
#define FUNCT_PRX    6
#define FUNCT_SLL    6
#define FUNCT_XOR    6
#define FUNCT_SPUSH  6
#define FUNCT_SPOP   7
#define FUNCT_SRL    7
#define FUNCT_SRA    8
#define FUNCT_LAND   8
#define FUNCT_LNAND  9
#define FUNCT_MOD    9
#define FUNCT_LNOR   10
#define FUNCT_LNOT   11
#define FUNCT_LOR    12
#define FUNCT_LXNOR  13
#define FUNCT_LXOR   14

#endif
