/************************************************
 * Contains functions for converting assembled  *
 * instructions back into text representations. *
 ************************************************/

#include <stdio.h>
#include <string.h>
#include "registers.h"
#include "instruction.h"
#include "ansi.h"
#include "disassemble.h"
#include "interrupts.h"

char * wvm_disassemble(word instruction) {
	opcode_t opcode = wvm_get_opcode(instruction);

	char *out = calloc(255, sizeof(char));

	if (instruction == 0) {
		sprintf(out, "%sNOP%s", ANSI_DIM, ANSI_RESET);
		return out;
	}

	ins_type type = wvm_get_type(opcode);

	if (type == R) {
		wvm_disassemble_r(out, instruction);
	} else if (type == I) {
		wvm_disassemble_i(out, instruction);
	} else if (type == J) {
		wvm_disassemble_j(out, instruction);
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
		case OPS_RMATH:
			switch (func) {
				case FUNCT_ADD: oper = "+"; break;
				case FUNCT_SUB: oper = "-"; break;
				case FUNCT_MULT:
					sprintf(str, "%s$%s%s %s*%s %s$%s%s", COLOR_REG, srs, ANSI_RESET, ANSI_BOLD, ANSI_RESET, COLOR_REG, srt, ANSI_RESET);
					return;
				case FUNCT_SLL: oper = "<<";  break;
				case FUNCT_SRL: oper = ">>>"; break;
				case FUNCT_SRA: oper = ">>";  break;
				case FUNCT_MOD: oper = "%";   break;
			}

			wvm_disassemble_r_alt_op(str, rs, rt, rd, oper);
			return;

		case OPS_RLOGIC:
			switch (func) {
				case FUNCT_AND:   oper = "&";   break;
				case FUNCT_NAND:  oper = "~&";  break;
				case FUNCT_NOR:   oper = "~|";  break;
				case FUNCT_XNOR:  oper = "~x";  break;
				case FUNCT_XOR:   oper = "x";   break;
				case FUNCT_LAND:  oper = "&&";  break;
				case FUNCT_LNAND: oper = "~&&"; break;
				case FUNCT_LNOR:  oper = "~||"; break;
				case FUNCT_LXNOR: oper = "~xx"; break;
				case FUNCT_LXOR:  oper = "xx";  break;
				case FUNCT_OR:
				case FUNCT_LOR:
					if (rs == R_0) {
						sprintf(str, "%s$%s%s %s->%s %s$%s%s", COLOR_REG, srt, ANSI_RESET, ANSI_DIM, ANSI_RESET, COLOR_REG, srd, ANSI_RESET);
						return;
					}
					oper = func == FUNCT_OR? "|" : "||";
					break;
				case FUNCT_NOT:
					sprintf(str, "%s~%s%s$%s%s %s->%s %s$%s%s", ANSI_BOLD, ANSI_RESET, COLOR_REG, srs, ANSI_RESET, ANSI_DIM, ANSI_RESET, COLOR_REG, srd, ANSI_RESET);
					return;
				case FUNCT_LNOT:
					if (rs == rd)
						sprintf(str, "%s!%s%s$%s%s", ANSI_BOLD, ANSI_RESET, COLOR_REG, srs, ANSI_RESET);
					else
						sprintf(str, "%s!%s%s$%s%s %s->%s %s$%s%s", ANSI_BOLD, ANSI_RESET, COLOR_REG, srs, ANSI_RESET, ANSI_DIM, ANSI_RESET, COLOR_REG, srd, ANSI_RESET);
					return;
			}

			wvm_disassemble_r_alt_op(str, rs, rt, rd, oper);
			return;

		case OPS_RCOMP:
			sprintf(str, "%s$%s%s %s", COLOR_REG, srs, ANSI_RESET, ANSI_BOLD);
			if (func ==  FUNCT_SL || func ==  FUNCT_SLU)   strcat(str, "<");
			if (func ==  FUNCT_SLE || func ==  FUNCT_SLEU) strcat(str, "<=");
			if (func ==  FUNCT_SEQ || func ==  FUNCT_SEQU) strcat(str, "==");
			sprintf(str + strlen(str), "%s %s$%s%s %s->%s %s$%s%s", ANSI_RESET, COLOR_REG, srt, ANSI_RESET, ANSI_DIM, ANSI_RESET, COLOR_REG, srd, ANSI_RESET);
			if (func == FUNCT_SLU || func == FUNCT_SLEU || func == FUNCT_SEQU)
				strcat(str, " /u");
			return;

		case OPS_RJUMP:
			sprintf(str, "%s:", ANSI_DIM);
			if (func == FUNCT_JRL || func == FUNCT_JRLC)
				strcat(str, ":");
			sprintf(str, "%s%s %s$%s%s", str, ANSI_RESET, COLOR_REG, srd, ANSI_RESET);
			if (func == FUNCT_JRC || func == FUNCT_JRLC)
				sprintf(str + strlen(str), " %sif%s %s$%s%s", ANSI_RED, ANSI_RESET, COLOR_REG, srs, ANSI_RESET);
			return;

		case OPS_MEM:
			switch (func) {
				case FUNCT_C:
				case FUNCT_CB:
					sprintf(str, "[%s$%s%s] %s->%s [%s$%s%s]%s", COLOR_REG, srs, ANSI_RESET, ANSI_DIM, ANSI_RESET, COLOR_REG, srd, ANSI_RESET, func == FUNCT_CB? " /b" : "");
					return;
				case FUNCT_L:
				case FUNCT_LB:
					sprintf(str, "[%s$%s%s] %s->%s %s$%s%s%s", COLOR_REG, srs, ANSI_RESET, ANSI_DIM, ANSI_RESET, COLOR_REG, srd, ANSI_RESET, func == FUNCT_LB? " /b" : "");
					return;
				case FUNCT_S:
				case FUNCT_SB:
					sprintf(str, "%s$%s%s %s->%s [%s$%s%s]%s", COLOR_REG, srs, ANSI_RESET, ANSI_DIM, ANSI_RESET, COLOR_REG, srd, ANSI_RESET, func == FUNCT_SB? " /b" : "");
					return;
				case FUNCT_SPUSH:
					sprintf(str, "[ %s$%s%s", COLOR_REG, srs, ANSI_RESET);
					return;
				case FUNCT_SPOP:
					sprintf(str, "  %s$%s%s ]", COLOR_REG, srd, ANSI_RESET);
					return;
			}

		case OPS_EXT:
			sprintf(str, "<%s", ANSI_CYAN);
			switch (func) {
				case FUNCT_PRINTR:
					sprintf(str + strlen(str), "print%s %s$%s", ANSI_RESET, COLOR_REG, srs);
					break;
				case FUNCT_HALT:  sprintf(str + strlen(str), "halt"); break;
				case FUNCT_EVAL:  sprintf(str + strlen(str), "eval"); break;
				case FUNCT_PRC:   sprintf(str + strlen(str), "prc%s %s$%s",   ANSI_RESET, COLOR_REG, srs); break;
				case FUNCT_PRD:   sprintf(str + strlen(str), "prd%s %s$%s",   ANSI_RESET, COLOR_REG, srs); break;
				case FUNCT_PRX:   sprintf(str + strlen(str), "prx%s %s$%s",   ANSI_RESET, COLOR_REG, srs); break;
				case FUNCT_SLEEP: sprintf(str + strlen(str), "sleep%s %s$%s", ANSI_RESET, COLOR_REG, srs); break;
				default: sprintf(str, "%s(Unknown external)%s", ANSI_DIM, ANSI_RESET); return;
			}

			sprintf(str + strlen(str), "%s>", ANSI_RESET);
			return;

		case OP_TIME:
			sprintf(str, "%stime %s%s$%s%s", ANSI_CYAN, ANSI_RESET, COLOR_REG, srs, ANSI_RESET);
			return;

		case OP_RING:
			sprintf(str, "%sring %s%s$%s%s", ANSI_CYAN, ANSI_RESET, COLOR_REG, srs, ANSI_RESET);
			return;
	}

	sprintf(str, "%s(Unknown R-type)%s", ANSI_DIM, ANSI_RESET);
}

