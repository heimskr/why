#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace Wasmc {
	using Opcode = uint16_t;
	using Funct  = uint16_t;

	constexpr uint16_t FUNCT_MAX = 4095;

	extern std::unordered_set<Opcode> RTYPES, ITYPES, JTYPES;
	extern std::unordered_map<std::string, Opcode> OPCODES;
	extern std::unordered_map<int, std::string> TOKEN_INSTRUCTIONS_R, TOKEN_INSTRUCTIONS_I;
	extern std::unordered_map<std::string, std::string> UNSIGNED_EQUIVALENTS;
	extern std::unordered_map<std::string, Funct> FUNCTS;
	extern std::unordered_map<int, Funct> TOKEN_FUNCTS;

	constexpr Opcode OP_NOP    = 0b000000000000;
	constexpr Opcode OP_ADD    = 0b000000000001;
	constexpr Opcode OP_SLL    = 0b000000000001;
	constexpr Opcode OP_SRL    = 0b000000000001;
	constexpr Opcode OP_SRA    = 0b000000000001;
	constexpr Opcode OP_SUB    = 0b000000000001;
	constexpr Opcode OP_MULT   = 0b000000000001;
	constexpr Opcode OP_MULTU  = 0b000000000001;
	constexpr Opcode OP_MOD    = 0b000000000001;
	constexpr Opcode OP_DIV    = 0b000000000001;
	constexpr Opcode OP_DIVU   = 0b000000000001;
	constexpr Opcode OP_AND    = 0b000000000010;
	constexpr Opcode OP_NAND   = 0b000000000010;
	constexpr Opcode OP_NOR    = 0b000000000010;
	constexpr Opcode OP_NOT    = 0b000000000010;
	constexpr Opcode OP_OR     = 0b000000000010;
	constexpr Opcode OP_XNOR   = 0b000000000010;
	constexpr Opcode OP_XOR    = 0b000000000010;
	constexpr Opcode OP_LAND   = 0b000000000010;
	constexpr Opcode OP_LNAND  = 0b000000000010;
	constexpr Opcode OP_LNOR   = 0b000000000010;
	constexpr Opcode OP_LNOT   = 0b000000000010;
	constexpr Opcode OP_LOR    = 0b000000000010;
	constexpr Opcode OP_LXNOR  = 0b000000000010;
	constexpr Opcode OP_LXOR   = 0b000000000010;
	constexpr Opcode OP_ADDI   = 0b000000000011;
	constexpr Opcode OP_SUBI   = 0b000000000100;
	constexpr Opcode OP_MULTI  = 0b000000000101;
	constexpr Opcode OP_ANDI   = 0b000000000110;
	constexpr Opcode OP_NANDI  = 0b000000000111;
	constexpr Opcode OP_NORI   = 0b000000001000;
	constexpr Opcode OP_ORI    = 0b000000001001;
	constexpr Opcode OP_XNORI  = 0b000000001010;
	constexpr Opcode OP_XORI   = 0b000000001011;
	constexpr Opcode OP_LUI    = 0b000000001101;
	constexpr Opcode OP_SL     = 0b000000001110;
	constexpr Opcode OP_SLE    = 0b000000001110;
	constexpr Opcode OP_SEQ    = 0b000000001110;
	constexpr Opcode OP_SLU    = 0b000000001110;
	constexpr Opcode OP_SLEU   = 0b000000001110;
	constexpr Opcode OP_CMP    = 0b000000001110;
	constexpr Opcode OP_J      = 0b000000001111;
	constexpr Opcode OP_JC     = 0b000000010000;
	constexpr Opcode OP_JR     = 0b000000010001;
	constexpr Opcode OP_JRC    = 0b000000010001;
	constexpr Opcode OP_JRL    = 0b000000010001;
	constexpr Opcode OP_JRLC   = 0b000000010001;
	constexpr Opcode OP_C      = 0b000000010010;
	constexpr Opcode OP_CB     = 0b000000010010;
	constexpr Opcode OP_CH     = 0b000000010010;
	constexpr Opcode OP_L      = 0b000000010010;
	constexpr Opcode OP_LB     = 0b000000010010;
	constexpr Opcode OP_LH     = 0b000000010010;
	constexpr Opcode OP_S      = 0b000000010010;
	constexpr Opcode OP_SB     = 0b000000010010;
	constexpr Opcode OP_SH     = 0b000000010010;
	constexpr Opcode OP_SPUSH  = 0b000000010010;
	constexpr Opcode OP_SPOP   = 0b000000010010;
	constexpr Opcode OP_MS     = 0b000000010010;
	constexpr Opcode OP_LI     = 0b000000010011;
	constexpr Opcode OP_SI     = 0b000000010100;
	constexpr Opcode OP_SET    = 0b000000010101;
	constexpr Opcode OP_MULTUI = 0b000000011000;
	constexpr Opcode OP_SLI    = 0b000000011001;
	constexpr Opcode OP_SLEI   = 0b000000011010;
	constexpr Opcode OP_SEQI   = 0b000000011011;
	constexpr Opcode OP_SLUI   = 0b000000011100;
	constexpr Opcode OP_SLEUI  = 0b000000011101;
	constexpr Opcode OP_MODI   = 0b000000011110;
	constexpr Opcode OP_EXT    = 0b000000011111;
	constexpr Opcode OP_INT    = 0b000000100000;
	constexpr Opcode OP_RIT    = 0b000000100001;
	constexpr Opcode OP_SLLI   = 0b000000100010;
	constexpr Opcode OP_SRLI   = 0b000000100011;
	constexpr Opcode OP_SRAI   = 0b000000100100;
	constexpr Opcode OP_LBI    = 0b000000100101;
	constexpr Opcode OP_SBI    = 0b000000100110;
	constexpr Opcode OP_LNI    = 0b000000100111;
	constexpr Opcode OP_LBNI   = 0b000000101000;
	constexpr Opcode OP_SGI    = 0b000000101001;
	constexpr Opcode OP_SGEI   = 0b000000101010;
	constexpr Opcode OP_CMPI   = 0b000000101011;
	constexpr Opcode OP_JP     = 0b000000101100;
	constexpr Opcode OP_JN     = 0b000000101101;
	constexpr Opcode OP_JZ     = 0b000000101110;
	constexpr Opcode OP_JNZ    = 0b000000101111;
	constexpr Opcode OP_TIME   = 0b000000110000;
	constexpr Opcode OP_TIMEI  = 0b000000110001;
	constexpr Opcode OP_RING   = 0b000000110010;
	constexpr Opcode OP_RINGI  = 0b000000110011;
	constexpr Opcode OP_DIVI   = 0b000000110100;
	constexpr Opcode OP_DIVUI  = 0b000000110101;
	constexpr Opcode OP_DIVII  = 0b000000110110;
	constexpr Opcode OP_DIVUII = 0b000000110111;
	constexpr Opcode OP_SEL    = 0b000000111000;
	constexpr Opcode OP_SSPUSH = 0b000000111001;
	constexpr Opcode OP_SSPOP  = 0b000000111010;
	constexpr Opcode OP_SGEUI  = 0b000000111011;
	constexpr Opcode OP_SGUI   = 0b000000111100;
	constexpr Opcode OP_PGON   = 0b000000111101;
	constexpr Opcode OP_PGOFF  = 0b000000111101;
	constexpr Opcode OP_SETPT  = 0b000000111101;
	constexpr Opcode OP_SVPG   = 0b000000111101;
	constexpr Opcode OP_SLLII  = 0b000000111110;
	constexpr Opcode OP_SRLII  = 0b000000111111;
	constexpr Opcode OP_SRAII  = 0b000001000000;
	constexpr Opcode OP_QM     = 0b000001000001;
}
