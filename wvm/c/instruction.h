#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "ops.h"
#include "registers.h"

enum ins_type_enum {J, I, R};
typedef enum ins_type_enum ins_type;
typedef uint16_t opcode_t;
typedef uint32_t imm_t;

opcode_t wvm_get_opcode(lomg instruction);
ins_type wvm_get_type(opcode_t opcode);
op_fn wvm_get_fn(lomg instruction);

reg_t wvm_r_rt(lomg instruction);
reg_t wvm_r_rs(lomg instruction);
reg_t wvm_r_rd(lomg instruction);
char wvm_r_flags(lomg instruction);
char wvm_r_func(lomg instruction);

char wvm_i_flags(lomg instruction);
reg_t wvm_i_rs(lomg instruction);
reg_t wvm_i_rd(lomg instruction);
imm_t wvm_i_imm(lomg instruction);

reg_t wvm_j_rs(lomg instruction);
char wvm_j_flags(lomg instruction);
imm_t wvm_j_addr(lomg instruction);

#endif