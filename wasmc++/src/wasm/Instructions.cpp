#include "parser/Lexer.h"
#include "wasm/Instructions.h"

namespace Wasmc {
	std::unordered_set<Opcode> RTYPES {
		0b000000000001, // Math
		0b000000000010, // Logic
		0b000000001100, // Move From HI Register, Move From LO Register
		0b000000001110, // Comparisons
		0b000000010001, // Jumps
		0b000000010010, // Memory
		0b000000011111, // External
		0b000000110000, // Set Timer
		0b000000110010, // Change Ring
		0b000000111000, // Select
		0b000000111101, // Paging
		0b000001000001, // Query
	};

	std::unordered_set<Opcode> ITYPES {
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
		0b000000011110, // Modulo Unsigned Immediate
		0b000000100000, // Interrupt
		0b000000100001, // Register Interrupt Table
		0b000000100010, // Shift Left Logical Immediate
		0b000000100011, // Shift Right Logical Immediate
		0b000000100100, // Shift Right Arithmetic Immediate
		0b000000100101, // Load Byte Immediate
		0b000000100110, // Store Byte Immediate
		0b000000100111, // Load Indirect Immediate
		0b000000101000, // Load Byte Indirect Immediate
		0b000000101001, // Set on Greater Than Immediate
		0b000000101010, // Set on Greater Than or Equal Immediate
		0b000000101011, // Compare Immediate
		0b000000110001, // Set Timer Immediate
		0b000000110011, // Change Ring Immediate
		0b000000110100, // Divide Immediate
		0b000000110101, // Divide Unsigned Immediate
		0b000000110110, // Divide Inverse Immediate
		0b000000110111, // Divide Unsigned Inverse Immediate
		0b000000111001, // Sized Stack Push
		0b000000111010, // Sized Stack Pop
		0b000000111011, // Set on Greater Than or Equal Unsigned Immediate
		0b000000111100, // Set on Greater Than Unsigned Immediate
		0b000000111110, // Shift Left Logical Inverse Immediate
		0b000000111111, // Shift Right Logical Inverse Immediate
		0b000001000000, // Shift Right Arithmetic Inverse Immediate
	};

	std::unordered_set<Opcode> JTYPES {
		0b000000001111, // Jump
		0b000000010000, // Jump Conditional
		0b000000101100, // Jump If Positive
		0b000000101101, // Jump If Negative
		0b000000101110, // Jump If Zero
		0b000000101111, // Jump If Nonzero
	};

