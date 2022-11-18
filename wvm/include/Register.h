#pragma once

#include "Defs.h"
#include "Why.h"

namespace WVM {
	struct Register {
		Word value;
		OperandType type;
		Word index = -1;

		Register():
			Register(0, OperandType()) {}

		Register(Word value_, OperandType type_):
			value(value_), type(std::move(type_)) {}

		Register(Word value_, uint8_t type_):
			Register(value_, OperandType(type_)) {}

		Register(Word value_, int type_):
			Register(value_, static_cast<uint8_t>(type_)) {}

		Register(const Register &) = default;
		Register(Register &&) = default;

		Register & operator=(const Register &other) {
			if (this == &other)
				return *this;
			value = other.value;
			type = other.type;
			return *this;
		}

		Register & operator=(Register &&other) {
			if (this == &other)
				return *this;
			value = other.value;
			type = std::move(other.type);
			other.value = 0;
			other.type = {};
			other.index = -1;
			return *this;
		}

		explicit operator Word() const {
			return value;
		}

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
