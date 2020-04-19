#ifndef WVM_VM_H_
#define WVM_VM_H_

#include <vector>

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

	class VM {
		private:
			std::vector<int> memory;

		public:
			VM(size_t memory_size);
	};
}

#endif
