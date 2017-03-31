SYNTAX		= wasm.ne
COMPILED	= wasm.js
COMPILE		= nearleyc
TEST		= node tree.js
EXAMPLE		= example.wasm
ALT			= example2.wasm

$(COMPILED): $(SYNTAX)
	$(COMPILE) $(SYNTAX) -o $(COMPILED)

test: $(EXAMPLE)
	$(TEST) $(EXAMPLE)

testj: $(EXAMPLE)
	$(TEST) $(EXAMPLE) -s



atest: $(ALT)
	$(TEST) $(ALT)

atestj: $(ALT)
	$(TEST) $(ALT) -s



ftest: fibonacci.wasm
	$(TEST) fibonacci.wasm

ftestj: fibonacci.wasm
	$(TEST) fibonacci.wasm -s



fcompile: $(COMPILED) fibonacci.wasm
	node wasmc.js fibonacci.wasm

imm: $(COMPILED) immtest.wasm
	node wasmc.js immtest.wasm

clean:
	rm -f $(COMPILED)
