#ifndef DISASSEMBLE_H_
#define DISASSEMBLE_H_

#include "wvm.h"
#include "instruction.h"
#include "registers.h"

#define COLOR_IMM "\33[38;5;164m"
#define COLOR_IF "\33[38;5;90m"
#define COLOR_REG "\33[38;5;226m"

char * wvm_disassemble(word instruction);
void wvm_disassemble_r(char *str, word instruction);
void wvm_disassemble_r_alt_op(char *str, reg_t rs, reg_t rt, reg_t rd, char *oper, char *suffix);
void wvm_disassemble_i(char *str, word instruction);
void wvm_disassemble_i_comp(char *str, reg_t rs, reg_t rd, imm_t imm, char *oper);
void wvm_disassemble_i_alt_op(char *str, reg_t rs, reg_t rd, imm_t imm, char *oper);
void wvm_disassemble_i_math(char *str, reg_t rs, reg_t rd, imm_t imm, char oper);
void wvm_disassemble_j(char *str, word instruction);

#endif
