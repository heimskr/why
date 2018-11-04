// Contains text constants for the UI.

module.exports = {
	halt: "Halts the VM.",
	prc: "Prints the contents of the register as a character.",
	prd: "Prints the contents of the register as a decimal number.",
	prx: "Prints the contents of the register as a hexadecimal number.",
	prb: "Prints the contents of the register as a binary number.",
	sleep: "Pauses execution for a given number of milliseconds.",
	seq: "If the value in rs is equal to the value in rt, rd is set to 1; otherwise, rd is set to 0.",
	add: "Adds the values in rs and rt and stores the result in rd.",
	sub: "Subtracts the value in rt from the value in rs and stores the result in rd.",
	jrc: "Jumps to the address stored in rd, provided the value in rs is nonzero.",
	or: "Computes the bitwise OR of rs and rt and stores the result in rd."
};
