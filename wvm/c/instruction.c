/************************************************
 * Contains functions for parsing instructions. *
 ************************************************/

#include "wvm.h"
#include "instruction.h"
#include "ops.h"
#include "ext/net.h"

/**
 * Returns an instruction's opcode.
 * @param instruction A raw instruction.
 * @return The instruction's opcode.
 */
opcode_t wvm_get_opcode(word instruction) {
	return instruction >> 52;
}

/**
 * Returns the type of an instruction based on its opcode.
 * @param opcode An opcode.
 * @return The corresponding type.
 */
ins_type wvm_get_type(opcode_t opcode) {
	switch (opcode) {
		case 1:
		case 2:
		case 14:
		case 17:
		case 18:
		case 31:
			return R;

		case 15:
		case 16:
		case 44:
		case 45:
		case 46:
		case 47:
			return J;
	}

	return I;
}

/**
 * Returns a pointer to the function that handles a given instruction.
 * @param instruction A raw instruction.
 * @return A pointer to a function that handles the instruction.
 */
op_fn wvm_get_fn(word instruction) {
	switch (wvm_get_opcode(instruction)) {
		case OP_NOP:
			return op_nop;
		case OPS_MATH:
			switch (wvm_r_func(instruction)) {
				case FUNCT_ADD:   return op_add;
				case FUNCT_SUB:   return op_sub;
				case FUNCT_MULT:  return op_mult;
				case FUNCT_MULTU: return op_multu;
				case FUNCT_SLL:   return op_sll;
				case FUNCT_SRL:   return op_srl;
				case FUNCT_SRA:   return op_sra;
				case FUNCT_MOD:   return op_mod;
			}
		case OPS_RLOGIC:
			switch (wvm_r_func(instruction)) {
				case FUNCT_AND:   return op_and;
				case FUNCT_NAND:  return op_nand;
				case FUNCT_NOR:   return op_nor;
				case FUNCT_NOT:   return op_not;
				case FUNCT_OR:    return op_or;
				case FUNCT_XNOR:  return op_xnor;
				case FUNCT_XOR:   return op_xor;
				case FUNCT_LAND:  return op_land;
				case FUNCT_LNAND: return op_lnand;
				case FUNCT_LNOR:  return op_lnor;
				case FUNCT_LNOT:  return op_lnot;
				case FUNCT_LOR:   return op_lor;
				case FUNCT_LXNOR: return op_lxnor;
				case FUNCT_LXOR:  return op_lxor;
			}
		case OP_ADDI:  return op_addi;
		case OP_SUBI:  return op_subi;
		case OP_MULTI: return op_multi;
		case OP_ANDI:  return op_andi;
		case OP_NANDI: return op_nandi;
		case OP_NORI:  return op_nori;
		case OP_ORI:   return op_ori;
		case OP_XNORI: return op_xnori;
		case OP_XORI:  return op_xori;
		// MFHI/MFLO were dropped.
		case OP_LUI:   return op_lui;
		case OPS_RCOMP:
			switch (wvm_r_func(instruction)) {
				case FUNCT_SL:   return op_sl;
				case FUNCT_SLE:  return op_sle;
				case FUNCT_SEQ:  return op_seq;
				case FUNCT_SLU:  return op_slu;
				case FUNCT_SLEU: return op_sleu;
			}
		case OP_J:   return op_j;
		case OP_JC:  return op_jc;
		case OPS_RJUMP:
			switch (wvm_r_func(instruction)) {
				case FUNCT_JR:   return op_jr;
				case FUNCT_JRC:  return op_jrc;
				case FUNCT_JRL:  return op_jrl;
				case FUNCT_JRLC: return op_jrlc;
			}
		case OPS_MEM:
			switch (wvm_r_func(instruction)) {
				case FUNCT_C:     return op_c;
				case FUNCT_L:     return op_l;
				case FUNCT_S:     return op_s;
				case FUNCT_CB:    return op_cb;
				case FUNCT_LB:    return op_lb;
				case FUNCT_SB:    return op_sb;
				case FUNCT_SPUSH: return op_spush;
				case FUNCT_SPOP:  return op_spop;
			}
		case OP_LI:     return op_li;
		case OP_SI:     return op_si;
		case OP_SET:    return op_set;
		case OP_MULTUI: return op_multui;
		case OP_SLI:    return op_sli;
		case OP_SLEI:   return op_slei;
		case OP_SGI:    return op_sgi;
		case OP_SGEI:   return op_sgei;
		case OP_SEQI:   return op_seqi;
		case OP_SLUI:   return op_slui;
		case OP_SLEUI:  return op_sleui;
		case OP_MODI:   return op_modi;
		case OPS_EXT:
			switch (wvm_r_func(instruction)) {
				case FUNCT_PRINTR: return op_pr;
				case FUNCT_HALT:   return op_halt;
				case FUNCT_EVAL:   return op_eval;
				case FUNCT_PRC:    return op_prc;
				case FUNCT_PRD:    return op_prd;
				case FUNCT_PRX:    return op_prx;
				case FUNCT_SLEEP:  return op_sleep;

				case FUNCT_XN_INIT:    return op_xn_init;
				case FUNCT_XN_CONNECT: return op_xn_connect;
				case FUNCT_XN_SEND:    return op_xn_send;
				case FUNCT_XN_RECV:    return op_xn_recv;
			}
		case OP_SLLI: return op_slli;
		case OP_SRLI: return op_srli;
		case OP_SRAI: return op_srai;
		case OP_LBI:  return op_lbi;
		case OP_SBI:  return op_sbi;
		case OP_LNI:  return op_lni;
		case OP_LBNI: return op_lbni;
	}

	return NULL;
}

