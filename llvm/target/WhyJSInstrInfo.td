def ADDrr : InstWhyJS<
	(outs GRRegs:$dst), (ins GRRegs:$src1, GRRegs:$src2),
	"$src + $src -> $dst",
	[(set i32:$dst, (add i32:$src1, i32:$src2))]
>;