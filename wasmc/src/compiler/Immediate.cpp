#include "compiler/Immediate.h"
#include "parser/ASTNode.h"
#include "parser/Lexer.h"
#include "util/Color.h"
#include "util/Util.h"
#include "wasm/Nodes.h"

namespace Wasmc {
	std::string colorize(const TypedImmediate &imm, bool ampersand) {
		if (std::holds_alternative<int>(imm.value))
			return green(std::to_string(imm.get<int>()));
		if (std::holds_alternative<char>(imm.value))
			return yellow(toString(imm, ampersand));
		if (ampersand)
			return orange("&" + *imm.get<const std::string *>());
		return orange(*imm.get<const std::string *>());
	}

	std::string colorize(const Either &either, bool ampersand) {
		if (std::holds_alternative<TypedImmediate>(either))
			return colorize(std::get<TypedImmediate>(either), ampersand);
		return cyan(std::get<TypedReg>(either));
	}

	std::string toString(const TypedImmediate &imm, bool ampersand) {
		if (std::holds_alternative<int>(imm.value))
			return std::to_string(imm.get<int>());
		if (std::holds_alternative<char>(imm.value)) {
			const char ch = imm.get<char>();
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
			return "&" + *imm.get<const std::string *>();
		return *imm.get<const std::string *>();
	}

	std::string toString(const Either &either, bool ampersand) {
		if (std::holds_alternative<TypedImmediate>(either))
			return toString(std::get<TypedImmediate>(either), ampersand);
		return std::get<TypedReg>(either);
	}

	TypedImmediate getImmediate(const ASTNode *node) {
		return {OperandType(node), getUntypedImmediate(node->front())};
	}

	Immediate getUntypedImmediate(const ASTNode *node) {
		if (!node)
			throw std::invalid_argument("getImmediate requires its argument not to be null");

		if (node->symbol == WASM_IMMEDIATE)
			return dynamic_cast<const WASMImmediateNode *>(node)->imm.value;

		if (node->symbol == WASMTOK_NUMBER)
			return static_cast<int>(node->atoi());

		if (node->symbol == WASMTOK_CHAR) {
			const std::string middle = node->lexerInfo->substr(1, node->lexerInfo->size() - 2);
			if (middle.size() == 1)
				return middle.front();
			if (middle.front() != '\\')
				throw std::runtime_error("Invalid character literal: " + *node->lexerInfo);
			size_t pos = middle.find_first_not_of("\\");
			if (pos == std::string::npos)
				return '\\';
			switch (middle[pos]) {
				case 'n':  return '\n';
				case 'r':  return '\r';
				case 'a':  return '\a';
				case 't':  return '\t';
				case 'b':  return '\b';
				case '\'': return '\'';
				case 'x':  return static_cast<char>(Util::parseUlong(middle.substr(pos + 1, 2), 16));
				default:   throw std::runtime_error("Invalid character literal: " + *node->lexerInfo);
			}
		}

		if (node->symbol == WASMTOK_STRING) {
			if (node->lexerInfo->size() < 2)
				throw std::runtime_error("Encountered WASMTOK_STRING with a lexerInfo shorter than 2 characters");
			return StringSet::intern(node->lexerInfo->substr(1, node->lexerInfo->size() - 2));
		}

		return node->lexerInfo;
	}

	Either getEither(const ASTNode *node) {
		if (node->symbol == WASMTOK_TYPE)
			return TypedReg(node);
		return getImmediate(node);
	}
}
