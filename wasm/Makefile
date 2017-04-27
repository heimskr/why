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

compiled/linkertest.why: compiled/doubler.wo compiled/fibonacci.wo examples/linkertest.wasm
	@ node wld.js examples/linkertest.wasm compiled/doubler.wo compiled/fibonacci.wo -o compiled/linkertest.why

compiled/%.wo: $(COMPILED) examples/%.wasm _
	@ node wasmc.js examples/$*.wasm compiled/$*.wo -l

fib:
	@ node wasmc.js examples/fibonacci.wasm compiled/fibonacci.wo -l

$(COMPILED): $(SYNTAX) _
	@ $(COMPILE) $(SYNTAX) -o $(COMPILED) && echo "$(CHECK) Compiled $(SYNTAX) to $(COMPILED)."

clean:
	rm -f $(COMPILED) compiled/*

_:
