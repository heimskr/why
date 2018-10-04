def RetCC_WhyJS : CallingConv<[
	CCIfType<[i64], CCAssignToReg<[R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, RA, RB, RC, RD, RE, RF]>>,
	CCIfType<[i64], CCAssignToStack<8, 8>>
]>;

def CC_WhyJS : CallingConv<[
	CCIfType<[i8, i16, i32], CCPromoteToType<i64>>,
	CCIfType<[i64], CCAssignToReg<[A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, AA, AB, AC, AD, AE, AF]>>,
	CCIfType<[i64], CCAssignToStack<8, 8>>
]>;

