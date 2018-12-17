SYNTAX		= llvm.ne
COMPILED	= llvm.js
COMPILE		= nearleyc
TEST		= tree.js
CHECK		= \x1b[32m\xe2\x9c\x94\x1b[0m

all: $(COMPILED) _

$(COMPILED): $(SYNTAX) _
	@ $(COMPILE) $(SYNTAX) -o $(COMPILED) && echo "$(CHECK) Compiled $(SYNTAX) to $(COMPILED)."

test: $(TEST) $(COMPILED) test.ll
	node $(TEST) test.ll

itest: $(TEST) $(COMPILED) io.ll
	node ll2w.js -d io.ll

itree: $(TEST) $(COMPILED) io.ll
	node $(TEST) io.ll

stest: $(TEST) $(COMPILED) syntax.ll
	node $(TEST) syntax.ll

qtest: $(TEST) $(COMPILED) test.ll
	node $(TEST) test.ll -q

ptest: $(TEST) $(COMPILED) printf.ll
	node $(TEST) printf.ll

io.ll: test/io.c
	clang -S -emit-llvm test/io.c

clean:
	rm -f $(COMPILED)

_:
