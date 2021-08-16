#pragma once

#include <cstdint>
#include <string>
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
}
