#include "parser/ASTNode.h"
#include "parser/Lexer.h"
#include "parser/Types.h"
#include "util/Util.h"

namespace Wasmc {
	ArrayType::ArrayType(long count_, const Type *subtype_):
		count(count_), subtype(subtype_? subtype_->copy() : nullptr) {}

	ArrayType::ArrayType(long count_, std::unique_ptr<Type> &&subtype_): count(count_), subtype(std::move(subtype_)) {}

	std::unique_ptr<Type> ArrayType::copy() const {
		return std::make_unique<ArrayType>(count, subtype.get());
	}

	StructType::StructType(std::vector<std::unique_ptr<Type>> &&subtypes_): subtypes(std::move(subtypes_)) {}

	std::unique_ptr<Type> StructType::copy() const {
		std::vector<std::unique_ptr<Type>> copies;
		for (const auto &subtype: subtypes)
			copies.push_back(subtype? subtype->copy() : nullptr);
		return std::make_unique<StructType>(std::move(copies));
	}

	IntType::IntType(size_t width_): width(width_) {}

	std::unique_ptr<Type> IntType::copy() const {
		return std::make_unique<IntType>(width);
	}

	PointerType::PointerType(const Type *subtype_):
		subtype(subtype_? subtype_->copy() : nullptr) {}

	PointerType::PointerType(std::unique_ptr<Type> &&subtype_): subtype(std::move(subtype_)) {}

	std::unique_ptr<Type> PointerType::copy() const {
		return std::make_unique<PointerType>(subtype.get());
	}

	std::unique_ptr<Type> getType(const ASTNode *node) {
		switch (node->symbol) {
			case WASMTOK_INT_TYPE:
				return std::make_unique<IntType>(Util::parseUlong(node->lexerInfo->substr(2)));
			case WASM_POINTERTYPE:
				return std::make_unique<PointerType>(getType(node->front()));
			case WASM_ARRAYTYPE:
				return std::make_unique<ArrayType>(node->front()->atoi(), getType(node->back()));
			case WASM_STRUCTTYPE: {
				std::vector<std::unique_ptr<Type>> types;
				for (const ASTNode *type_node: *node->front())
					types.push_back(getType(type_node));
				return std::make_unique<StructType>(std::move(types));
			}
			case WASMTOK_FUNCTION_TYPE:
				return std::make_unique<FunctionType>();
			default:
				throw std::runtime_error("Unknown symbol in getType: " + std::to_string(node->symbol));
		}
	}

	FunctionType::FunctionType() {}

	std::unique_ptr<Type> FunctionType::copy() const {
		return std::make_unique<FunctionType>();
	}
}