void wvm_disassemble_r_alt_op(char *str, reg_t rs, reg_t rt, reg_t rd, char *oper) {
	char *srs = wvm_decode_reg(rs);
	char *srt = wvm_decode_reg(rt);
	char *srd = wvm_decode_reg(rd);

	if (rs == rd || rt == rd) {
		char *src  = rs == rd? srs : srt;
		char *dest = rs == rd? srt : srs;
		sprintf(str, "%s$%s%s %s%s=%s %s$%s%s", COLOR_REG, src, ANSI_RESET, ANSI_BOLD, oper, ANSI_RESET, COLOR_REG, dest, ANSI_RESET);
	} else {
		sprintf(str, "%s$%s%s %s%s%s %s$%s%s %s->%s %s$%s%s", COLOR_REG, srs, ANSI_RESET, ANSI_BOLD, oper, ANSI_RESET, COLOR_REG, srt, ANSI_RESET, ANSI_DIM, ANSI_RESET, COLOR_REG, srd, ANSI_RESET);
	}
}

void wvm_disassemble_i(char *str, word instruction) {
	opcode_t opcode = wvm_get_opcode(instruction);
	reg_t rs = wvm_i_rs(instruction);
	reg_t rd = wvm_i_rd(instruction);
	imm_t imm = wvm_i_imm(instruction);

	switch (opcode) {
		case OP_ADDI:
			wvm_disassemble_i_math(str, rs, rd, imm, '+');
			break;
		case OP_SUBI:
			wvm_disassemble_i_math(str, rs, rd, imm, '-');
			break;
		case OP_MULTI:
		case OP_MULTUI:
			sprintf(str, "%s$%s%s %s*%s %s%d%s", COLOR_REG, wvm_decode_reg(rs), ANSI_RESET, ANSI_BOLD, ANSI_RESET, COLOR_IMM, imm, ANSI_RESET);
			break;
		case OP_MODI:  wvm_disassemble_i_alt_op(str, rs, rd, imm, "%");   break;
		case OP_ANDI:  wvm_disassemble_i_alt_op(str, rs, rd, imm, "&");   break;
		case OP_NANDI: wvm_disassemble_i_alt_op(str, rs, rd, imm, "~&");  break;
		case OP_NORI:  wvm_disassemble_i_alt_op(str, rs, rd, imm, "~|");  break;
		case OP_ORI:   wvm_disassemble_i_alt_op(str, rs, rd, imm, "|");   break;
		case OP_XNORI: wvm_disassemble_i_alt_op(str, rs, rd, imm, "~x");  break;
		case OP_XORI:  wvm_disassemble_i_alt_op(str, rs, rd, imm, "x");   break;
		case OP_SLLI:  wvm_disassemble_i_alt_op(str, rs, rd, imm, "<<");  break;
		case OP_SRLI:  wvm_disassemble_i_alt_op(str, rs, rd, imm, ">>>"); break;
		case OP_SRAI:  wvm_disassemble_i_alt_op(str, rs, rd, imm, ">>");  break;
		case OP_LUI:
			sprintf(str, "%slui:%s %s%d%s %s->%s %s$%s%s", ANSI_DIM, ANSI_RESET, COLOR_IMM, imm, ANSI_RESET, ANSI_DIM, ANSI_RESET, COLOR_REG, wvm_decode_reg(rd), ANSI_RESET);
			break;
		case OP_LI:
		case OP_LBI:
			sprintf(str, "[%s%d%s] %s->%s %s$%s%s", COLOR_IMM, imm, ANSI_RESET, ANSI_DIM, ANSI_RESET, COLOR_REG, wvm_decode_reg(rd), ANSI_RESET);
			break;
		case OP_LNI:
		case OP_LBNI:
			sprintf(str, "[%s%d%s] %s->%s [%s$%s%s]", COLOR_IMM, imm, ANSI_RESET, ANSI_DIM, ANSI_RESET, COLOR_REG, wvm_decode_reg(rd), ANSI_RESET);
			break;
		case OP_SI:
		case OP_SBI:
			sprintf(str, "%s$%s%s %s->%s [%s%d%s]", COLOR_REG, wvm_decode_reg(rs), ANSI_RESET, ANSI_DIM, ANSI_RESET, COLOR_IMM, imm, ANSI_RESET);
			break;
		case OP_SET:
			sprintf(str, "%s%d%s %s->%s %s$%s%s", COLOR_IMM, imm, ANSI_RESET, ANSI_DIM, ANSI_RESET, COLOR_REG, wvm_decode_reg(rd), ANSI_RESET);
			break;
		case OP_SLI:
		case OP_SLUI:
			wvm_disassemble_i_comp(str, rs, rd, imm, "<");
			break;
		case OP_SLEI:
		case OP_SLEUI:
			wvm_disassemble_i_comp(str, rs, rd, imm, "<=");
			break;
		case OP_SGI:
			wvm_disassemble_i_comp(str, rs, rd, imm, ">");
			break;
		case OP_SGEI:
			wvm_disassemble_i_comp(str, rs, rd, imm, ">=");
			break;
		case OP_SEQI:
			wvm_disassemble_i_comp(str, rs, rd, imm, "==");
			break;
		case OP_INT:
			sprintf(str, "%sint%s ", ANSI_CYAN, ANSI_RESET);
			switch (imm) {
				case INT_NULL:   sprintf(str + strlen(str), "null");   return;
				case INT_SYSTEM: sprintf(str + strlen(str), "system"); return;
				case INT_TIMER:  sprintf(str + strlen(str), "timer");  return;
				case INT_PROTEC: sprintf(str + strlen(str), "protec"); return;
				default: sprintf(str + strlen(str), "???"); return;
			}
		case OP_RIT:
			sprintf(str, "%srit%s %s%d%s", ANSI_CYAN, ANSI_RESET, COLOR_IMM, imm, ANSI_RESET);
			break;
		case OP_TIMEI:
			sprintf(str, "%stime%s %s%d%s", ANSI_CYAN, ANSI_RESET, COLOR_IMM, imm, ANSI_RESET);
			break;
		case OP_RINGI:
			sprintf(str, "%sring%s %s%d%s", ANSI_CYAN, ANSI_RESET, COLOR_IMM, imm, ANSI_RESET);
			break;
		default:
			sprintf(str, "%s(Unknown I-type)%s", ANSI_DIM, ANSI_RESET);
			break;
	}

	if (opcode == OP_MULTUI || opcode == OP_SLUI || opcode == OP_SLEUI)
		strcat(str, " /u");
	else if (opcode == OP_LBI || opcode == OP_SBI)
		strcat(str, " /b");
}