/**
 * Returns the `rt` register of an R-type instruction.
 * @param instruction An R-type instruction.
 * @return A register ID.
 */
reg_t wvm_r_rt(word instruction) {
	return (instruction >> 45) & 127; 
}

/**
 * Returns the `rs` register of an R-type instruction.
 * @param instruction An R-type instruction.
 * @return A register ID.
 */
reg_t wvm_r_rs(word instruction) {
	return (instruction >> 38) & 127; 
}

/**
 * Returns the `rd` register of an R-type instruction.
 * @param instruction An R-type instruction.
 * @return A register ID.
 */
reg_t wvm_r_rd(word instruction) {
	return (instruction >> 31) & 127; 
}

/**
 * Decodes all three registers of an R-type instruction at once.
 * @param instruction An R-type instruction.
 * @param rs A pointer to the rs ID.
 * @param rt A pointer to the rt ID.
 * @param rd A pointer to the rd ID.
 */
void wvm_r_regs(word instruction, reg_t *rs, reg_t *rt, reg_t *rd) {
	*rs = wvm_r_rs(instruction);
	*rt = wvm_r_rt(instruction);
	*rd = wvm_r_rd(instruction);
}

/**
 * Returns the linker flags of an R-type instruction.
 * @param instruction An R-type instruction.
 * @return A set of linker flags.
 */
char wvm_r_flags(word instruction) {
	return (instruction >> 12) & 63;
}

/**
 * Returns the function ID of an R-type instruction.
 * @param instruction An R-type instruction.
 * @return A function ID.
 */
int wvm_r_func(word instruction) {
	return instruction & 4095;
}

/**
 * Returns the conditions of an R-type instruction.
 * @param instruction An R-type instruction.
 * @return A conditions ID.
 */
char wvm_r_cond(word instruction) {
	return (instruction >> 14) & 0b1111;
}

/**
 * Returns the linker flags of an I-type instruction.
 * @param instruction An I-type instruction.
 * @return A set of linker flags.
 */
char wvm_i_flags(word instruction) {
	return (instruction >> 46) & 63;
}

/**
 * Returns the `rs` register of an I-type instruction.
 * @param instruction An I-type instruction.
 * @return A register ID.
 */
reg_t wvm_i_rs(word instruction) {
	return (instruction >> 39) & 127;
}

/**
 * Returns the `rd` register of an I-type instruction.
 * @param instruction An I-type instruction.
 * @return A register ID.
 */
reg_t wvm_i_rd(word instruction) {
	return (instruction >> 32) & 127;
}

/**
 * Returns the immediate value of an I-type instruction.
 * @param instruction An I-type instruction.
 * @return An immediate value.
 */
imm_t wvm_i_imm(word instruction) {
	return instruction & 0xffffffff;
}

/**
 * Returns the `rs` register of a J-type instruction.
 * @param instruction A J-type instruction.
 * @return A register ID.
 */
reg_t wvm_j_rs(word instruction) {
	return (instruction >> 45) & 127;
}

/**
 * Returns the linker flags of a J-type instruction.
 * @param instruction A J-type instruction.
 * @return A set of linker flags.
 */
char wvm_j_flags(word instruction) {
	return (instruction >> 32) & 63;
}

/**
 * Returns the address value of a J-type instruction.
 * @param instruction A J-type instruction.
 * @return An address value.
 */
imm_t wvm_j_addr(word instruction) {
	return instruction & 0xffffffff;
}

/**
 * Returns the link bit of a J-type instruction.
 * @param instruction A J-type instruction.
 * @return A link bit.
 */
char wvm_j_link(word instruction) {
	return (instruction >> 44) & 1;
}

/**
 * Returns the conditions of a J-type instruction.
 * @param instruction A J-type instruction.
 * @return A conditions ID.
 */
char wvm_j_cond(word instruction) {
	return (instruction >> 34) & 0b1111;
}
