#pragma once

#include <string>
#include <variant>

namespace Wasmc {
	using Immediate = std::variant<int, char, const std::string *>;
	std::string colorize(const Immediate &, bool ampersand = false);
	std::string toString(const Immediate &, bool ampersand = false);
}
