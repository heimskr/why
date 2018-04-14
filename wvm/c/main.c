#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "wvm.h"

int main(int argc, char *argv[]) {
	if (!wvm_init(0xffffff)) {
		fprintf(stderr, "Couldn't allocate memory.\n");
		exit(1);
	}

	wvm_load("ctest.why");
	wvm_init_vm();
	wvm_print_memory();

	do {
		// printf("\33[2J\33[H");
		// wvm_print_memory();
		// printf("(%04lld)\n", pc);
		// usleep(50);
	} while (wvm_tick());

	printf("\33[31mExecution halted.\33[0m\n");

	wvm_free();

	return 0;
}
