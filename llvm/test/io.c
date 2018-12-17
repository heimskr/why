#include "io.h"


int i = 5;
int retvar() { return i; }

void _main() {
	int x = _gettime();
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
