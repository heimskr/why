#pragma once

#include <cstdint>
#include <string>
#include <unordered_set>

namespace Wasmc {
	using Opcode = uint16_t;

	extern std::unordered_set<Opcode> RTYPES, ITYPES, JTYPES;

	extern std::unordered_map<std::string, Opcode> OPCODES;

	extern std::unordered_map<int, Opcode> TOKEN_OPCODES_R;
}
