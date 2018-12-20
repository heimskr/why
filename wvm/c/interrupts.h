#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#include "instruction.h"
#include "wvm.h"

uword inttab;

int wvm_interrupt(imm_t id);
int wvm_force_interrupt(imm_t id, ring_t new_ring);
int check_inttab();
void int_protec();
int check_ring();

#define INTERRUPT_MAX	3

#define INT_NULL		0
#define INT_NULL_FROM	-1
#define INT_NULL_TO		-1

#define INT_SYSTEM		1
#define INT_SYSTEM_FROM	0
#define INT_SYSTEM_TO	-1

#define INT_TIMER		2
#define INT_TIMER_FROM	-1
#define INT_TIMER_TO	0

#define INT_PROTEC		3
#define INT_PROTEC_FROM	0
#define INT_PROTEC_TO	2

#endif
