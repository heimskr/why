#include "parser/ASTNode.h"
#include "parser/Lexer.h"
#include "parser/Parser.h"
#include "util/Util.h"
#include "wasm/Expression.h"

namespace Wasmc {
	Expression::Expression(ASTNode *node): ASTNode(wasmParser, WASM_EXPRESSION) {
		symbol = node->symbol;
		absorb(node);
	}

	void Expression::findLabels(const ASTNode *node, std::set<const std::string *> &set) {
		if (!node)
			throw std::runtime_error("Node is null in Expression::findLabels");
		switch (node->symbol) {
			case WASMTOK_PLUS:
			case WASMTOK_MINUS:
			case WASMTOK_ASTERISK:
			case WASMTOK_SLASH:
			case WASMTOK_PERCENT:
				if (node->size() != 2) {
					node->debug();
					throw std::runtime_error("Expected exactly two children in binary expression node");
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
			case WASMTOK_NUMBER:
				// "." isn't a true symbol and numeric constants aren't labels.
				break;
			default:
				node->debug();
				throw std::runtime_error("Unexpected node in Expression::findLabels");
		}
	}

	size_t Expression::labelCount(const ASTNode *node) {
		if (!node)
			throw std::runtime_error("Node is null in Expression::labelCount");
		switch (node->symbol) {
			case WASMTOK_PLUS:
			case WASMTOK_MINUS:
			case WASMTOK_ASTERISK:
			case WASMTOK_SLASH:
			case WASMTOK_PERCENT:
				if (node->size() != 2) {
					node->debug();
					throw std::runtime_error("Expected exactly two children in binary expression node");
				}
				return labelCount(node->at(0)) + labelCount(node->at(1));
			case WASMTOK_IDENT:
			case WASMTOK_STRING:
				return 1;
			case WASMTOK_DOT:
			case WASMTOK_NUMBER:
				return 0;
			default:
				node->debug();
				throw std::runtime_error("Unexpected node in Expression::labelCount");
		}
	}

	bool Expression::validate(const ASTNode *node) {
		for (const ASTNode *child: *node)
			if (!validate(child))
				return false;

		switch (node->symbol) {
			case WASMTOK_PLUS:
				return fullyNumeric(node->at(0)) || fullyNumeric(node->at(1));
			case WASMTOK_MINUS:
				if (fullyNumeric(node->at(0)) && fullyNumeric(node->at(1)))
					return true;
				if (isSymbol(node->at(0)) || node->at(0)->symbol == WASMTOK_DOT)
					return fullyNumeric(node->at(1)) || isSymbol(node->at(1));
				warn() << "Returning false by default for expression " << node->debugExtra() << "\n";
				return false;
			case WASMTOK_ASTERISK:
			case WASMTOK_SLASH:
			case WASMTOK_PERCENT:
				// Multiplication, division and modulo are valid for numeric constants only.
				return fullyNumeric(node->at(0)) && fullyNumeric(node->at(1));
			case WASMTOK_IDENT:
			case WASMTOK_STRING:
			case WASMTOK_DOT:
			case WASMTOK_NUMBER:
				return true;
			default:
				node->debug();
				throw std::runtime_error("Unexpected node in Expression::validate");
		}
	}

	bool Expression::fullyNumeric(const ASTNode *node) {
		for (const ASTNode *child: *node)
			if (!fullyNumeric(child))
				return false;

		switch (node->symbol) {
			case WASMTOK_PLUS:
			case WASMTOK_MINUS:
			case WASMTOK_ASTERISK:
			case WASMTOK_SLASH:
			case WASMTOK_PERCENT:
				// If any descendants are non-numeric, the function will have already returned false by this point.
				return true;
			case WASMTOK_IDENT:
			case WASMTOK_STRING:
			case WASMTOK_DOT:
				return false;
			case WASMTOK_NUMBER:
				return true;
			default:
				node->debug();
				throw std::runtime_error("Unexpected node in Expression::fullyNumeric");
		}
	}

	bool Expression::isSymbol(const ASTNode *node) {
		return node && (node->symbol == WASMTOK_IDENT || node->symbol == WASMTOK_STRING);
	}

	std::set<const std::string *> Expression::findLabels() const {
		std::set<const std::string *> out;
		findLabels(this, out);
		return out;
	}

	size_t Expression::labelCount() const {
		return labelCount(this);
	}

	void Expression::checkLabelCount(size_t max) const {
		if (max < labelCount()) {
			debug();
			throw std::runtime_error("Expression contains too many label references");
		}
	}

	bool Expression::validate() const {
		return validate(this);
	}
}
