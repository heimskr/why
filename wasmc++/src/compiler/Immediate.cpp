#include "compiler/Immediate.h"
#include "parser/ASTNode.h"
#include "parser/Lexer.h"
#include "util/Color.h"
#include "wasm/Nodes.h"

namespace Wasmc {
	Register::Register(const ASTNode *node): reg(node->lexerInfo) {}

	std::string colorize(const Immediate &imm, bool ampersand) {
		if (std::holds_alternative<int>(imm))
			return green(std::to_string(std::get<int>(imm)));
		if (std::holds_alternative<char>(imm))
			return yellow(toString(imm, ampersand));
		if (ampersand)
			return orange("&" + *std::get<const std::string *>(imm));
		return orange(*std::get<const std::string *>(imm));
	}

	std::string colorize(const Either &either, bool ampersand) {
		if (std::holds_alternative<Immediate>(either))
			return colorize(std::get<Immediate>(either), ampersand);
		return cyan(*std::get<Register>(either));
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

	std::string toString(const Either &either, bool ampersand) {
		if (std::holds_alternative<Immediate>(either))
			return toString(std::get<Immediate>(either), ampersand);
		return *std::get<Register>(either);
	}

	Immediate getImmediate(const ASTNode *node) {
		if (node->symbol == WASM_IMMEDIATE)
			return dynamic_cast<const WASMImmediateNode *>(node)->imm;
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
				case 'n': return '\n';
				case 'r': return '\r';
				case 'a': return '\a';
				case 't': return '\t';
				case 'b': return '\b';
				default:  throw std::runtime_error("Invalid character literal: " + *node->lexerInfo);
			}
		}
		return node->lexerInfo;
	}

	Either getEither(const ASTNode *node) {
		return node->symbol == WASMTOK_REG? Register(node->lexerInfo) : getImmediate(node);
	}
}
