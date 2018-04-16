#include <stdio.h>
#include <stdlib.h>
#include "wvm.h"
#include "instruction.h"
#include "disassemble.h"
#include "ansi.h"

/**
 * Allocates memory for the VM and resets some global variables.
 * @param length The number of words to allocate.
 * @return Whether the allocation succeeded.
 */
bool wvm_init(word length) {
	pc = 0;
	memsize = 0;
	alive = 1;
	membytes = length * 8;
	return (memory = calloc(membytes, sizeof(byte))) != NULL;
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
		word instr = strtoul(line, NULL, 16);
		wvm_set_word(memsize++ * 8, instr);
	}

	offset_handlers = wvm_get_word(0);
	offset_data = wvm_get_word(8);
	offset_code = wvm_get_word(16);
	offset_end = wvm_get_word(24);

	return memsize;
}

/**
 * Initializes the VM after a program is loaded.
 */
void wvm_init_vm() {
	pc = offset_code;
	registers[R_SP] = membytes - 8;
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
 * Increments the program counter by one word.
 */
void wvm_increment_pc() {
	pc += 8;
}

/**
 * Executes the next instruction.
 * @return Whether to continue to the next instruction.
 */
bool wvm_tick() {
	word instruction = wvm_get_word(pc);
	op_fn op = wvm_get_fn(instruction);
	op(instruction);
	return alive;
}

/**
 * Prints all the memory to the console.
 */
void wvm_print_memory() {
	char *sep = "────────────────────────";
	char *jump = "\33[54G";
	printf("       ┌────────────────────┬%s┐\n", sep);
	printf("       │    \33[1mHexadecimal\33[0m     │");
	printf("     \33[1mDecoded\33[0m            │");
	printf("\n┌──────┼────────────────────┼%s┤\n", sep);
	for (int i = 0; i < memsize; i++) {
		word boffset = i << 3;
		word word = wvm_get_word(boffset);
		if (boffset == offset_handlers || boffset == offset_data)
			printf("├──────┼────────────────────┼%s┤\n", sep);
		else if (boffset == offset_code)
			printf("├──────┼────────────────────┼%s┤\n", sep);

		printf("│\33[38;5;8m");
		if (boffset == pc)
			printf("\33[7m");
		
		printf(" %04lld \33[0m│ \33[38;5;7m0x\33[0m\33[1m%016llx\33[0m │\33[38;5;250m", boffset, word);
		if (i < 4)
			printf(" %s%lld%s", ANSI_MAGENTA, word, ANSI_RESET);
		else if (boffset < offset_handlers || (offset_data <= boffset && boffset < offset_code)) {
			printf(" ");
			for (char j = 56; j >= 0; j -= 8) {
				char byte = (char) (word >> j) & 0xff;
				if (byte < 32)
					printf("%s.%s", ANSI_DIM, ANSI_RESET);
				else
					printf("%c", byte);
			}
		}

		if (i == 0)
			printf("\33[45G%shandlers%s", ANSI_DIM, ANSI_RESET);
		else if (i == 1)
			printf("\33[49G%sdata%s", ANSI_DIM, ANSI_RESET);
		else if (i == 2)
			printf("\33[49G%scode%s", ANSI_DIM, ANSI_RESET);
		else if (i == 3)
			printf("\33[50G%send%s", ANSI_DIM, ANSI_RESET);

		if (boffset < offset_code)
			printf("%s│", jump);

		if (boffset == 0)
			printf(" Metadata");
		else if (boffset == offset_handlers)
			printf(" Handlers");
		else if (boffset == offset_data)
			printf(" Data");

		if (offset_code <= boffset) {
			printf(" %s", wvm_disassemble(word));
			printf("%s│", jump);
		
		}
		if (boffset == offset_code)
			printf(" Code");
		
		printf("\n");
	}

	printf("└──────┴────────────────────┴%s┘\n", sep);
}
