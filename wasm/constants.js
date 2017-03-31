exports.EXCEPTIONS = ["dbz"];

exports.R_TYPES = [
	0b000000000001, // Math
	0b000000000010, // Logic
	0b000000001100, // Move From HI Register
	0b000000001100, // Move From LO Register
	0b000000001110, // Comparisons
	0b000000010001, // Jump to Register
	0b000000010010, // Memory
];

exports.I_TYPES = [
	0b000000000011, // Addition Immediate
	0b000000000100, // Subtraction Immediate
	0b000000000101, // Multiplication Immediate
	0b000000000110, // Bitwise AND Immediate
	0b000000000111, // Bitwise NAND Immediate
	0b000000001000, // Bitwise NOR Immediate
	0b000000001001, // Bitwise OR Immediate
	0b000000001010, // Bitwise XNOR Immediate
	0b000000001011, // Bitwise XOR Immediate
	0b000000001101, // Load Upper Immediate

];

exports.J_TYPES = [
	0b000000001111, // Jump
	0b000000010000, // Jump Conditional
];

exports.OPS = {
	add:   0b000000000001,
	sub:   0b000000000001,
	mult:  0b000000000001,
	and:   0b000000000010,
	nand:  0b000000000010,
	nor:   0b000000000010,
	not:   0b000000000010,
	or:    0b000000000010,
	xnor:  0b000000000010,
	xor:   0b000000000010,
	addi:  0b000000000011,
	subi:  0b000000000100,
	multi: 0b000000000101,
	andi:  0b000000000110,
	nandi: 0b000000000111,
	nori:  0b000000001000,
	ori:   0b000000001001,
	xnori: 0b000000001010,
	xori:  0b000000001011,
	mfhi:  0b000000001100,
	mflo:  0b000000001100,
	lui:   0b000000001101,
	sl:    0b000000001110,
	sle:   0b000000001110,
	seq:   0b000000001110,
	jump:  0b000000001111,
	jc:    0b000000010000,
	jr:    0b000000010001,
	c:     0b000000010010,
	l:     0b000000010010,
	s:     0b000000010010,
};

exports.FUNCTS = {
	add:  0b000000000000,
	and:  0b000000000000,
	c:    0b000000000000,
	jr:   0b000000000000,
	mfhi: 0b000000000000,
	sl:   0b000000000000,
	l:    0b000000000001,
	mflo: 0b000000000001,
	nand: 0b000000000001,
	sle:  0b000000000001,
	sub:  0b000000000001,
	mult: 0b000000000010,
	nor:  0b000000000010,
	s:    0b000000000010,
	seq:  0b000000000010,
	not:  0b000000000011,
	or:   0b000000000100,
	xnor: 0b000000000101,
	xor:  0b000000000110,
};

exports.REGISTER_OFFSETS = {
	zero: 0,
	stack: 2,
	return: 4,
	r: 5,
	a: 21,
	t: 37,
	s: 61,
	e: 122
};