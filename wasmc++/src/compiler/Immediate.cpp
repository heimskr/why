#include "compiler/Immediate.h"

namespace Wasmcpp {
	std::string colorize(const Immediate &imm, bool ampersand) {
		if (std::holds_alternative<int>(imm))
			return "\e[32m" + std::to_string(std::get<int>(imm)) + "\e[39m";
		if (std::holds_alternative<char>(imm))
			return "\e[33m" + toString(imm, ampersand) + "\e[39m";
		if (ampersand)
			return "\e[38;5;202m&" + *std::get<const std::string *>(imm) + "\e[39m";
		return "\e[38;5;202m" + *std::get<const std::string *>(imm) + "\e[39m";
	}

	std::string toString(const Immediate &imm, bool ampersand) {
		if (std::holds_alternative<int>(imm))
			return std::to_string(std::get<int>(imm));
		if (std::holds_alternative<char>(imm)) {
			char ch = std::get<char>(imm);
			switch (ch) {
				case '\n': return "'\\n'";
				case '\r': return "'\\r'";
				case '\a': return "'\\a'";
				case '\b': return "'\\b'";
				case '\t': return "'\\t'";
				default: return "'" + std::string(1, ch) + "'";
			}
		}
		if (ampersand)
			return "&" + *std::get<const std::string *>(imm);
		return *std::get<const std::string *>(imm);
	}
}
