SYNTAX		= wasm.ne
COMPILED	= wasm.js
COMPILE		= nearleyc
TEST		= node test.js
EXAMPLE		= example.wasm
ALT			= example2.wasm

$(COMPILED): $(SYNTAX)
	$(COMPILE) $(SYNTAX) -o $(COMPILED)



test: $(COMPILED) $(EXAMPLE)
	$(TEST) $(EXAMPLE)

tree: $(COMPILED) $(EXAMPLE)
	$(TEST) $(EXAMPLE) -t

trees: $(COMPILED) $(EXAMPLE)
	$(TEST) $(EXAMPLE) -st



atest: $(COMPILED) $(ALT)
	$(TEST) $(ALT)

atree: $(COMPILED) $(ALT)
	$(TEST) $(ALT) -t

atrees: $(COMPILED) $(ALT)
	$(TEST) $(ALT) -st



ftest: $(COMPILED) fibonacci.wasm
	$(TEST) fibonacci.wasm

ftree: $(COMPILED) fibonacci.wasm
	$(TEST) fibonacci.wasm -t

ftrees: $(COMPILED) fibonacci.wasm
	$(TEST) fibonacci.wasm -st



clean:
	rm -f $(COMPILED)
