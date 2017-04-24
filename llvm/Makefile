SYNTAX		= llvm.ne
COMPILED	= llvm.js
COMPILE		= nearleyc
TEST		= tree.js
CHECK		= \x1b[32m\xe2\x9c\x94\x1b[0m

all: $(COMPILED)

$(COMPILED): $(SYNTAX) _
	@ $(COMPILE) $(SYNTAX) -o $(COMPILED) && echo "$(CHECK) Compiled $(SYNTAX) to $(COMPILED)."

test: $(TEST) $(COMPILED) test.ll
	node $(TEST) test.ll

stest: $(TEST) $(COMPILED) syntax.ll
	node $(TEST) syntax.ll

qtest: $(TEST) $(COMPILED) test.ll
	node $(TEST) test.ll -q

clean:
	rm -f $(COMPILED)

_:
