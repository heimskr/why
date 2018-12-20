#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "interrupts.h"
#include "timer.h"

pthread_mutex_t timer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t timer_thread;

uword timer_delay;

static void *wvm_timer_thread(void *args) {
	usleep(timer_delay);
	wvm_force_interrupt(INT_TIMER, INT_TIMER_TO);
	pthread_mutex_unlock(&timer_mutex);
	return NULL;
}

int wvm_set_timer(uword us) {
	if (pthread_mutex_trylock(&timer_mutex) == EBUSY) {
		// Only one timer at a time.
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
