#include <stdio.h>
#include <stdlib.h>
#include "wvm.h"

/**
 * Allocates memory for the VM and resets some global variables..
 * @param length The number of longs to allocate.
 * @return Whether the allocation succeeded.
 */
bool wvm_init(lomg length) {
	pc = 0;
	ir = 0;
	memsize = 0;
	return (memory = calloc(length, sizeof(lomg))) != NULL;
}

/**
 * Frees all the VM memory.
 */
void wvm_free() {
	if (memory != NULL) {
		free(memory);
	}
}

/**
 * Loads a wvm program into VM memory.
 * @param filename The path of the file to load.
 * @return The number of lines read into memory.
 */
int wvm_load(char *filename) {
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		fprintf(stderr, "Couldn't open %s.\n", filename);
		exit(1);
	}

	memsize = 0;
	char line[20];
	while (fgets(line, 18, file)) {
		lomg instr = strtol(line, NULL, 16);
		memory[memsize++] = instr;
	}

	offset_handlers = wvm_get_word(0);
	offset_data = wvm_get_word(8);
	offset_code = wvm_get_word(16);
	offset_end = wvm_get_word(24);

	return memsize;
}

/**
 * Sets the initial position of the program counter.
 */
void wvm_init_pc() {
	pc = memory[2] >> 3;
}

/**
 * Returns the nth word in the VM memory.
 * @param n The byte-index of the word to retrieve.
 * @return The word at the given index.
 */
lomg wvm_get_word(lomg n) {
	if (n % 8 == 0) {
		// Everything is so much simpler when the index is word aligned.
		return memory[n >> 3];
	}

	// The offset isn't word aligned, so we have to get two words from the memory
	// and shuffle them around to assemble a single word to return.

	// The number of bytes to take from the word on the right half of the concatenation.
	// It's equal to the remainder of trying to use n as a word-aligned offset.
	char right_length = n % 8;

	// The length of the left half is 8 minus the right length, because the total length is 8 bytes.
	char left_length = 8 - right_length;

	// The left half is earlier in memory, the right half is right after.
	lomg left = memory[n >> 3];
	lomg right = memory[(n >> 3) + 1];
	
	// First, we assemble a mask to select the (left_length) least significant bytes of the left word.
	// Then we shift those bytes to the left to make room for the bytes from the right word.
	return ((left & ((1 << (left_length << 3)) - 1)) << right_length) | (right >> left_length);
}

/**
 * Returns the byte at a given offset in the VM memory.
 * @param n The index of the byte to retrieve.
 * @return The byte at the given index.
 */
char wvm_get_byte(lomg byte_offset) {
	return (memory[byte_offset >> 4] >> (byte_offset % 16)) & 0xff;
}

/**
 * Prints all the memory to the console.
 */
void wvm_print_memory() {
	char *binsep = "───────────────────────────────────────────────────────────────────";
	printf("       ┌────────────────────┬%s┬──────────────────────┐\n", binsep);
	printf("       │    \33[1mHexadecimal\33[0m     │");
	printf("                              \33[1mBinary\33[0m                               │");
	printf("       \33[1mDecimal\33[0m        │\n");
	printf("┌──────┼────────────────────┼%s┼──────────────────────┤\n", binsep);
	for (int i = 0; i < memsize; i++) {
		lomg boffset = i << 3;
		lomg word = wvm_get_word(boffset);
		if (boffset == offset_handlers || boffset == offset_data)
			printf("├──────┼────────────────────┼%s┼──────────────────────┤\n", binsep);
		else if (boffset == offset_code)
			printf("├──────┼────────────────────┼%s┼──────────────────────┤\n", binsep);

		printf("│\33[38;5;8m");
		if (i == pc)
			printf("\33[7m");
		
		printf(" %04lld \33[0m│ \33[38;5;7m0x\33[0m\33[1m%016llx\33[0m │ \33[38;5;250m", boffset, word);
		if (boffset < offset_code)
			printf(" ");
		else {
			ins_type type = wvm_get_type(wvm_get_opcode(word));
			if (type == R)
				printf("\33[31m");
			else if (type == I)
				printf("\33[32m");
			else if (type == J)
				printf("\33[34m");
			else
				printf("\33[33m");
		}

		for (int j = 63; j >= 0; j--) {
			printf("%d", (int) (word >> j) & 1);
			if (j == 52 && boffset >= offset_code)
				printf("\33[38;5;238m│\33[38;5;250m");
		}

		printf("\33[0m │ \33[38;5;240m%20lld\33[0m │", word);

		if (boffset == offset_handlers)
			printf(" Handlers");
		else if (boffset == offset_data)
			printf(" Data");
		else if (boffset == offset_code)
			printf(" Code");
		
		printf("\n");
	}

	printf("└──────┴────────────────────┴%s┴──────────────────────┘\n", binsep);
}

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
