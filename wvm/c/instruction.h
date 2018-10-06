#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "ops.h"
#include "registers.h"

enum ins_type_enum {J, I, R};
typedef enum ins_type_enum ins_type;
typedef uint16_t opcode_t;
typedef uint32_t imm_t;

opcode_t wvm_get_opcode(word instruction);
ins_type wvm_get_type(opcode_t opcode);
op_fn wvm_get_fn(word instruction);

reg_t wvm_r_rt(word instruction);
reg_t wvm_r_rs(word instruction);
reg_t wvm_r_rd(word instruction);
void wvm_r_regs(word instruction, reg_t *rs, reg_t *rt, reg_t *rd);
char wvm_r_flags(word instruction);
int wvm_r_func(word instruction);

char wvm_i_flags(word instruction);
reg_t wvm_i_rs(word instruction);
reg_t wvm_i_rd(word instruction);
imm_t wvm_i_imm(word instruction);

reg_t wvm_j_rs(word instruction);
char wvm_j_flags(word instruction);
imm_t wvm_j_addr(word instruction);
char wvm_j_link(word instruction);

#endif
