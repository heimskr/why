#include "io.h"


int i = 5;
int retvar() {
	int wow, uh;
	int j = i + 1;
	int k = j + 2;

	asm (
		"$a0 + $a1 -> $q"
		: "=r" (wow), "+l" (uh)
		: "a" (j), "a" (k)
		: "cc"
	);

	return wow;
}

void _main() {
	int x = retvar();
	foo:
	strprint("hello\n");
	if (++x < i) {
		goto foo;
	}

	strprint("bye");
	strprint("\n");
	int y = retvar();
	int z = retvar();
}

void strprint(char *str) {
	for (int i = 0; str[i] != '\0'; i++)
		_prc(str[i]);
}
