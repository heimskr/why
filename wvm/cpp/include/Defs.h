#ifndef WVM_DEFS_H_
#define WVM_DEFS_H_

#include <stdint.h>

namespace WVM {
	using   Word =  int64_t;
	using  UWord = uint64_t;
	using  HWord =  int32_t;
	using UHWord = uint32_t;
	using  QWord =  int16_t;
	using UQWord = uint16_t;
	using   Byte =  int8_t;
	using  UByte = uint8_t;

	enum class Conditions: char {
		Disabled = 0b0000,
		Positive = 0b1000,
		Negative = 0b1001,
		Zero     = 0b1010,
		Nonzero  = 0b1011,
	};

	class VM;
}

#endif
