/************************************************
 * Contains functions for converting assembled  *
 * instructions back into text representations. *
 ************************************************/

#include <stdio.h>
#include "registers.h"
#include "instruction.h"
#include "ansi.h"
#include "disassemble.h"

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
		case OPS_MATH:
			switch (func) {
				case FUNCT_ADD: oper = "+"; break;
				case FUNCT_SUB: oper = "-"; break;
				case FUNCT_MULT:
					sprintf(str, "%s$%s%s %s*%s %s$%s%s", COLOR_REG, srs, ANSI_RESET, ANSI_BOLD, ANSI_RESET, COLOR_REG, srt, ANSI_RESET);
					return;
				case FUNCT_SLL: oper = "<<"; break;
				case FUNCT_SRL: oper = ">>>"; break;
				case FUNCT_SRA: oper = ">>"; break;
				case FUNCT_MOD: oper = "%"; break;
			}

			wvm_disassemble_r_alt_op(str, rs, rt, rd, oper);
			return;

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
			if (func ==  FUNCT_SL || func ==  FUNCT_SLU)
				sprintf(str, "%s<", str);
			if (func ==  FUNCT_SLE || func ==  FUNCT_SLEU)
				sprintf(str, "%s<=", str);
			if (func ==  FUNCT_SEQ || func ==  FUNCT_SEQU)
				sprintf(str, "%s==", str);
			sprintf(str, "%s%s %s$%s%s %s->%s %s$%s%s", str, ANSI_RESET, COLOR_REG, srt, ANSI_RESET, ANSI_DIM, ANSI_RESET, COLOR_REG, srd, ANSI_RESET);
			if (func == FUNCT_SLU || func == FUNCT_SLEU || func == FUNCT_SEQU)
				sprintf(str, "%s /u", str);
			return;

		case OPS_RJUMP:
			sprintf(str, "%s:", ANSI_DIM);
			if (func == FUNCT_JRL || func == FUNCT_JRLC)
				sprintf(str, "%s:", str);
			sprintf(str, "%s%s %s$%s%s", str, ANSI_RESET, COLOR_REG, srd, ANSI_RESET);
			if (func == FUNCT_JRC || func == FUNCT_JRLC)
				sprintf(str, "%s %sif%s %s$%s%s", str, ANSI_RED, ANSI_RESET, COLOR_REG, srs, ANSI_RESET);
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
			}

		case OPS_TRAP:
			sprintf(str, "<%s", ANSI_CYAN);
			switch (func) {
				case FUNCT_PRINTR:
					sprintf(str, "%sprint%s %s%s", str, ANSI_RESET, COLOR_REG, srs);
					break;
				case FUNCT_HALT:
					sprintf(str, "%shalt", str);
					break;
				case FUNCT_EVAL:
					sprintf(str, "%seval", str);
					break;
				case FUNCT_PRC:
					sprintf(str, "%sprc%s %s%s", str, ANSI_RESET, COLOR_REG, srs);
					break;
				case FUNCT_PRD:
					sprintf(str, "%sprd%s %s%s", str, ANSI_RESET, COLOR_REG, srs);
					break;
				case FUNCT_PRX:
					sprintf(str, "%sprx%s %s%s", str, ANSI_RESET, COLOR_REG, srs);
					break;
			}

			sprintf(str, "%s%s>", str, ANSI_RESET);
			return;
	}

	sprintf(str, "%s(Unknown R-type)%s", ANSI_DIM, ANSI_RESET);
}

