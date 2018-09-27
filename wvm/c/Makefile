SOURCES			= main.c wvm.c instruction.c ops.c registers.c disassemble.c
HEADERS			= ansi.h wvm.h instruction.h ops.h registers.h disassemble.h
OBJECTS			= main.o wvm.o instruction.o ops.o registers.o disassemble.o
COMPILE			= gcc -g -c -std=c99 -Wall
LINK			= gcc -g -o

wvm: $(OBJECTS)
	$(LINK) wvm $(OBJECTS)

$(OBJECTS): $(SOURCES) $(HEADERS)
	$(COMPILE) $(SOURCES)

clean:
	rm -f wvm *.o

test: wvm
	./wvm

link:
	../../wasm/wld.js ../../wasm/examples/memory.wasm ../../wasm/examples/string.why -o ../../wasm/compiled/memory.why

../../wasm/compiled/string.why: ../../wasm/examples/string.wasm
	../../wasm/wld.js ../../wasm/examples/string.wasm -o ../../wasm/compiled/string.why

../../wasm/compiled/stringtest.why: ../../wasm/compiled/string.why ../../wasm/examples/stringtest.wasm
	../../wasm/wld.js ../../wasm/examples/stringtest.wasm -o ../../wasm/compiled/stringtest.why

ltest: link test

grind: wvm ../../wasm/compiled/stringtest.why
	valgrind --leak-check=full --show-leak-kinds=all ./wvm ../../wasm/compiled/stringtest.why
