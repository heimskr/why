#include "interrupts.h"
#include <stdio.h>

#define TOFROM(id) case INT_##id: to = INT_##id##_TO; from = INT_##id##_FROM; break

uword inttab = 0;

/**
 * Performs an interrupt.
 * @param id The ID of the interrupt.
 * @return Whether to increment the counter after performing the interrupt. (Useful only for software interrupts.)
 */
int wvm_interrupt(imm_t id) {
	if (INTERRUPT_MAX < id) {
		fprintf(stderr, "Unknown interrupt: %u\n", id);
		exit(1);
	}

	if (inttab == 0) {
#ifdef INTERRUPTS_DEBUG
		fprintf(stderr, "Warning: no interrupt table registered; ignoring interrupt.\n");
#endif
		return 1;
	}

	if (id == INT_NULL) {
		return 1;
	}

	ring_t to, from;
	switch (id) {
		TOFROM(SYSTEM);
		TOFROM(TIMER);
		TOFROM(PROTEC);
	}

	if (from < cur_ring) {
#ifdef INTERRUPTS_DEBUG
		fprintf(stderr, "Insufficient privilege for interrupt %u (currently %d, required %d); ignoring interrupt.\n",
			id, cur_ring, from);
#endif
		return 1;
	}

	registers[R_E0] = pc + 8;
	registers[R_E1] = cur_ring;

	if (-1 < to) {
		cur_ring = to;
	}

	wvm_jump(inttab + 8 * id);
	return 0;
}

void int_protec() {
	wvm_interrupt(INT_PROTEC);
}

int check_kernel() {
	if (RING_KERNEL < cur_ring) {
		int_protec();
		return 0;
	}

	return 1;
}

