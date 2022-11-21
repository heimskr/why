#pragma once

#include <string>
#include <variant>

#include "wasm/Types.h"

namespace Wasmc {
	class ASTNode;

	using Immediate = std::variant<int, char, const std::string *>;

	struct TypedImmediate {
		OperandType type;
		Immediate value;
		TypedImmediate(OperandType type_, Immediate value_):
			type(std::move(type_)), value(std::move(value_)) {}
		template <typename T>
		TypedImmediate(OperandType type_, T value_):
			type(std::move(type_)), value(value_) {}
		template <typename T>
		const T & get() const { return std::get<T>(value); }
		template <typename T>
		T & get() { return std::get<T>(value); }
		template <typename T>
		bool is() const {
			return std::holds_alternative<T>(value);
		}
	};

	using Either = std::variant<TypedReg, TypedImmediate>;

	TypedImmediate getImmediate(const ASTNode *);
	Immediate getUntypedImmediate(const ASTNode *);
	Either getEither(const ASTNode *);

	std::string colorize(const TypedImmediate &, bool ampersand = false);
	std::string colorize(const Either &, bool ampersand = false);
	std::string toString(const TypedImmediate &, bool ampersand = false);
	std::string toString(const Either &, bool ampersand = false);
}
