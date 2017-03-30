SYNTAX		= wasm.ne
COMPILED	= wasm.js
COMPILE		= nearleyc
TEST		= node test.js
EXAMPLE		= example.wasm
ALT			= example2.wasm

$(COMPILED): $(SYNTAX)
	$(COMPILE) $(SYNTAX) -o $(COMPILED)

test: $(EXAMPLE)
	$(TEST) $(EXAMPLE)

tree: $(EXAMPLE)
	$(TEST) $(EXAMPLE) -t

trees: $(EXAMPLE)
	$(TEST) $(EXAMPLE) -st



atest: $(ALT)
	$(TEST) $(ALT)

atree: $(ALT)
	$(TEST) $(ALT) -t

atrees: $(ALT)
	$(TEST) $(ALT) -st



ftest: fibonacci.wasm
	$(TEST) fibonacci.wasm

ftree: fibonacci.wasm
	$(TEST) fibonacci.wasm -t

ftrees: fibonacci.wasm
	$(TEST) fibonacci.wasm -st



fcompile: $(COMPILED) fibonacci.wasm
	node wasmc.js fibonacci.wasm



clean:
	rm -f $(COMPILED)
