#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "wvm.h"

int main(int argc, char *argv[]) {
	if (!wvm_init(0xffffff)) {
		fprintf(stderr, "Couldn't allocate memory.\n");
		exit(1);
	}

	wvm_load("../../wasm/examples/fizzbuzz.why");
	wvm_init_vm();

	wvm_print_memory();

	while (wvm_tick());

	printf("\n\33[31mExecution halted.\33[0m\n");

	wvm_free();

	return 0;
}
