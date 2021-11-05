#include "parser/ASTNode.h"
#include "parser/Lexer.h"
#include "parser/Parser.h"
#include "util/Util.h"
#include "wasm/Assembler.h"
#include "wasm/Expression.h"

namespace Wasmc {
	Expression::Expression(ASTNode *node): ASTNode(wasmParser, WASM_EXPRESSION) {
		symbol = node->symbol;
		absorb(node);
	}

	Expression & Expression::setCounter(Section &section_) {
		section = &section_;
		counter = section_.counter;
		return *this;
	}

	void Expression::findLabels(const ASTNode *node, std::set<const std::string *> &set) {
		if (!node)
			throw std::invalid_argument("Node is null in Expression::findLabels");

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
			throw std::invalid_argument("Node is null in Expression::labelCount");

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

	bool Expression::resultIsNumeric(const ASTNode *node) {
		switch (validate(node)) {
			case ValidationResult::DoubleLabelDifference:
			case ValidationResult::Pure:
				return true;
			default:
				return false;
		}
	}

	Expression::ValidationResult Expression::validate(const ASTNode *node) {
		if (!node)
			throw std::invalid_argument("Node is null in Expression::validate");

		for (const ASTNode *child: *node)
			if (validate(child) == ValidationResult::Invalid)
				return ValidationResult::Invalid;

		const ASTNode *left = node->empty()? nullptr : node->at(0), *right = node->size() < 2? nullptr : node->at(1);

		switch (node->symbol) {
			case WASMTOK_PLUS: {
				const bool right_numeric = fullyNumeric(right, true), left_numeric = fullyNumeric(left, true);
				if ((left_numeric && right_numeric) || (resultIsNumeric(left) && resultIsNumeric(right)))
					return ValidationResult::Pure;
				if ((isSymbol(left) && right_numeric) || (left_numeric && isSymbol(right)))
					return ValidationResult::LabelNumberSum;
				return ValidationResult::Invalid;
			}
			case WASMTOK_MINUS: {
				const bool right_numeric = fullyNumeric(right, true), left_numeric = fullyNumeric(left, true);
				if ((left_numeric && right_numeric) || (resultIsNumeric(left) && resultIsNumeric(right)))
					return ValidationResult::Pure;
				if (isSymbol(left)) {
					if ((resultIsNumeric(left) && resultIsNumeric(right)))
						return ValidationResult::LabelNumberDifference;
					if (isSymbol(right))
						return ValidationResult::DoubleLabelDifference;
					return ValidationResult::Invalid;
				}
				if (left->symbol == WASMTOK_DOT) {
					if (isSymbol(right))
						return ValidationResult::DotLabelDifference;
					return ValidationResult::Invalid;
				}
				return ValidationResult::Invalid;
			}
			case WASMTOK_ASTERISK:
			case WASMTOK_SLASH:
			case WASMTOK_PERCENT: {
				const bool right_numeric = fullyNumeric(right, true), left_numeric = fullyNumeric(left, true);
				if ((left_numeric && right_numeric) || (resultIsNumeric(left) && resultIsNumeric(right)))
					return ValidationResult::Pure;
				return ValidationResult::Invalid;
			}
			case WASMTOK_IDENT:
			case WASMTOK_STRING:
				return ValidationResult::Label;
			case WASMTOK_DOT:
			case WASMTOK_NUMBER:
				return ValidationResult::Pure;
			default:
				node->debug();
				throw std::runtime_error("Unexpected node in Expression::validate");
		}
	}

	bool Expression::fullyNumeric(const ASTNode *node, bool dot_okay) {
		if (!node)
			throw std::invalid_argument("Node is null in Expression::fullyNumeric");

		for (const ASTNode *child: *node)
			if (!fullyNumeric(child, dot_okay))
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
				return false;
			case WASMTOK_DOT:
				return dot_okay;
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

	bool Expression::hasDot(const ASTNode *node) {
		if (!node)
			throw std::invalid_argument("Node is null in Expression::hasDot");

		switch (node->symbol) {
			case WASMTOK_PLUS:
			case WASMTOK_MINUS:
			case WASMTOK_ASTERISK:
			case WASMTOK_SLASH:
			case WASMTOK_PERCENT:
				return hasDot(node->at(0)) || hasDot(node->at(1));
			case WASMTOK_IDENT:
			case WASMTOK_STRING:
			case WASMTOK_NUMBER:
				return false;
			case WASMTOK_DOT:
				return true;
			default:
				node->debug();
				throw std::runtime_error("Unexpected node in Expression::hasDot");
		}
	}

	long Expression::evaluate(const ASTNode *node, const Assembler &assembler, size_t counter) {
		if (!node)
			throw std::invalid_argument("Node is null in Expression::evaluate");

		switch (node->symbol) {
			case WASMTOK_PLUS:
				return evaluate(node->at(0), assembler, counter) + evaluate(node->at(1), assembler, counter);
			case WASMTOK_MINUS:
				return evaluate(node->at(0), assembler, counter) - evaluate(node->at(1), assembler, counter);
			case WASMTOK_ASTERISK:
				return evaluate(node->at(0), assembler, counter) * evaluate(node->at(1), assembler, counter);
			case WASMTOK_SLASH: {
				const size_t divisor = evaluate(node->at(1), assembler, counter);
				if (divisor == 0) {
					node->debug();
					throw std::invalid_argument("Division by zero in Expression::evaluate");
				}
				return evaluate(node->at(0), assembler, counter) / divisor;
			}
			case WASMTOK_PERCENT: {
				const size_t divisor = evaluate(node->at(1), assembler, counter);
				if (divisor == 0) {
					node->debug();
					throw std::invalid_argument("Modulo by zero in Expression::evaluate");
				}
				return evaluate(node->at(0), assembler, counter) % divisor;
			}
			case WASMTOK_IDENT:
				return assembler.offsets.at(node->lexerInfo);
			case WASMTOK_STRING:
				return assembler.offsets.at(node->extracted());
			case WASMTOK_NUMBER:
				return node->atoi();
			case WASMTOK_DOT:
				return long(counter);
			default:
				node->debug();
				throw std::runtime_error("Unexpected node in Expression::evaluate");
		}
	}

	std::string Expression::toString(const ASTNode *node) {
		if (!node)
			throw std::invalid_argument("Node is null in Expression::toString");

		switch (node->symbol) {
			case WASMTOK_PLUS:
				return "((" + toString(node->at(0)) + ") + (" + toString(node->at(1)) + "))";
			case WASMTOK_MINUS:
				return "((" + toString(node->at(0)) + ") - (" + toString(node->at(1)) + "))";
			case WASMTOK_ASTERISK:
				return "((" + toString(node->at(0)) + ") * (" + toString(node->at(1)) + "))";
			case WASMTOK_SLASH:
				return "((" + toString(node->at(0)) + ") / (" + toString(node->at(1)) + "))";
			case WASMTOK_PERCENT:
				return "((" + toString(node->at(0)) + ") % (" + toString(node->at(1)) + "))";
			case WASMTOK_IDENT:
				return *node->lexerInfo;
			case WASMTOK_NUMBER:
				return std::to_string(node->atoi());
			case WASMTOK_STRING:
				return node->extractName();
			case WASMTOK_DOT:
				return ".";
			default:
				node->debug();
				throw std::runtime_error("Unexpected node in Expression::toString");
		}
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

	Expression::ValidationResult Expression::validate() const {
		return validate(this);
	}

	long Expression::evaluate(const Assembler &assembler) const {
		if (hasDot(this) && counter == -1ul)
			throw std::runtime_error("Can't evaluate Expression: contains dot but counter is undefined");
		return evaluate(this, assembler, counter);
	}

	Expression::operator std::string() const {
		return toString(this);
	}
}
