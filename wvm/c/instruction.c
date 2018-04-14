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
opcode_t wvm_get_opcode(lomg instruction) {
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
		case 12:
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

// /**
//  * Returns a pointer to the function that handles a given instruction.
//  * @param instruction A raw instruction.
//  * @return A pointer to a function that handles the instruction.
//  */
// op_fn wvm_get_fn(lomg instruction) {
// 	
// }

/**
 * Returns the `rt` register of an R-type instruction.
 * @param instruction An R-type instruction.
 * @return A register ID.
 */
reg_t wvm_r_rt(lomg instruction) {
	return (instruction >> 45) & 127; 
}

/**
 * Returns the `rs` register of an R-type instruction.
 * @param instruction An R-type instruction.
 * @return A register ID.
 */
reg_t wvm_r_rs(lomg instruction) {
	return (instruction >> 38) & 127; 
}

/**
 * Returns the `rd` register of an R-type instruction.
 * @param instruction An R-type instruction.
 * @return A register ID.
 */
reg_t wvm_r_rd(lomg instruction) {
	return (instruction >> 31) & 127; 
}

/**
 * Returns the linker flags of an R-type instruction.
 * @param instruction An R-type instruction.
 * @return A set of linker flags.
 */
char wvm_r_flags(lomg instruction) {
	return (instruction >> 12) & 63;
}

/**
 * Returns the function ID of an R-type instruction.
 * @param instruction An R-type instruction.
 * @return A function ID.
 */
char wvm_r_func(lomg instruction) {
	return instruction & 4095;
}

/**
 * Returns the linker flags of an I-type instruction.
 * @param instruction An I-type instruction.
 * @return A set of linker flags.
 */
char wvm_i_flags(lomg instruction) {
	return (instruction >> 46) & 63;
}

/**
 * Returns the `rs` register of an I-type instruction.
 * @param instruction An I-type instruction.
 * @return A register ID.
 */
reg_t wvm_i_rs(lomg instruction) {
	return (instruction >> 39) & 127;
}

/**
 * Returns the `rd` register of an I-type instruction.
 * @param instruction An I-type instruction.
 * @return A register ID.
 */
reg_t wvm_i_rd(lomg instruction) {
	return (instruction >> 32) & 127;
}

/**
 * Returns the immediate value of an I-type instruction.
 * @param instruction An I-type instruction.
 * @return An immediate value.
 */
imm_t wvm_i_imm(lomg instruction) {
	return instruction & 0xffffffff;
}

/**
 * Returns the `rs` register of a J-type instruction.
 * @param instruction A J-type instruction.
 * @return A register ID.
 */
reg_t wvm_j_rs(lomg instruction) {
	return (instruction >> 45) & 127;
}

/**
 * Returns the linker flags of a J-type instruction.
 * @param instruction A J-type instruction.
 * @return A set of linker flags.
 */
char wvm_j_flags(lomg instruction) {
	return (instruction >> 32) & 63;
}

/**
 * Returns the address value of a J-type instruction.
 * @param instruction A J-type instruction.
 * @return An address value.
 */
imm_t wvm_j_addr(lomg instruction) {
	return instruction & 0xffffffff;
}
