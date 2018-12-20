#include <pthread.h>
#include <unistd.h>

#include "interrupts.h"
#include "timer.h"

pthread_t timer_thread;

int timer_active = 0;
uword timer_delay;

static void *wvm_timer_thread(void *args) {
	timer_active = 1;
	usleep(timer_delay);
	wvm_force_interrupt(INT_TIMER, INT_TIMER_TO);
	timer_active = 0;
	return NULL;
}

int wvm_set_timer(uword us) {
	if (timer_active == 1) {
		return 0;
	}

	timer_delay = us;
	int status = pthread_create(&timer_thread, NULL, wvm_timer_thread, NULL);

#ifdef WVM_DEBUG
	if (status != 0) {
		fprintf(stderr, "Status code of pthread_create() in wvm_set_timer: %d\n", status);
	}
#endif

	return status;
}
