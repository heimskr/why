CC				?= clang++
SOURCES			:= $(shell find -L src -name '*.cpp' | sed -nE '/(tests?|test_.+)\.cpp$$/!p')
OBJECTS			:= $(patsubst src/%.cpp,build/%.o, $(SOURCES))
INCLUDE			:= -Iinclude -Ihaunted/formicine
OPTIMIZATION	?= -O0 -g
CFLAGS			:= $(OPTIMIZATION) -Wall -Wextra -std=c++2a
OUT				:= wvm
MKBUILD			:= mkdir -p build

.PHONY: all clean

all: $(OUT)

$(OUT): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@

build/%.o: src/%.cpp
	$(MKBUILD)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(OUT)

DEPFILE  = .dep
DEPTOKEN = "\# MAKEDEPENDS"
DEPFLAGS = -Y -f $(DEPFILE) -s $(DEPTOKEN)

depend:
	@ echo $(SOURCES); echo
	@ echo $(DEPTOKEN) > $(DEPFILE)
	makedepend $(DEPFLAGS) -- $(CC) $(INCLUDE) -- $(SOURCES) $(EXTRASRC) 2>/dev/null
	@ sed -i.sed 's/^src\//build\//' $(DEPFILE)
	@ rm $(DEPFILE).bak $(DEPFILE).sed

sinclude $(DEPFILE)