void wvm_disassemble_i_comp(char *str, reg_t rs, reg_t rd, imm_t imm, char *oper) {
	sprintf(str, "%s$%s%s %s%s%s %s%d%s %s->%s %s$%s%s", COLOR_REG, wvm_decode_reg(rs), ANSI_RESET, ANSI_BOLD, oper, ANSI_RESET, COLOR_IMM, imm, ANSI_RESET, ANSI_DIM, ANSI_RESET, COLOR_REG, wvm_decode_reg(rd), ANSI_RESET);
}

void wvm_disassemble_i_alt_op(char *str, reg_t rs, reg_t rd, imm_t imm, char *oper) {
	sprintf(str, "%s$%s%s %s%s%s%s %s%d%s", COLOR_REG, wvm_decode_reg(rs), ANSI_RESET, ANSI_BOLD, oper, rs == rd? "=" : "", ANSI_RESET, COLOR_IMM, imm, ANSI_RESET);
	if (rs != rd)
		sprintf(str + strlen(str), " %s->%s %s$%s%s", ANSI_DIM, ANSI_RESET, COLOR_REG, wvm_decode_reg(rd), ANSI_RESET);
}

void wvm_disassemble_i_math(char *str, reg_t rs, reg_t rd, imm_t imm, char oper) {
	if (rs == rd) {
		if (imm == 1)
			sprintf(str, "%s$%s%s%c%c%s", COLOR_REG, wvm_decode_reg(rs), ANSI_DIM, oper, oper, ANSI_RESET);
		else
			sprintf(str, "%s$%s%s %s%c=%s %s%d%s", COLOR_REG, wvm_decode_reg(rs), ANSI_RESET, ANSI_BOLD, oper, ANSI_RESET, COLOR_IMM, imm, ANSI_RESET);
	} else
		sprintf(str, "%s$%s%s %s%c%s %s%d%s %s->%s %s$%s%s", COLOR_REG, wvm_decode_reg(rs), ANSI_RESET, ANSI_BOLD, oper, ANSI_RESET, COLOR_IMM, imm, ANSI_RESET, ANSI_DIM, ANSI_RESET, COLOR_REG, wvm_decode_reg(rd), ANSI_RESET);
}

void wvm_disassemble_j(char *str, word instruction) {
	opcode_t opcode = wvm_get_opcode(instruction);
	reg_t rs = wvm_j_rs(instruction);
	imm_t addr = wvm_j_addr(instruction);
	char link = wvm_j_link(instruction);
	char cond = wvm_j_cond(instruction);
	char *srs = wvm_decode_reg(rs);
	sprintf(str, "%s:", ANSI_DIM);

	if (link)
		sprintf(str, "%s:", str);

	switch (cond) {
		case COND_P:
			sprintf(str, "+%s", str);
			break;
		case COND_N:
			sprintf(str, "-%s", str);
			break;
		case COND_Z:
			sprintf(str, "0%s", str);
			break;
		case COND_NZ:
			sprintf(str, "*%s", str);
			break;
	}

	sprintf(str + strlen(str), " %s%d%s", COLOR_IMM, addr, ANSI_RESET);
	if (opcode == OP_JC)
		sprintf(str + strlen(str), " %sif%s %s$%s%s", COLOR_IF, ANSI_RESET, COLOR_REG, srs, ANSI_RESET);
}
