#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "ops.h"

enum ins_type_enum {J, I, R};
typedef enum ins_type_enum ins_type;
typedef uint16_t opcode_t;

opcode_t wvm_get_opcode(lomg instruction);
ins_type wvm_get_type(opcode_t opcode);
op_fn wvm_get_fn(lomg instruction);

#endif