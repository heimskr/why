#pragma once

#include <string>
#include <variant>

#include "wasm/Types.h"

namespace Wasmc {
	class ASTNode;

	using Immediate = std::variant<int, char, const std::string *>;

	using Either = std::variant<TypedReg, Immediate>;

	Immediate getImmediate(const ASTNode *);
	Either getEither(const ASTNode *);

	std::string colorize(const Immediate &, bool ampersand = false);
	std::string colorize(const Either &, bool ampersand = false);
	std::string toString(const Immediate &, bool ampersand = false);
	std::string toString(const Either &, bool ampersand = false);
}
