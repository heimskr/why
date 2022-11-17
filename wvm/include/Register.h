#pragma once

#include "Defs.h"
#include "Why.h"

namespace WVM {
	struct Register {
		Word value;
		OperandType type;
		Register():
			Register(0, OperandType()) {}
		Register(Word value_, OperandType type_):
			value(value_), type(std::move(type_)) {}
		Register(Word value_, uint8_t type_):
			Register(value_, OperandType(type_)) {}
		Register(Word value_, int type_):
			Register(value_, static_cast<uint8_t>(type_)) {}
		explicit operator Word() const { return value; }
		Word & set(Word new_value, OperandType new_type) {
			value = new_value;
			type = std::move(new_type);
			return value;
		}
		bool operator==(const Register &other) const {
			return this == &other || (value == other.value && type == other.type);
		}
	};
}
