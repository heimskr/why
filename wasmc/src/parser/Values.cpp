#include "parser/Lexer.h"
#include "parser/Parser.h"
#include "parser/Values.h"

namespace Wasmc {
	AggregateValue::AggregateValue(std::vector<std::unique_ptr<Value>> &&values_): values(std::move(values_)) {}

	AggregateValue * AggregateValue::add(const ASTNode *node) {
		values.push_back(getValue(node));
		return this;
	}

	IntValue::IntValue(long value_): value(value_) {}

	PointerValue::PointerValue(const std::string *name_): name(name_) {}

	std::unique_ptr<Value> getValue(const ASTNode *node) {
		switch (node->symbol) {
			case WASMTOK_INT_TYPE:
				return std::make_unique<IntValue>(node->front()->atoi());
			case WASM_ARRAYTYPE: {
				if (node->empty())
					return std::make_unique<ArrayValue>();
				std::vector<std::unique_ptr<Value>> values;
				for (const ASTNode *subnode: *node->front())
					values.push_back(getValue(subnode));
				return std::make_unique<ArrayValue>(std::move(values));
			}
			case WASMTOK_LCURLY: {
				if (node->empty())
					return std::make_unique<StructValue>();
				std::vector<std::unique_ptr<Value>> values;
				for (const ASTNode *subnode: *node->front())
					values.push_back(getValue(subnode));
				return std::make_unique<StructValue>(std::move(values));
			}
			case WASMTOK_AND:
				return std::make_unique<PointerValue>(node->front()->lexerInfo);
			default:
				throw std::runtime_error("Invalid symbol in getValue: " + std::to_string(node->symbol));
		}
	}
}
