#pragma once

#include <string>
#include <unordered_map>

namespace Wasmc {
	enum class Condition {Positive = 0b1000, Negative = 0b1001, Zero = 0b1010, Nonzero = 0b1011, None = 0b0000};
	enum class PrintType {Dec, Bin, Hex, Char, Full};
	enum class QueryType {Memory};

	extern std::unordered_map<QueryType, std::string> query_map;
}
