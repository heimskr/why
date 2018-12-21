SYNTAX		= llvm.ne
COMPILED	= llvm.js
COMPILE		= nearleyc
TEST		= tree.js
CHECK		= \x1b[32m\xe2\x9c\x94\x1b[0m
LLDIR		= ../wvm/c/ll

all: $(COMPILED) _

test: mtest

$(COMPILED): $(SYNTAX) _
	@ $(COMPILE) $(SYNTAX) -o $(COMPILED) && echo "$(CHECK) Compiled $(SYNTAX) to $(COMPILED)."

ttest: $(TEST) $(COMPILED) test.ll
	node $(TEST) test.ll

ntest: $(TEST) $(COMPILED) $(LLDIR)/net.ll
	node ll2w.js -d $(LLDIR)/net.ll

wtest: $(TEST) $(COMPILED) $(LLDIR)/wvm.ll
	node ll2w.js -d $(LLDIR)/wvm.ll

tmtest: $(TEST) $(COMPILED) $(LLDIR)/timer.ll
	node ll2w.js -d $(LLDIR)/timer.ll

mtest: $(TEST) $(COMPILED) $(LLDIR)/main.ll
	node ll2w.js -d $(LLDIR)/main.ll

itest: $(TEST) $(COMPILED) io.ll
	node ll2w.js -d io.ll

otest: $(TEST) $(COMPILED) optimized.ll
	node ll2w.js -d optimized.ll

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
