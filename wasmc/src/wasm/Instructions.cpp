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
		0b000001000010, // Disable/Enable Interrupts
		0b000001000100, // Translate Address
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
		0b000001000011, // Modulo Unsigned Immediate
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
		{"modu",   0b000000000001},
		{"sext32", 0b000000000001},
		{"sext16", 0b000000000001},
		{"sext8",  0b000000000001},
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
		{"cs",     0b000000010010},
		{"ls",     0b000000010010},
		{"ss",     0b000000010010},
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
		{"ei",     0b000001000010},
		{"di",     0b000001000010},
		{"modui",  0b000001000011},
		{"trans",  0b000001000100},
	};

	std::unordered_map<int, std::string> TOKEN_INSTRUCTIONS_R {
		{WASMTOK_PLUS,    "add"   },
		{WASMTOK_MINUS,   "sub"   },
		{WASMTOK_AND,     "and"   },
		{WASMTOK_OR,      "or"    },
		{WASMTOK_LAND,    "land"  },
		{WASMTOK_LOR,     "lor"   },
		{WASMTOK_X,       "xor"   },
		{WASMTOK_XNOR,    "xnor"  },
		{WASMTOK_LNAND,   "lnand" },
		{WASMTOK_LNOR,    "lnor"  },
		{WASMTOK_NAND,    "nand"  },
		{WASMTOK_NOR,     "nor"   },
		{WASMTOK_SLASH,   "div"   },
		{WASMTOK_LXNOR,   "lxnor" },
		{WASMTOK_LXOR,    "lxor"  },
		{WASMTOK_PERCENT, "mod"   },
		{WASMTOK_LL,      "sll"   },
		{WASMTOK_RL,      "srl"   },
		{WASMTOK_RA,      "sra"   },
		{WASMTOK_NOT,     "not"   },
		{WASMTOK_LANGLE,  "sl"    },
		{WASMTOK_LEQ,     "sle"   },
		{WASMTOK_DEQ,     "seq"   },
		{WASMTOK_BANG,    "lnot"  },
		{WASMTOK_MEMSET,  "ms"    },
		{WASMTOK_SEXT32,  "sext32"},
		{WASMTOK_SEXT16,  "sext16"},
		{WASMTOK_SEXT8,   "sext8" },
	};

	std::unordered_map<int, std::string> TOKEN_INSTRUCTIONS_I {
		{WASMTOK_PLUS,    "addi" },
		{WASMTOK_MINUS,   "subi" },
		{WASMTOK_AND,     "andi" },
		{WASMTOK_OR,      "ori"  },
		{WASMTOK_X,       "xori" },
		{WASMTOK_XNOR,    "xnori"},
		{WASMTOK_NAND,    "nandi"},
		{WASMTOK_NOR,     "nori" },
		{WASMTOK_SLASH,   "divi" },
		{WASMTOK_PERCENT, "modi" },
		{WASMTOK_LL,      "slli" },
		{WASMTOK_RL,      "srli" },
		{WASMTOK_RA,      "srai" },
		{WASMTOK_LANGLE,  "sli"  },
		{WASMTOK_LEQ,     "slei" },
		{WASMTOK_GEQ,     "sgei" },
		{WASMTOK_RANGLE,  "sgi"  },
		{WASMTOK_DEQ,     "seqi" },
	};

	std::unordered_map<std::string, std::string> UNSIGNED_EQUIVALENTS {
		{"sgi",   "sgui"  },
		{"sgei",  "sgeui" },
		{"sli",   "slui"  },
		{"slei",  "sleui" },
		{"mult",  "multu" },
		{"divi",  "divui" },
		{"divii", "divuii"},
		{"multi", "multui"},
		{"sl",    "slu"   },
		{"sle",   "sleu"  },
		{"div",   "divu"  },
		{"mod",   "modu"  },
		{"modi",  "modui" },
	};

	std::unordered_map<std::string, Funct> FUNCTS {
		{"add",    0b000000000000},
		{"and",    0b000000000000},
		{"c",      0b000000000000},
		{"jr",     0b000000000000},
		{"sl",     0b000000000000},
		{"time",   0b000000000000},
		{"ring",   0b000000000000},
		{"sel",    0b000000000000},
		{"pgoff",  0b000000000000},
		{"qm",     0b000000000000},
		{"di",     0b000000000000},
		{"trans",  0b000000000000},
		{"jrc",    0b000000000001},
		{"l",      0b000000000001},
		{"nand",   0b000000000001},
		{"sle",    0b000000000001},
		{"sub",    0b000000000001},
		{"pgon",   0b000000000001},
		{"printr", 0b000000000001},
		{"ei",     0b000000000001},
		{"jrl",    0b000000000010},
		{"mult",   0b000000000010},
		{"nor",    0b000000000010},
		{"s",      0b000000000010},
		{"seq",    0b000000000010},
		{"setpt",  0b000000000010},
		{"halt",   0b000000000010},
		{"cb",     0b000000000011},
		{"jrlc",   0b000000000011},
		{"not",    0b000000000011},
		{"slu",    0b000000000011},
		{"svpg",   0b000000000011},
		{"eval",   0b000000000011},
		{"lb",     0b000000000100},
		{"or",     0b000000000100},
		{"sleu",   0b000000000100},
		{"prc",    0b000000000100},
		{"cmp",    0b000000000101},
		{"multu",  0b000000000101},
		{"sb",     0b000000000101},
		{"xnor",   0b000000000101},
		{"prd",    0b000000000101},
		{"sll",    0b000000000110},
		{"xor",    0b000000000110},
		{"spush",  0b000000000110},
		{"prx",    0b000000000110},
		{"srl",    0b000000000111},
		{"spop",   0b000000000111},
		{"sleep",  0b000000000111},
		{"ch",     0b000000001000},
		{"land",   0b000000001000},
		{"sra",    0b000000001000},
		{"prb",    0b000000001000},
		{"lh",     0b000000001001},
		{"lnand",  0b000000001001},
		{"mod",    0b000000001001},
		{"rest",   0b000000001001},
		{"div",    0b000000001010},
		{"lnor",   0b000000001010},
		{"sh",     0b000000001010},
		{"io",     0b000000001010},
		{"divu",   0b000000001011},
		{"lnot",   0b000000001011},
		{"ms",     0b000000001011},
		{"cs",     0b000000001100},
		{"lor",    0b000000001100},
		{"modu",   0b000000001100},
		{"sext32", 0b000000001101},
		{"lxnor",  0b000000001101},
		{"ls",     0b000000001101},
		{"lxor",   0b000000001110},
		{"ss",     0b000000001110},
		{"sext16", 0b000000001110},
		{"sext8",  0b000000001111},
	};

	std::unordered_map<int, Funct> TOKEN_FUNCTS {
		{WASMTOK_PLUS,    0b000000000000},
		{WASMTOK_MINUS,   0b000000000001},
		{WASMTOK_AND,     0b000000000000},
		{WASMTOK_OR,      0b000000000100},
		{WASMTOK_LAND,    0b000000001000},
		{WASMTOK_LOR,     0b000000001100},
		{WASMTOK_X,       0b000000000110},
		{WASMTOK_XNOR,    0b000000000101},
		{WASMTOK_LNAND,   0b000000001001},
		{WASMTOK_LNOR,    0b000000001010},
		{WASMTOK_NAND,    0b000000000001},
		{WASMTOK_NOR,     0b000000000010},
		{WASMTOK_SLASH,   0b000000001010},
		{WASMTOK_LXNOR,   0b000000001101},
		{WASMTOK_LXOR,    0b000000001110},
		{WASMTOK_PERCENT, 0b000000001001},
		{WASMTOK_LL,      0b000000000110},
		{WASMTOK_RL,      0b000000000111},
		{WASMTOK_RA,      0b000000001000},
		{WASMTOK_NOT,     0b000000000011},
		{WASMTOK_LANGLE,  0b000000000000},
		{WASMTOK_LEQ,     0b000000000001},
		{WASMTOK_DEQ,     0b000000000010},
		{WASMTOK_MEMSET,  0b000000001011},
	};
}
