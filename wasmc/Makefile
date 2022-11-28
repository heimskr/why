COMPILER        ?= clang++
DEBUGGER        ?= lldb
OPTIMIZATION    ?= -Ofast -march=native
STANDARD        ?= c++20
WARNINGS        ?= -Wall -Wextra
CFLAGS          := -std=$(STANDARD) $(OPTIMIZATION) $(WARNINGS) -Iinclude
OUTPUT          ?= wasmc
LEXFLAGS        := -Wno-sign-compare -Wno-register
WASMLEXCPP      := src/parser/wasmlex.cpp
WASMPARSECPP    := src/parser/wasmparse.cpp
WASMPARSEHDR    := include/wasmparse.h
WASMFLEXSRC     := src/parser/wasm.l
WASMBISONSRC    := src/parser/wasm.y

CLOC_OPTIONS    := --exclude-dir=.vscode,PicoSHA2,tools --not-match-f='^(yy|wasm)(lex|parse)|pico|strnat'
BISON_OPTIONS   := --color=always
SOURCES         := $(filter-out src/DebugSearch.cpp src/DumpSymbols.cpp src/DumpRelocation.cpp src/main.cpp,$(shell find src/**/*.cpp src/*.cpp))
OBJECTS         := $(SOURCES:.cpp=.o) $(WASMLEXCPP:.cpp=.o) $(WASMPARSECPP:.cpp=.o)
TESTBASE		?= example+-
TESTFILE        ?= examples/$(TESTBASE).wasm
OUTFILE         ?= compiled/$(TESTBASE).why
TESTARGS        ?= $(TESTFILE) $(OUTFILE)
# TESTARGS        ?= -l compiled/text_linked.why compiled/text.why compiled/text2.why
# TESTARGS        ?= -l compiled/text_linked2.why compiled/text_linked.why compiled/example4.why
# TESTARGS        ?= -l ../wasm/compiled/debug.why ../wasm/examples/debug.wasm
# TESTARGS        ?= -l ../wasm/compiled/a.why ../wasm/examples/a.wasm
# TESTARGS        ?= --test

.PHONY: all counter test clean count countbf

all: $(OUTPUT) ds++ syms reloc

$(OUTPUT): $(OBJECTS) src/main.o
	$(COMPILER) -o $@ $^ $(LDFLAGS)

ds++: $(OBJECTS) src/DebugSearch.o
	$(COMPILER) -o $@ $^ $(LDFLAGS)

syms: $(OBJECTS) src/DumpSymbols.o
	$(COMPILER) -o $@ $^ $(LDFLAGS)

reloc: $(OBJECTS) src/DumpRelocation.o
	$(COMPILER) -o $@ $^ $(LDFLAGS)

$(WASMLEXCPP): $(WASMFLEXSRC) $(WASMPARSEHDR)
	flex --prefix=wasm --outfile=$(WASMLEXCPP) $(WASMFLEXSRC)

$(WASMPARSECPP) $(WASMPARSEHDR): $(WASMBISONSRC)
	bison $(BISON_OPTIONS) --defines=$(WASMPARSEHDR) --output=$(WASMPARSECPP) $(WASMBISONSRC)

$(WASMLEXCPP:.cpp=.o): $(WASMLEXCPP)
	$(COMPILER) $(CFLAGS) $(LEXFLAGS) -c $< -o $@

$(WASMPARSECPP:.cpp=.o): $(WASMPARSECPP) $(WASMPARSEHDR)
	$(COMPILER) $(CFLAGS) $(LEXFLAGS) -c $< -o $@

%.o: %.cpp include/wasmparse.h
	$(COMPILER) $(CFLAGS) -c $< -o $@

counter:
	bison -Wcounterexamples $(BISON_OPTIONS) --defines=$(WASMPARSEHDR) --output=$(WASMPARSECPP) $(WASMBISONSRC)

test: $(OUTPUT)
	./$< $(TESTARGS)

clean:
	rm -f $(OUTPUT) ds++ syms src/*.o src/**/*.o graph_*.png PVS-Studio.log report.tasks strace_out \
		$(WASMPARSEHDR) $(WASMPARSECPP) $(WASMLEXCPP) $(WASMPARSECPP:.c=.output) $(WASMLEXCPP) $(WASMPARSECPP)

grind: $(OUTPUT)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --show-reachable=no ./$< $(TESTFILE)

count:
	cloc . parser $(CLOC_OPTIONS)

countbf:
	cloc --by-file . parser $(CLOC_OPTIONS)

DEPFILE  = .dep
DEPTOKEN = "\# MAKEDEPENDS"
DEPFLAGS = -f $(DEPFILE) -s $(DEPTOKEN)

depend:
	@ echo $(DEPTOKEN) > $(DEPFILE)
	makedepend $(DEPFLAGS) -- $(COMPILER) $(CFLAGS) -- $(SOURCES) 2>/dev/null
	@ rm $(DEPFILE).bak

sinclude $(DEPFILE)
