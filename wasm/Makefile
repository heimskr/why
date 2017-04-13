SYNTAX		= wasm.ne
COMPILED	= wasm.js
COMPILE		= nearleyc
TEST		= node tree.js
EXAMPLE		= example.wasm
ALT			= example2.wasm
LIBRARIES	= fibonacci doubler
LIBS		= $(patsubst %, compiled/%.wo, $(LIBRARIES))

all: $(COMPILED) $(LIBS) compiled/linkertest.why

compiled/%.wo: %.wasm _
	@ node wasmc.js $*.wasm -lo compiled/$*.wo

compiled/linkertest.why: compiled/doubler.wo compiled/fibonacci.wo linkertest.wasm
	@ node wld.js linkertest.wasm compiled/doubler.wo compiled/fibonacci.wo -o compiled/linkertest.why

fib:
	@ node wasmc.js fibonacci.wasm -lo compiled/fibonacci.wo

$(COMPILED): $(SYNTAX) _
	@ $(COMPILE) $(SYNTAX) -o $(COMPILED)

clean:
	rm -f $(COMPILED) compiled/*

_:
