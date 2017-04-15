SYNTAX		= wasm.ne
COMPILED	= wasm.js
COMPILE		= nearleyc
TEST		= node tree.js
EXAMPLE		= example.wasm
ALT			= example2.wasm
LIBRARIES	= fibonacci doubler string
LIBS		= $(patsubst %, compiled/%.wo, $(LIBRARIES))
CHECK		= \x1b[32m\xe2\x9c\x94\x1b[0m

all: $(COMPILED) $(LIBS) compiled/linkertest.why

compiled/linkertest.why: compiled/doubler.wo compiled/fibonacci.wo linkertest.wasm
	@ node wld.js linkertest.wasm compiled/doubler.wo compiled/fibonacci.wo -o compiled/linkertest.why

compiled/%.wo: $(COMPILED) %.wasm _
	@ node wasmc.js $*.wasm -lo compiled/$*.wo

fib:
	@ node wasmc.js fibonacci.wasm -lo compiled/fibonacci.wo

$(COMPILED): $(SYNTAX) _
	@ $(COMPILE) $(SYNTAX) -o $(COMPILED) && echo "$(CHECK) Compiled wasm.ne to wasm.js."

clean:
	rm -f $(COMPILED) compiled/*

_:
