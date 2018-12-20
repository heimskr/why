MAIN_SRC	= $(wildcard *.c)
SOURCES		= $(MAIN_SRC) ext/net.c
HEADERS		= $(wildcard *.h) ext/net.h
OBJECTS		= $(MAIN_SRC:.c=.o) net.o
COMPILE			= gcc -g -c -std=c99 -Wall
LINK			= gcc -g -o

WASM			= ../../wasm
WASMC			= $(WASM)/wasmc.js
WLD				= $(WASM)/wld.js
COMPILED		= $(WASM)/compiled
EXAMPLES		= $(WASM)/examples

wvm: $(OBJECTS)
	$(LINK) wvm $(OBJECTS)

$(OBJECTS): $(SOURCES) $(HEADERS)
	$(COMPILE) $(SOURCES)

clean:
	rm -f wvm *.o

test: wvm
	./wvm -c interrupts

link:
	$(WLD) $(EXAMPLES)/memory.wasm $(EXAMPLES)/string.why -o $(COMPILED)/memory.why

ir: $(SOURCES) $(HEADERS)
	clang -S -emit-llvm $(SOURCES)

ir1: $(SOURCES) $(HEADERS)
	clang -O1 -S -emit-llvm $(SOURCES)

ir2: $(SOURCES) $(HEADERS)
	clang -O2 -S -emit-llvm $(SOURCES)

ir3: $(SOURCES) $(HEADERS)
	clang -O3 -S -emit-llvm $(SOURCES)

$(COMPILED)/string.why: $(EXAMPLES)/string.wasm
	$(WASMC) $(EXAMPLES)/string.wasm $(COMPILED)/string.why

$(COMPILED)/stringtest.why: $(COMPILED)/string.why $(EXAMPLES)/stringtest.wasm
	$(WLD) $(EXAMPLES)/stringtest.wasm -o $(COMPILED)/stringtest.why

ltest: link test

grind: wvm $(COMPILED)/stringtest.why
	valgrind --leak-check=full --show-leak-kinds=all ./wvm $(COMPILED)/stringtest.why
