#include "io.h"


int i = 5;
int retvar() { return i; }

int phitest() {
	int a = 0;
	if (i) {
		a++;
	}

	a += 2;

	if (!i) {
		a += 3;
	}

	return a - 1;
}

void _main() {
	int x = retvar();
	foo:
	strprint("hello\n");
	if (++x < i) {
		goto foo;
	}

	strprint("bye\n");
}

void strprint(char *str) {
	for (int i = 0; str[i] != '\0'; i++)
		_prc(str[i]);
}
