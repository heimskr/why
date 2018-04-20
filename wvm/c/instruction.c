/************************************************
 * Contains functions for parsing instructions. *
 ************************************************/

#include "wvm.h"
#include "instruction.h"
#include "ops.h"

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
		case 32:
		case 33:
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
		case 0:
			return op_nop;
		case 1:
			switch (wvm_r_func(instruction)) {
				case 0: return op_add;
				case 1: return op_sub;
				case 2: return op_mult;
				case 3: return op_addu;
				case 4: return op_subu;
				case 5: return op_multu;
				case 6: return op_sll;
				case 7: return op_srl;
				case 8: return op_sra;
				case 9: return op_mod;
			}
		case 2:
			switch (wvm_r_func(instruction)) {
				case 0:  return op_and;
				case 1:  return op_nand;
				case 2:  return op_nor;
				case 3:  return op_not;
				case 4:  return op_or;
				case 5:  return op_xnor;
				case 6:  return op_xor;
				case 8:  return op_land;
				case 9:  return op_lnand;
				case 10: return op_lnor;
				case 11: return op_lnot;
				case 12: return op_lor;
				case 13: return op_lxnor;
				case 14: return op_lxor;
			}
		case 3:  return op_addi;
		case 4:  return op_subi;
		case 5:  return op_multi;
		case 6:  return op_andi;
		case 7:  return op_nandi;
		case 8:  return op_nori;
		case 9:  return op_ori;
		case 10: return op_xnori;
		case 11: return op_xori;
		// MFHI/MFLO were dropped.
		case 13: return op_lui;
		case 14:
			switch (wvm_r_func(instruction)) {
				case 0: return op_sl;
				case 1: return op_sle;
				case 2: return op_seq;
				case 3: return op_slu;
				case 4: return op_sleu;
			}
		case 15: return op_j;
		case 16: return op_jc;
		case 17:
			switch (wvm_r_func(instruction)) {
				case 0: return op_jr;
				case 1: return op_jrc;
				case 2: return op_jrl;
				case 3: return op_jrlc;
			}
		case 18:
			switch (wvm_r_func(instruction)) {
				case 0: return op_c;
				case 1: return op_l;
				case 2: return op_s;
				case 3: return op_cb;
				case 4: return op_lb;
				case 5: return op_sb;
			}
		case 19: return op_li;
		case 20: return op_si;
		case 21: return op_set;
		case 22: return op_addui;
		case 23: return op_subui;
		case 24: return op_multui;
		case 25: return op_sli;
		case 26: return op_slei;
		case 27: return op_seqi;
		case 28: return op_slui;
		case 29: return op_sleui;
		case 30: return op_modi;
		case 31: // Are traps true R-types? The format in the ISA documentation looks strange.
			switch (wvm_r_func(instruction)) {
				case 1:
					return op_pr;
				case 2:
					return op_halt;
				case 3:
					return op_eval;
				case 4:
					return op_prc;
				case 5:
					return op_prd;
				case 6:
					return op_prx;
			}
		case 32: return op_jl;
		case 33: return op_jlc;
		case 34: return op_slli;
		case 35: return op_srli;
		case 36: return op_srai;
		case 37: return op_lbi;
		case 38: return op_sbi;
		case 39: return op_lni;
		case 40: return op_lbni;
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
char wvm_r_func(word instruction) {
	return instruction & 4095;
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
