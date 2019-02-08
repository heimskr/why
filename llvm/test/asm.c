#include "../whysa.h"

int i = 5;
int test1() {
	int wow, uh;
	int j = i + 1;
	int k = j - 2;

	asm (
		"%2 + %3 -> %0; [%4] -> %1; ring %3; ring %[foo]"
		: "=r" (wow), "=y" (uh)
		: "a" (j), "b" (k), [foo] "e" (664L)
		: "cc"
	);


	return wow;
}

void test2() {
	asm ("ring $a0" : : "a" (i));
}
