SYNTAX		= wasm.ne
COMPILED	= wasm.js
COMPILE		= nearleyc
TEST		= node tree.js
EXAMPLE		= example.wasm
ALT			= example2.wasm
LIBRARIES	= fibonacci double
LIBS		= $(patsubst %, compiled/%.wo, $(LIBRARIES))

all: $(COMPILED) $(LIBS)

compiled/%.wo: %.wasm
	node wasmc.js $*.wasm -lo compiled/$*.wo

$(COMPILED): $(SYNTAX)
	$(COMPILE) $(SYNTAX) -o $(COMPILED)

clean:
	rm -f $(COMPILED) compiled/*
