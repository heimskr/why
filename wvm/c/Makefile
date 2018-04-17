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
	../../wasm/wld.js ../../wasm/examples/memory.wasm ../../wasm/examples/string.why -o ../../wasm/examples/memory.why

ltest: link test
