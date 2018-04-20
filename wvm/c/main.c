#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "wvm.h"

int main(int argc, char *argv[]) {
	if (!wvm_init(0xffffff)) {
		fprintf(stderr, "Couldn't allocate memory.\n");
		exit(1);
	}

	if (argc == 1) {
		wvm_load("../../wasm/examples/memory.why");
	} else if (argc == 2) {
		wvm_load(argv[1]);
	} else if (argc == 3 && strcmp(argv[1], "-e") == 0) {
		char *path = calloc(strlen("../../wasm/examples/") + strlen(argv[2]) + strlen(".why") + 1, sizeof(char));
		sprintf(path, "../../wasm/examples/%s.why", argv[2]);
		wvm_load(path);
	}
	wvm_init_vm();

	wvm_print_memory();

	while (wvm_tick());

	printf("\n\33[31mExecution halted.\33[0m\n");

	wvm_free();

	return 0;
}
