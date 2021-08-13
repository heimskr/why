#pragma once

#include <array>
#include <map>

namespace Wasmc {
	extern std::map<const std::string *, int> registerMap;
	extern std::array<const std::string *, 128> registerArray;
}
