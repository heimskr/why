#include <stdio.h>
#include <stdlib.h>
#include "wvm.h"

int main(int argc, char *argv[]) {
	if (!wvm_init(0xffffff)) {
		fprintf(stderr, "Couldn't allocate memory.\n");
		exit(1);
	}

	wvm_load("../../wasm/compiled/megafib.why");
	wvm_init_pc();

	wvm_print_memory();

	wvm_free();

	return 0;
}
