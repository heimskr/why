#pragma once

#include <string>
#include <unordered_map>

namespace Wasmc {
	enum class Condition {Positive, Negative, Zero, Nonzero, None};
	enum class PrintType {Dec, Bin, Hex, Char, Full};
	enum class QueryType {Memory};

	extern std::unordered_map<QueryType, std::string> query_map;
}
