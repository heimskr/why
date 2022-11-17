CHECK			:= none
COMPILER		?= clang++
# OPTIMIZATION	?= -Ofast -march=native
OPTIMIZATION	?= -O0 -ggdb3
CFLAGS			+= $(strip $(OPTIMIZATION) -Wall -Wextra -std=c++2a)
OUT				:= wvm
# TESTFILE		?= ~/src/thurisaz/Thurisaz.why
TESTFILE		?= ../wasmc/compiled/text_linked.why
# TESTFILE		?= ../wasmc/compiled/c--.why
# TESTFILE		?= ~/csmith/7.full.why

SOURCES_WVM		:= $(shell find -L src -name '*.cpp' | sed -nE '/(Tests?|Test_.+)\.cpp$$/!p')
OBJECTS_WVM		:= $(patsubst src/%.cpp,build/%.o, $(SOURCES_WVM))

SOURCES_HN		:= $(shell find haunted/src -name '*.cpp' | sed -nE '/(Tests?|Test_.+)\.cpp$$/!p') \
	haunted/src/lib/formicine/ansi.cpp haunted/src/lib/formicine/futil.cpp haunted/src/lib/formicine/performance.cpp
INCLUDE_HN		= -Ihaunted/include
OBJECTS_HN		:= $(patsubst haunted/src/%.cpp,haunted/build/%.o,$(SOURCES_HN))
CFLAGS_HN		:= -DDISABLE_PERFORMANCE

SOURCES			:= $(SOURCES_WVM) $(SOURCES_HN)
INCLUDE			:= -Iinclude -I../wasmc/include -Ihaunted/formicine $(INCLUDE_HN)
OBJECTS			:= $(OBJECTS_WVM) $(OBJECTS_HN)
LDFLAGS			+= $(strip -pthread $(LDFLAGS_EXTRA))

ifeq ($(CHECK), asan)
	CFLAGS  += -fsanitize=address -fno-omit-frame-pointer
	LDFLAGS += -fsanitize=address
else ifeq ($(CHECK), msan)
	CFLAGS  += -fsanitize=memory -fno-omit-frame-pointer
	LDFLAGS += -fsanitize=memory
endif

.PHONY: all clean count countbf memtest outtest regtest test

all: $(OUT)

$(OUT): $(OBJECTS)
	$(COMPILER) $(INCLUDE) $^ -o $@ $(LDFLAGS)

test: memtest

disk.img:
	dd if=/dev/zero of=disk.img bs=1024 count=10240

servertest: $(OUT)
	./$(OUT) server $(TESTFILE) disk.img disk.1.img

consoletest: $(OUT)
	./$(OUT) console 127.0.0.1 `cat .port`

memtest: $(OUT)
	./$(OUT) memory 127.0.0.1 `cat .port`

outtest: $(OUT)
	./$(OUT) output 127.0.0.1 `cat .port`

regtest: $(OUT)
	./$(OUT) registers 127.0.0.1 `cat .port`

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