void wvm_disassemble_r_alt_op(char *str, reg_t rs, reg_t rt, reg_t rd, char *oper) {
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
		case OP_ADDUI:
			wvm_disassemble_i_math(str, rs, rd, imm, '+');
			break;
		case OP_SUBI:
		case OP_SUBUI:
			wvm_disassemble_i_math(str, rs, rd, imm, '-');
			break;
		case OP_MULTI:
		case OP_MULTUI:
			sprintf(str, "%s$%s%s %s*%s %s%d%s", COLOR_REG, wvm_decode_reg(rs), ANSI_RESET, ANSI_BOLD, ANSI_RESET, COLOR_IMM, imm, ANSI_RESET);
			break;
		case OP_MODI:
			wvm_disassemble_i_alt_op(str, rs, rd, imm, "%");
			break;
		case OP_ANDI:
			wvm_disassemble_i_alt_op(str, rs, rd, imm, "&");
			break;
		case OP_NANDI:
			wvm_disassemble_i_alt_op(str, rs, rd, imm, "~&");
			break;
		case OP_NORI:
			wvm_disassemble_i_alt_op(str, rs, rd, imm, "~|");
			break;
		case OP_ORI:
			wvm_disassemble_i_alt_op(str, rs, rd, imm, "|");
			break;
		case OP_XNORI:
			wvm_disassemble_i_alt_op(str, rs, rd, imm, "~x");
			break;
		case OP_XORI:
			wvm_disassemble_i_alt_op(str, rs, rd, imm, "x");
			break;
		case OP_SLLI:
			wvm_disassemble_i_alt_op(str, rs, rd, imm, "<<");
			break;
		case OP_SRLI:
			wvm_disassemble_i_alt_op(str, rs, rd, imm, ">>>");
			break;
		case OP_SRAI:
			wvm_disassemble_i_alt_op(str, rs, rd, imm, ">>");
			break;
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
		case OP_SEQI:
			wvm_disassemble_i_comp(str, rs, rd, imm, "==");
			break;
		default:
			sprintf(str, "%s(Unknown I-type)%s", ANSI_DIM, ANSI_RESET);
			return;
	}

	if (opcode == OP_ADDUI || opcode == OP_SUBUI || opcode == OP_MULTUI || opcode == OP_SLUI || opcode == OP_SLEUI)
		sprintf(str, "%s /u", str);
	else if (opcode == OP_LBI || opcode == OP_SBI)
		sprintf(str, "%s /b", str);
}

void wvm_disassemble_i_comp(char *str, reg_t rs, reg_t rd, imm_t imm, char *oper) {
	sprintf(str, "%s$%s%s %s%s%s %s%d%s %s->%s %s$%s%s", COLOR_REG, wvm_decode_reg(rs), ANSI_RESET, ANSI_BOLD, oper, ANSI_RESET, COLOR_IMM, imm, ANSI_RESET, ANSI_DIM, ANSI_RESET, COLOR_REG, wvm_decode_reg(rd), ANSI_RESET);
}

void wvm_disassemble_i_alt_op(char *str, reg_t rs, reg_t rd, imm_t imm, char *oper) {
	sprintf(str, "%s$%s%s %s%s%s%s %s%d%s", COLOR_REG, wvm_decode_reg(rs), ANSI_RESET, ANSI_BOLD, oper, rs == rd? "=" : "", ANSI_RESET, COLOR_IMM, imm, ANSI_RESET);
	if (rs != rd)
		sprintf(str, "%s %s->%s %s$%s%s", str, ANSI_DIM, ANSI_RESET, COLOR_REG, wvm_decode_reg(rd), ANSI_RESET);
}

void wvm_disassemble_i_math(char *str, reg_t rs, reg_t rd, imm_t imm, char oper) {
	if (rs == rd) {
		if (imm == 1)
			sprintf(str, "%s$%s%s%c%c%s", COLOR_REG, wvm_decode_reg(rs), ANSI_DIM, oper, oper, ANSI_RESET);
		else sprintf(str, "%s$%s%s %s%c=%s %s%d%s", COLOR_REG, wvm_decode_reg(rs), ANSI_RESET, ANSI_BOLD, oper, ANSI_RESET, COLOR_IMM, imm, ANSI_RESET);
	} else sprintf(str, "%s$%s%s %s%c%s %s%d%s %s->%s %s$%s%s", COLOR_REG, wvm_decode_reg(rs), ANSI_RESET, ANSI_BOLD, oper, ANSI_RESET, COLOR_IMM, imm, ANSI_RESET, ANSI_DIM, ANSI_RESET, COLOR_REG, wvm_decode_reg(rd), ANSI_RESET);
}

void wvm_disassemble_j(char *str, word instruction) {
	opcode_t opcode = wvm_get_opcode(instruction);
	reg_t rs = wvm_j_rs(instruction);
	imm_t addr = wvm_j_addr(instruction);
	char *srs = wvm_decode_reg(rs);
	sprintf(str, "%s:", ANSI_DIM);

	if (opcode == OP_JL || opcode == OP_JLC)
		sprintf(str, "%s:", str);
	sprintf(str, "%s %s%d%s", str, COLOR_IMM, addr, ANSI_RESET);
	if (opcode == OP_JC || opcode == OP_JLC)
		sprintf(str, "%s %sif%s %s$%s%s", str, COLOR_IF, ANSI_RESET, COLOR_REG, srs, ANSI_RESET);
}
