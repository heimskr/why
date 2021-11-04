#include "parser/ASTNode.h"
#include "parser/Lexer.h"
#include "wasm/Expression.h"

namespace Wasmc {
	void Expression::findLabels(const ASTNode *node, std::set<const std::string *> &set) {
		switch (node->symbol) {
			case WASMTOK_PLUS:
			case WASMTOK_MINUS:
			case WASMTOK_ASTERISK:
			case WASMTOK_SLASH:
			case WASMTOK_PERCENT:
				if (node->size() != 2) {
					node->debug();
					throw std::runtime_error("Expected exactly two children in expression node");
				}
				findLabels(node->at(0), set);
				findLabels(node->at(1), set);
				break;
			case WASMTOK_IDENT:
				set.insert(node->lexerInfo);
				break;
			case WASMTOK_STRING:
				set.insert(node->extracted());
				break;
			case WASMTOK_DOT:
				// "." isn't a true symbol.
				break;
			default:
				node->debug();
				throw std::runtime_error("Unexpected node in Expression::findLabels");
		}
	}

	std::set<const std::string *> Expression::findLabels(const ASTNode *node) {
		std::set<const std::string *> out;
		findLabels(node, out);
		return out;
	}
}
