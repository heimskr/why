#include <stdio.h>
#include <stdlib.h>

#include "wvm.h"
#include "ansi.h"
#include "disassemble.h"
#include "instruction.h"
#include "interrupts.h"

ring_t cur_ring = RING_KERNEL;

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
	cycles = 0;
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
		wvm_set_word(memsize++ * 8, instr, false);
	}

	fclose(file);

	offset_symtab = wvm_get_word(0, false);
	offset_code = wvm_get_word(8, false);
	offset_data = wvm_get_word(16, false);
	offset_end = wvm_get_word(24, false);

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
 * @param little_endian Whether the access should be little-endian instead of big-endian.
 * @return The word at the given index.
 */
word wvm_get_word(word addr, bool little_endian) {
	word out = 0;

	if (little_endian) {
		for (byte i = 0; i < 8; i++)
			out |= ((word) memory[addr + i]) << (i * 8);
	} else {
		for (byte i = 0; i < 8; i++)
			out |= ((word) memory[addr + i]) << ((7 - i) * 8);
	}

	return out;
}

/**
 * Sets a word in the VM memory.
 * @param addr The byte-indexed starting address of the destination in the VM memory.
 * @param value The value to store in the VM memory.
 * @param little_endian Whether the access should be little-endian instead of big-endian.
 */
void wvm_set_word(word addr, word value, bool little_endian) {
	if (little_endian) {
		for (int i = 0; i < 8; i++)
			memory[addr + i] = (value >> 8*i) & 0xff;
	} else {
		for (int i = 0; i < 8; i++)
			memory[addr + 7 - i] = (value >> 8*i) & 0xff;
	}
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
 * Returns a halfword from the VM memory.
 * @param addr The byte-indexed address of the word to retrieve.
 * @return The halfword at the given index.
 */
hword wvm_get_halfword(word addr) {
	hword out = 0;

	for (byte i = 0; i < 4; i++)
		out |= ((hword) memory[addr + i]) << (i * 8);

	return out;
}

/**
 * Sets a halfword in the VM memory.
 * @param addr The byte-indexed starting address of the destination in the VM memory.
 * @param value The value to store in the VM memory.
 */
void wvm_set_halfword(word addr, hword value) {
	for (int i = 0; i < 4; i++)
		memory[addr + i] = (value >> 8*i) & 0xff;
}

/**
 * Retrieves a string from the VM memory.
 * Reads bytes starting at the given address until a null byte is reached.
 * @param addr The byte-indexed address of the string to retrieve.
 * @return A copy of the string stored at the given index.
 */
char * wvm_get_string(word addr) {
	int length;
	for (length = 0; memory[addr + length] != '\0'; length++);

	char *out = calloc(length + 1, sizeof(char));
	for (int i = 0; i < length; i++)
		out[i] = memory[addr + i];

	out[length] = '\0';
	return out;
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
 * Attempts to change the ring safely. If the new ring value is lower
 * than the current ring value, a protection interrupt will occur.
 * @param new_ring The ring to try to change to.
 * @return 1 if the operation succeeded; 0 otherwise.
 */
int wvm_change_ring(ring_t new_ring) {
	if (new_ring < RING_MIN || RING_MAX < new_ring) {
#ifdef WVM_DEBUG
		fprintf(stderr, "Invalid ring value: %d\n", new_ring);
#endif
	} else if (new_ring < cur_ring) {
#ifdef WVM_DEBUG
		fprintf(stderr, "Preventing privilege escalation (%d -> %d)\n", cur_ring, new_ring);
#endif
	} else {
		cur_ring = new_ring;
		return 1;
	}

	int_protec();
	return 0;
}

/**
 * Executes the next instruction.
 * @return Whether to continue to the next instruction.
 */
bool wvm_tick() {
	word instruction = wvm_get_word(pc, false);
	op_fn op = wvm_get_fn(instruction);
	if (op == NULL) {
		DIEF("%s Couldn't find function for instruction %s0x%016llx%s (opcode %s%d%s) at address %s%lld%s\n", S_NOPE,
			BOLD(instruction), BOLD(wvm_get_opcode(instruction)), BOLD(pc));
	}

	op(instruction);
	cycles++;
	return alive;
}

/**
 * Clears the status register's condition bits.
 */
void wvm_alu_flags_clear() {
	registers[R_ST] &= ~0b1111;
}

/**
 * Updates the status register after an ALU operation.
 * @param n The output of the ALU operation.
 */
void wvm_alu_flags_update(word n) {
	wvm_alu_flags_clear();
	if (n == 0) {
		FLAG_Z_SET(1);
	} else if (n < 0) {
		FLAG_N_SET(1);
	}
}

/**
 * Checks whether the status register matches a given condition.
 * @param n A condition enum.
 * @return 0 if the condition is false, 1 if the condition is true or 2 if the argument is invalid.
 */
int wvm_check_condition(cond_t n) {
	switch (n) {
		case COND_P: return !FLAG_N && !FLAG_Z;
		case COND_N: return FLAG_N;
		case COND_Z: return FLAG_Z;
		case COND_NZ: return !FLAG_Z;
		case COND_NONE: return 1;
	}
	
	fprintf(stderr, "Unknown condition: %d\n", n);
	exit(2);
}

/**
 * Prints all the memory to the console.
 */
void wvm_print_memory() {
	char *sep = "────────────────────────";
	char *jump = "\e[55G"; // If printed to the console, moves the cursor horizontally to right after the table
	printf("        ┌────────────────────┬%s┐\n", sep);
	printf("        │    \e[1mHexadecimal\e[0m     │");
	printf("     \e[1mDecoded\e[0m            │");
	printf("\n┌───────┼────────────────────┼%s┤\n", sep);
	for (int i = 0; i < memsize; i++) {
		word boffset = i << 3;
		word word = wvm_get_word(boffset, false);
		if (boffset == offset_symtab || boffset == offset_code || boffset == offset_data)
			printf("├───────┼────────────────────┼%s┤%s\n", sep, ANSI_RESET);

		printf("│\e[38;5;8m");
		if (boffset == pc)
			printf("\e[7m");
		
		printf(" %5lld \e[0m│ \e[38;5;7m0x\e[0m\e[1m%016llx\e[0m │\e[38;5;250m", boffset, word);
		if (i < 4)
			printf(" %s%lld%s", ANSI_MAGENTA, word, ANSI_RESET);
		else if (boffset < offset_code || offset_data <= boffset) {
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
			printf("\e[48G%ssymtab%s", ANSI_DIM, ANSI_RESET);
		else if (i == 1)
			printf("\e[50G%scode%s", ANSI_DIM, ANSI_RESET);
		else if (i == 2)
			printf("\e[50G%sdata%s", ANSI_DIM, ANSI_RESET);
		else if (i == 3)
			printf("\e[51G%send%s", ANSI_DIM, ANSI_RESET);

		if (boffset < offset_code || offset_data <= boffset)
			printf("%s%s│", jump, ANSI_RESET);

		if (boffset == 0)
			printf(" Metadata");
		else if (boffset == offset_symtab)
			printf(" Symbol Table");
		else if (boffset == offset_data)
			printf(" Data");

		if (offset_code <= boffset && boffset < offset_data) {
			char *disassembled = wvm_disassemble(word);
			printf(" %s", disassembled);
			free(disassembled);
			printf("%s│", jump);
		}

		if (boffset == offset_code)
			printf(" Code");
		
		printf("\n");
	}

	printf("└───────┴────────────────────┴%s┘\n", sep);
}
