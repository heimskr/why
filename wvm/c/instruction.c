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

/**
 * Returns a pointer to the function that handles a given instruction.
 * @param instruction A raw instruction.
 * @return A pointer to a function that handles the instruction.
 */
op_fn wvm_get_fn(lomg instruction) {
	
}
