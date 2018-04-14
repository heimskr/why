SOURCES			= main.c wvm.c instruction.c ops.c registers.c
HEADERS			=        wvm.h instruction.h ops.h registers.h
OBJECTS			= main.o wvm.o instruction.o ops.o registers.o
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
