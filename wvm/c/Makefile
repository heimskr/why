SOURCES			= main.c wvm.c
HEADERS			= wvm.h
OBJECTS			= main.o
COMPILE			= gcc -g -c -std=c99 -Wall
LINK			= gcc -g -o

wvm: main.o wvm.o
	$(LINK) wvm main.o wvm.o

$(OBJECTS): $(SOURCES) $(HEADERS)
	$(COMPILE) $(SOURCES)

clean:
	rm -f wvm *.o

test: wvm
	./wvm