	std::unordered_map<std::string, Opcode> OPCODES {
		{"nop",    0b000000000000},
		{"add",    0b000000000001},
		{"sll",    0b000000000001},
		{"srl",    0b000000000001},
		{"sra",    0b000000000001},
		{"sub",    0b000000000001},
		{"mult",   0b000000000001},
		{"multu",  0b000000000001},
		{"mod",    0b000000000001},
		{"div",    0b000000000001},
		{"divu",   0b000000000001},
		{"and",    0b000000000010},
		{"nand",   0b000000000010},
		{"nor",    0b000000000010},
		{"not",    0b000000000010},
		{"or",     0b000000000010},
		{"xnor",   0b000000000010},
		{"xor",    0b000000000010},
		{"land",   0b000000000010},
		{"lnand",  0b000000000010},
		{"lnor",   0b000000000010},
		{"lnot",   0b000000000010},
		{"lor",    0b000000000010},
		{"lxnor",  0b000000000010},
		{"lxor",   0b000000000010},
		{"addi",   0b000000000011},
		{"subi",   0b000000000100},
		{"multi",  0b000000000101},
		{"andi",   0b000000000110},
		{"nandi",  0b000000000111},
		{"nori",   0b000000001000},
		{"ori",    0b000000001001},
		{"xnori",  0b000000001010},
		{"xori",   0b000000001011},
		{"lui",    0b000000001101},
		{"sl",     0b000000001110},
		{"sle",    0b000000001110},
		{"seq",    0b000000001110},
		{"slu",    0b000000001110},
		{"sleu",   0b000000001110},
		{"cmp",    0b000000001110},
		{"j",      0b000000001111},
		{"jc",     0b000000010000},
		{"jr",     0b000000010001},
		{"jrc",    0b000000010001},
		{"jrl",    0b000000010001},
		{"jrlc",   0b000000010001},
		{"c",      0b000000010010},
		{"cb",     0b000000010010},
		{"ch",     0b000000010010},
		{"l",      0b000000010010},
		{"lb",     0b000000010010},
		{"lh",     0b000000010010},
		{"s",      0b000000010010},
		{"sb",     0b000000010010},
		{"sh",     0b000000010010},
		{"spush",  0b000000010010},
		{"spop",   0b000000010010},
		{"ms",     0b000000010010},
		{"li",     0b000000010011},
		{"si",     0b000000010100},
		{"set",    0b000000010101},
		{"multui", 0b000000011000},
		{"sli",    0b000000011001},
		{"slei",   0b000000011010},
		{"seqi",   0b000000011011},
		{"slui",   0b000000011100},
		{"sleui",  0b000000011101},
		{"modi",   0b000000011110},
		{"ext",    0b000000011111},
		{"int",    0b000000100000},
		{"rit",    0b000000100001},
		{"slli",   0b000000100010},
		{"srli",   0b000000100011},
		{"srai",   0b000000100100},
		{"lbi",    0b000000100101},
		{"sbi",    0b000000100110},
		{"lni",    0b000000100111},
		{"lbni",   0b000000101000},
		{"sgi",    0b000000101001},
		{"sgei",   0b000000101010},
		{"cmpi",   0b000000101011},
		{"jp",     0b000000101100},
		{"jn",     0b000000101101},
		{"jz",     0b000000101110},
		{"jnz",    0b000000101111},
		{"time",   0b000000110000},
		{"timei",  0b000000110001},
		{"ring",   0b000000110010},
		{"ringi",  0b000000110011},
		{"divi",   0b000000110100},
		{"divui",  0b000000110101},
		{"divii",  0b000000110110},
		{"divuii", 0b000000110111},
		{"sel",    0b000000111000},
		{"sspush", 0b000000111001},
		{"sspop",  0b000000111010},
		{"sgeui",  0b000000111011},
		{"sgui",   0b000000111100},
		{"pgon",   0b000000111101},
		{"pgoff",  0b000000111101},
		{"setpt",  0b000000111101},
		{"svpg",   0b000000111101},
		{"sllii",  0b000000111110},
		{"srlii",  0b000000111111},
		{"sraii",  0b000001000000},
		{"qm",     0b000001000001},
	};

	std::unordered_map<int, Opcode> TOKEN_OPCODES_R {
		{WASMTOK_PLUS,    OPCODES.at("add")  },
		{WASMTOK_MINUS,   OPCODES.at("sub")  },
		{WASMTOK_AND,     OPCODES.at("and")  },
		{WASMTOK_OR,      OPCODES.at("or")   },
		{WASMTOK_LAND,    OPCODES.at("land") },
		{WASMTOK_LOR,     OPCODES.at("lor")  },
		{WASMTOK_X,       OPCODES.at("xor")  },
		{WASMTOK_XNOR,    OPCODES.at("xnor") },
		{WASMTOK_LNAND,   OPCODES.at("lnand")},
		{WASMTOK_LNOR,    OPCODES.at("lnor") },
		{WASMTOK_NAND,    OPCODES.at("nand") },
		{WASMTOK_NOR,     OPCODES.at("nor")  },
		{WASMTOK_SLASH,   OPCODES.at("div")  },
		{WASMTOK_LXNOR,   OPCODES.at("lxnor")},
		{WASMTOK_LXOR,    OPCODES.at("lxor") },
		{WASMTOK_PERCENT, OPCODES.at("mod")  },
		{WASMTOK_LL,      OPCODES.at("sll")  },
		{WASMTOK_RL,      OPCODES.at("srl")  },
		{WASMTOK_RA,      OPCODES.at("sra")  },
	};

	std::unordered_map<int, Opcode> TOKEN_OPCODES_I {
		{WASMTOK_PLUS,    OPCODES.at("addi")  },
		{WASMTOK_MINUS,   OPCODES.at("subi")  },
		{WASMTOK_AND,     OPCODES.at("andi")  },
		{WASMTOK_OR,      OPCODES.at("ori")   },
		{WASMTOK_X,       OPCODES.at("xori")  },
		{WASMTOK_XNOR,    OPCODES.at("xnori") },
		{WASMTOK_NAND,    OPCODES.at("nandi") },
		{WASMTOK_NOR,     OPCODES.at("nori")  },
		{WASMTOK_SLASH,   OPCODES.at("divi")  },
		{WASMTOK_PERCENT, OPCODES.at("modi")  },
		{WASMTOK_LL,      OPCODES.at("slli")  },
		{WASMTOK_RL,      OPCODES.at("srli")  },
		{WASMTOK_RA,      OPCODES.at("srai")  },
	};
}
