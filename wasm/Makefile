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

atest: $(COMPILED) $(ALT)
	$(TEST) $(ALT)

atree: $(COMPILED) $(ALT)
	$(TEST) $(ALT) -t

clean:
	rm -f $(COMPILED)
