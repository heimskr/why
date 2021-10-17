#pragma once

#include <array>
#include <cstdint>
#include <map>

namespace Wasmc {
	extern std::map<const std::string *, uint64_t> registerMap;
	extern std::array<const std::string *, 128> registerArray;
}
