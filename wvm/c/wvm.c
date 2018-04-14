#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "wvm.h"

static lomg *memory;
static lomg pc; // in words, not bytes.

/**
 * Allocates memory for the VM.
 * @param length The number of longs to allocate.
 */
int wvm_init(lomg length) {
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

	char line[20];
	int i = 0;
	while (fgets(line, 18, file)) {
		lomg instr = strtol(line, NULL, 16);
		memory[i++] = instr;
	}

	return i;
}

/**
 * Sets the initial position of the program counter.
 */
void wvm_init_pc() {
	pc = memory[2] >> 3;
	printf("PC = %lld\n", pc);
}
