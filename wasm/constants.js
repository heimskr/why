exports.EXCEPTIONS = ["dbz"];

exports.R_TYPES = [
	0b000000000001, // Math
	0b000000000010, // Logic
	0b000000001100, // Move From HI Register
	0b000000001100, // Move From LO Register
	0b000000001110, // Comparisons
	0b000000010001, // Jumps
	0b000000010010, // Memory
	0b000000011111, // Trap
];

exports.I_TYPES = [
	0b000000000011, // Add Immediate
	0b000000000100, // Subtract Immediate
	0b000000000101, // Multiply Immediate
	0b000000000110, // Bitwise AND Immediate
	0b000000000111, // Bitwise NAND Immediate
	0b000000001000, // Bitwise NOR Immediate
	0b000000001001, // Bitwise OR Immediate
	0b000000001010, // Bitwise XNOR Immediate
	0b000000001011, // Bitwise XOR Immediate
	0b000000001101, // Load Upper Immediate
	0b000000010011, // Load Immediate
	0b000000010100, // Store Immediate
	0b000000010101, // Set
	0b000000010110, // Add Immediate Unsigned
	0b000000010111, // Subtract Immediate Unsigned
	0b000000011000, // Multiply Immediate Unsigned
	0b000000011001, // Set on Less Than Immediate
	0b000000011010, // Set on Less Than or Equal Immediate
	0b000000011011, // Set on Equal Immediate
	0b000000011100, // Set on Less Than Immediate Unsigned
	0b000000011101, // Set on Less Than or Equal Immediate Unsigned
	0b000000011110, // Set on Equal Immediate Unsigned
];

exports.J_TYPES = [
	0b000000001111, // Jump
	0b000000010000, // Jump Conditional
	0b000000100000, // Jump and Link
	0b000000100001, // Jump and Link Conditional
];

exports.OPCODES = {
	add:    0b000000000001,
	addu:   0b000000000001,
	sub:    0b000000000001,
	subu:   0b000000000001,
	mult:   0b000000000001,
	multu:  0b000000000001,
	and:    0b000000000010,
	nand:   0b000000000010,
	nor:    0b000000000010,
	not:    0b000000000010,
	or:     0b000000000010,
	xnor:   0b000000000010,
	xor:    0b000000000010,
	addi:   0b000000000011,
	subi:   0b000000000100,
	multi:  0b000000000101,
	andi:   0b000000000110,
	nandi:  0b000000000111,
	nori:   0b000000001000,
	ori:    0b000000001001,
	xnori:  0b000000001010,
	xori:   0b000000001011,
	lui:    0b000000001101,
	sl:     0b000000001110,
	sle:    0b000000001110,
	seq:    0b000000001110,
	slu:    0b000000001110,
	sleu:   0b000000001110,
	sequ:   0b000000001110,
	j:      0b000000001111,
	jc:     0b000000010000,
	jr:     0b000000010001,
	jrc:    0b000000010001,
	c:      0b000000010010,
	l:      0b000000010010,
	s:      0b000000010010,
	li:     0b000000010011,
	si:     0b000000010100,
	set:    0b000000010101,
	addiu:  0b000000010110,
	subiu:  0b000000010111,
	multiu: 0b000000011000,
	sli:    0b000000011001,
	slei:   0b000000011010,
	seqi:   0b000000011011,
	sliu:   0b000000011100,
	sleiu:  0b000000011101,
	seqiu:  0b000000011110,
	trap:   0b000000011111,
	jl:     0b000000100000,
	jlc:    0b000000100001,
};

exports.FUNCTS = {
	add:   0b000000000000,
	and:   0b000000000000,
	c:     0b000000000000,
	jr:    0b000000000000,
	sl:    0b000000000000,
	jrc:   0b000000000001,
	l:     0b000000000001,
	nand:  0b000000000001,
	sle:   0b000000000001,
	sub:   0b000000000001,
	jrl:   0b000000000010,
	mult:  0b000000000010,
	nor:   0b000000000010,
	s:     0b000000000010,
	seq:   0b000000000010,
	addu:  0b000000000011,
	jrlc:  0b000000000011,
	not:   0b000000000011,
	slu:   0b000000000011,
	or:    0b000000000100,
	sleu:  0b000000000100,
	subu:  0b000000000100,
	multu: 0b000000000101,
	sequ:  0b000000000101,
	xnor:  0b000000000101,
	xor:   0b000000000110,
};

exports.REGISTER_OFFSETS = {
	0: 0,
	zero: 0,
	g: 1,
	sp: 2,
	stack: 2,
	fp: 3,
	ra: 4,
	return: 4,
	lo: 5,
	hi: 6,
	r: 7,
	a: 23,
	t: 39,
	s: 62,
	k: 85,
	m: 102,
	f: 118,
	e: 122
};

exports.TRAPS = {
	printr: 0b0000000000000001, // print contents of register whose ID is stored in $a0.
	halt:   0b0000000000000010, // tells the vm to stop
};

// largest number of arguments allowed for a subroutine call.
// equal to the number of arguments registers.
exports.MAX_ARGS = exports.REGISTER_OFFSETS.t - exports.REGISTER_OFFSETS.a;

exports.FLAGS = {
	IGNORE: 0,
	ADJUST_ADDRESS: 1
};
