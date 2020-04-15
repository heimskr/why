COMPILER		?= clang++
OPTIMIZATION	?= -O0 -g
CFLAGS			:= $(strip $(OPTIMIZATION) -Wall -Wextra -std=c++2a)
OUT				:= wvm
MKBUILD			:= mkdir -p build

INCLUDE_ICU		:= $(shell pkg-config --cflags icu-uc)
LD_ICU			:= $(shell pkg-config --libs icu-uc)

SOURCES_WVM		:= $(shell find -L src -name '*.cpp' | sed -nE '/(tests?|test_.+)\.cpp$$/!p')
OBJECTS_WVM		:= $(patsubst src/%.cpp,build/%.o, $(SOURCES_WVM))

SOURCES_HN		:= $(shell find haunted/src -name '*.cpp' | sed -nE '/(tests?|test_.+)\.cpp$$/!p') \
	haunted/src/lib/formicine/ansi.cpp haunted/src/lib/formicine/futil.cpp haunted/src/lib/formicine/performance.cpp
INCLUDE_HN		= -Ihaunted/include $(INCLUDE_ICU)
OBJECTS_HN		:= $(patsubst haunted/src/%.cpp,haunted/build/%.o,$(SOURCES_HN))

SOURCES			:= $(SOURCES_WVM) $(SOURCES_HN)
INCLUDE			:= -Iinclude -Ihaunted/formicine $(INCLUDE_HN)
OBJECTS			:= $(OBJECTS_WVM) $(OBJECTS_HN)
LDFLAGS			:= $(LD_ICU)

.PHONY: all clean

all: $(OUT)

$(OUT): $(OBJECTS)
	$(COMPILER) $(LDFLAGS) $(INCLUDE) $^ -o $@

haunted/build/%.o: haunted/src/%.cpp
	@ mkdir -p "$(shell dirname "$@")"
	$(COMPILER) $(CFLAGS) $(INCLUDE_HN) -c $< -o $@

build/%.o: src/%.cpp
	$(MKBUILD)
	$(COMPILER) $(CFLAGS) $(INCLUDE) -c $< -o $@

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
