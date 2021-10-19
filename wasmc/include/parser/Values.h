#pragma once

#include <memory>
#include <vector>

namespace Wasmc {
	class ASTNode;

	enum class ValueType {
		Aggregate, Array, Struct, Int, Pointer
	};

	struct Value {
		virtual ~Value() {}
		virtual ValueType valueType() const = 0;
	};

	struct AggregateValue: Value {
		std::vector<std::unique_ptr<Value>> values;
		AggregateValue(std::vector<std::unique_ptr<Value>> && = {});
		AggregateValue * add(const ASTNode *);
		ValueType valueType() const override { return ValueType::Aggregate; }
	};

	struct ArrayValue: AggregateValue {
		using AggregateValue::AggregateValue;
		ValueType valueType() const override { return ValueType::Array; }
	};

	struct StructValue: AggregateValue {
		using AggregateValue::AggregateValue;
		ValueType valueType() const override { return ValueType::Struct; }
	};

	struct IntValue: Value {
		long value;
		IntValue(long);
		ValueType valueType() const override { return ValueType::Int; }
	};

	struct PointerValue: Value {
		const std::string *name;
		PointerValue(const std::string *);
		ValueType valueType() const override { return ValueType::Pointer; }
	};

	std::unique_ptr<Value> getValue(const ASTNode *);
}
