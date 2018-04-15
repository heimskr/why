#ifndef __DISASSEMBLE_H__
#define __DISASSEMBLE_H__

#include "wvm.h"

char *wvm_disassemble(word instruction);
void wvm_disassemble_r(char *str, word instruction);
void wvm_disassemble_alt_op(char *str, reg_t rs, reg_t rt, reg_t rd, char *oper);

#endif
