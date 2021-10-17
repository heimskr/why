#pragma once

#include <string>
#include <variant>

namespace Wasmc {
	class ASTNode;

	using Immediate = std::variant<int, char, const std::string *>;

	struct Register {
		const std::string *reg;
		explicit Register(const ASTNode *);
		explicit Register(const std::string *reg_): reg(reg_) {}
		operator const std::string *() const { return reg; }
		const std::string & operator*() const { return *reg; }
		const std::string & operator->() const { return *reg; }
	};

	using Either = std::variant<Register, Immediate>;

	Immediate getImmediate(const ASTNode *);
	Either getEither(const ASTNode *);

	std::string colorize(const Immediate &, bool ampersand = false);
	std::string colorize(const Either &, bool ampersand = false);
	std::string toString(const Immediate &, bool ampersand = false);
	std::string toString(const Either &, bool ampersand = false);
}
