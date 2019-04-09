SYNTAX		= llvm.ne
COMPILED	= llvm.js
COMPILE		= nearleyc
TEST		= tree.js
CHECK		= \x1b[32m\xe2\x9c\x94\x1b[0m
LLDIR		= ../wvm/c/ll
LL2W		= tsnode ll2w.ts

all: $(COMPILED) _

test: dtest

$(COMPILED): $(SYNTAX) _
	@ $(COMPILE) $(SYNTAX) -o $(COMPILED) && echo "$(CHECK) Compiled $(SYNTAX) to $(COMPILED)."

atest: $(TEST) $(COMPILED) asm.ll
	node $(TEST) asm.ll

ttest: $(TEST) $(COMPILED) test.ll
	node $(TEST) test.ll

rtest: $(TEST) $(COMPILED) regalloc.ll
	$(LL2W) -d regalloc.ll

ntest: $(TEST) $(COMPILED) $(LLDIR)/net.ll
	$(LL2W) -d $(LLDIR)/net.ll

wtest: $(TEST) $(COMPILED) $(LLDIR)/wvm.ll
	$(LL2W) -d $(LLDIR)/wvm.ll

tmtest: $(TEST) $(COMPILED) $(LLDIR)/timer.ll
	$(LL2W) -d $(LLDIR)/timer.ll

mtest: $(TEST) $(COMPILED) $(LLDIR)/main.ll
	$(LL2W) -d $(LLDIR)/main.ll

itest: $(TEST) $(COMPILED) io.ll
	$(LL2W) -d io.ll

otest: $(TEST) $(COMPILED) optimized.ll
	$(LL2W) -d optimized.ll

itree: $(TEST) $(COMPILED) io.ll
	node $(TEST) io.ll

stest: $(TEST) $(COMPILED) syntax.ll
	node $(TEST) syntax.ll

qtest: $(TEST) $(COMPILED) test.ll
	node $(TEST) test.ll -q

ptest: $(TEST) $(COMPILED) printf.ll
	node $(TEST) printf.ll

ltest: $(TEST) $(COMPILED) liveness.ll
	$(LL2W) liveness.ll

dtest: $(TEST) $(COMPILED) $(LLDIR)/disassemble.ll
	$(LL2W) $(LLDIR)/disassemble.ll

%.ll: test/%.c
	clang -S -emit-llvm $<

clean:
	rm -f $(COMPILED)

_:
