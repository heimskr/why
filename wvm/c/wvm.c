#include <stdio.h>
#include <stdlib.h>
#include "wvm.h"
#include "instruction.h"

/**
 * Allocates memory for the VM and resets some global variables.
 * @param length The number of words to allocate.
 * @return Whether the allocation succeeded.
 */
bool wvm_init(word length) {
	pc = 0;
	memsize = 0;
	alive = 1;
	return (memory = calloc(length * 8, sizeof(byte))) != NULL;
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
		word instr = strtol(line, NULL, 16);
		wvm_set_word(memsize++ * 8, instr);
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
	pc = offset_code;
}

/**
 * Returns a word from the VM memory.
 * @param addr The byte-indexed address of the word to retrieve.
 * @return The word at the given index.
 */
word wvm_get_word(word addr) {
	word out = 0;

	for (byte i = 0; i < 8; i++) {
		out |= ((word) memory[addr + i]) << ((7 - i) * 8);
	}

	return out;
}

/**
 * Sets a word in the VM memory.
 * @param addr The byte-indexed starting address of the destination in the VM memory.
 * @param value The value to store in the VM memory.
 */
void wvm_set_word(word addr, word value) {
	memory[addr]     = (value >> 7*8) & 0xff;
	memory[addr + 1] = (value >> 6*8) & 0xff;
	memory[addr + 2] = (value >> 5*8) & 0xff;
	memory[addr + 3] = (value >> 4*8) & 0xff;
	memory[addr + 4] = (value >> 3*8) & 0xff;
	memory[addr + 5] = (value >> 2*8) & 0xff;
	memory[addr + 6] = (value >> 1*8) & 0xff;
	memory[addr + 7] =  value         & 0xff;
}

/**
 * Returns a byte from the VM memory.
 * @param addr The byte-indexed address of the byte to retrieve.
 * @return The byte at the given index.
 */
byte wvm_get_byte(word addr) {
	return memory[addr];
}

/**
 * Sets a byte in the VM memory.
 * @param addr The byte-indexed starting address of the destination in the VM memory.
 * @param value The value to store in the VM memory.
 */
void wvm_set_byte(word addr, byte value) {
	memory[addr] = value;
}

/**
 * Jumps to a given (byte-indexed) address.
 * @param addr An address in memory.
 */
void wvm_jump(word addr) {
	pc = addr;
}

/**
 * Stores the address of the next instruction in `$rt`.
 */
void wvm_link() {
	registers[R_RT] = pc + 8;
}

/**
 * Executes the next instruction.
 * @return Whether to continue to the next instruction.
 */
bool wvm_tick() {
	word instruction = wvm_get_word(pc);
	printf("instruction: %016llx\n", instruction);
	alive = 0;
	return alive;
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
		word boffset = i << 3;
		word word = wvm_get_word(boffset);
		if (boffset == offset_handlers || boffset == offset_data)
			printf("├──────┼────────────────────┼%s┼──────────────────────┤\n", binsep);
		else if (boffset == offset_code)
			printf("├──────┼────────────────────┼%s┼──────────────────────┤\n", binsep);

		printf("│\33[38;5;8m");
		if (boffset == pc)
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
