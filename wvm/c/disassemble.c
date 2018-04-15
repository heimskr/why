#include <stdio.h>
#include "registers.h"
#include "instruction.h"
#include "ansi.h"
#include "disassemble.h"

char *wvm_disassemble(word instruction) {
	opcode_t opcode = wvm_get_opcode(instruction);
	char *out = calloc(255, sizeof(char));
	ins_type type = wvm_get_type(opcode);

	if (type == R) {
		wvm_disassemble_r(out, instruction);
	} else {
		sprintf(out, "???");
	}

	return out;
}

void wvm_disassemble_r(char *str, word instruction) {
	opcode_t opcode = wvm_get_opcode(instruction);
	char func = wvm_r_func(instruction);
	char *oper;
	reg_t rs, rt, rd;
	wvm_r_regs(instruction, &rs, &rt, &rd);
	char *srs = wvm_decode_reg(rs);
	char *srt = wvm_decode_reg(rt);
	char *srd = wvm_decode_reg(rd);

	switch (opcode) {
		case OPS_MATH:
			switch (func) {
				case FUNCT_ADD: oper = "+"; break;
				case FUNCT_SUB: oper = "-"; break;
				case FUNCT_MULT:
					sprintf(str, "%s$%s%s %s*%s %s$%s%s", ANSI_YELLOW, srs, ANSI_RESET, ANSI_BOLD, ANSI_RESET, ANSI_YELLOW, srt, ANSI_RESET);
					return;
				case FUNCT_SLL: oper = "<<"; break;
				case FUNCT_SRL: oper = ">>>"; break;
				case FUNCT_SRA: oper = ">>"; break;
			}

			wvm_disassemble_alt_op(str, rs, rt, rd, oper);
			break;
		case OPS_RLOGIC:
			switch (func) {
				case FUNCT_AND: oper = "&"; break;
				case FUNCT_NAND: oper = "~&"; break;
				case FUNCT_NOR: oper = "~|"; break;
				case FUNCT_XNOR: oper = "~x"; break;
				case FUNCT_XOR: oper = "x"; break;
				case FUNCT_LAND: oper = "&&"; break;
				case FUNCT_LNAND: oper = "~&&"; break;
				case FUNCT_LNOR: oper = "~||"; break;
				case FUNCT_LXNOR: oper = "~xx"; break;
				case FUNCT_LXOR: oper = "xx"; break;
				case FUNCT_OR:
				case FUNCT_LOR:
					if (rs == R_0) {
						sprintf(str, "%s$%s%s %s->%s %s$%s%s", ANSI_YELLOW, srt, ANSI_RESET, ANSI_DIM, ANSI_RESET, ANSI_YELLOW, srd, ANSI_RESET);
						return;
					}
					oper = func == FUNCT_OR? "|" : "||";
					break;
				case FUNCT_NOT:
					sprintf(str, "%s~%s%s$%s%s %s->%s %s$%s%s", ANSI_BOLD, ANSI_RESET, ANSI_YELLOW, srs, ANSI_RESET, ANSI_DIM, ANSI_RESET, ANSI_YELLOW, srd, ANSI_RESET);
					return;
				case FUNCT_LNOT:
					if (rs == rd)
						sprintf(str, "%s!%s%s$%s%s", ANSI_BOLD, ANSI_RESET, ANSI_YELLOW, srs, ANSI_RESET);
					else
						sprintf(str, "%s!%s%s$%s%s %s->%s %s$%s%s", ANSI_BOLD, ANSI_RESET, ANSI_YELLOW, srs, ANSI_RESET, ANSI_DIM, ANSI_RESET, ANSI_YELLOW, srd, ANSI_RESET);
					return;
			}

			wvm_disassemble_alt_op(str, rs, rt, rd, oper);
			break;

		case OPS_MEM:
			switch (func) {
				case FUNCT_C:
				case FUNCT_CB:
					sprintf(str, "[%s$%s%s] %s->%s [%s$%s%s]%s", ANSI_YELLOW, srs, ANSI_RESET, ANSI_DIM, ANSI_RESET, ANSI_YELLOW, srd, ANSI_RESET, func == FUNCT_CB? " /b" : "");
					return;
				case FUNCT_L:
				case FUNCT_LB:
					sprintf(str, "[%s$%s%s] %s->%s %s$%s%s%s", ANSI_YELLOW, srs, ANSI_RESET, ANSI_DIM, ANSI_RESET, ANSI_YELLOW, srd, ANSI_RESET, func == FUNCT_LB? " /b" : "");
					return;
				case FUNCT_S:
				case FUNCT_SB:
					sprintf(str, "%s$%s%s %s->%s [%s$%s%s]%s", ANSI_YELLOW, srs, ANSI_RESET, ANSI_DIM, ANSI_RESET, ANSI_YELLOW, srd, ANSI_RESET, func == FUNCT_SB? " /b" : "");
					return;
			}

		case OPS_TRAP:
			sprintf(str, "<%s", ANSI_CYAN);
			switch (func) {
				case FUNCT_PRINTR:
					sprintf(str, "%sprint%s %s%s", str, ANSI_RESET, ANSI_YELLOW, srs);
					break;
				case FUNCT_HALT:
					sprintf(str, "%shalt", str);
					break;
				case FUNCT_EVAL:
					sprintf(str, "%seval", str);
					break;
				case FUNCT_PRINTC:
					sprintf(str, "%sprintc%s %s%s", str, ANSI_RESET, ANSI_YELLOW, srs);
					break;
			}

			sprintf(str, "%s%s>", str, ANSI_RESET);
			return;
	}

	sprintf(str, "(R)");
}

void wvm_disassemble_alt_op(char *str, reg_t rs, reg_t rt, reg_t rd, char *oper) {
	char *srs = wvm_decode_reg(rs);
	char *srt = wvm_decode_reg(rt);
	char *srd = wvm_decode_reg(rd);

	if (rs == rd || rt == rd) {
		char *src, *dest;
		if (rs == rd) {
			src = srs;
			dest = srt;
		} else {
			src = srt;
			dest = srs;
		}

		sprintf(str, "%s$%s%s %s%s=%s %s$%s%s", ANSI_YELLOW, src, ANSI_RESET, ANSI_BOLD, oper, ANSI_RESET, ANSI_YELLOW, dest, ANSI_RESET);
	} else {
		sprintf(str, "%s$%s%s %s%s%s %s$%s%s %s->%s %s$%s%s",
			ANSI_YELLOW, srs, ANSI_RESET,
			ANSI_BOLD, oper, ANSI_RESET,
			ANSI_YELLOW, srt, ANSI_RESET,
			ANSI_DIM, ANSI_RESET,
			ANSI_YELLOW, srd, ANSI_RESET);
	}
}
