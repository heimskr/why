COMPILER		?= clang++
OPTIMIZATION	?= -O0 -g
CFLAGS			:= $(strip $(OPTIMIZATION) -Wall -Wextra -std=c++2a)
OUT				:= wvm

SOURCES_WVM		:= $(shell find -L src -name '*.cpp' | sed -nE '/(tests?|test_.+)\.cpp$$/!p')
OBJECTS_WVM		:= $(patsubst src/%.cpp,build/%.o, $(SOURCES_WVM))

SOURCES_HN		:= $(shell find haunted/src -name '*.cpp' | sed -nE '/(tests?|test_.+)\.cpp$$/!p') \
	haunted/src/lib/formicine/ansi.cpp haunted/src/lib/formicine/futil.cpp haunted/src/lib/formicine/performance.cpp
INCLUDE_HN		= -Ihaunted/include
OBJECTS_HN		:= $(patsubst haunted/src/%.cpp,haunted/build/%.o,$(SOURCES_HN))
CFLAGS_HN		:= -DDISABLE_PERFORMANCE

SOURCES			:= $(SOURCES_WVM) $(SOURCES_HN)
INCLUDE			:= -Iinclude -Ihaunted/formicine $(INCLUDE_HN)
OBJECTS			:= $(OBJECTS_WVM) $(OBJECTS_HN)
LDFLAGS			:= $(strip -pthread $(LDFLAGS_EXTRA))

.PHONY: all clean count countbf memtest test

all: $(OUT)

$(OUT): $(OBJECTS)
	$(COMPILER) $(INCLUDE) $^ -o $@ $(LDFLAGS)

test: memtest

servertest: $(OUT)
	./$(OUT) server ../../wasm/compiled/mal.why

consoletest: $(OUT)
	./$(OUT) console 127.0.0.1 `cat .port`

memtest: $(OUT)
	./$(OUT) memory 127.0.0.1 `cat .port`

haunted/build/%.o: haunted/src/%.cpp
	@ mkdir -p "$(shell dirname "$@")"
	$(COMPILER) $(strip $(CFLAGS) $(INCLUDE_HN) $(CFLAGS_HN)) -c $< -o $@

build/%.o: src/%.cpp
	@ mkdir -p "$(shell dirname "$@")"
	$(COMPILER) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(OUT)

count:
	cloc src include $(CLOC_OPTIONS)

countbf:
	cloc --by-file src include $(CLOC_OPTIONS)

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
