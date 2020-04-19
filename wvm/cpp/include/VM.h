#ifndef WVM_VM_H_
#define WVM_VM_H_

#include <filesystem>
#include <istream>
#include <vector>

#include <stdint.h>

#include "Why.h"

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
			std::vector<UByte> memory;

		public:
			Word programCounter = 0;
			Word registers[Why::totalRegisters];

			VM(size_t memory_size);

			void setWord(Word address, Word value, Endianness = Endianness::Little);
			void setHalfword(Word address, HWord value, Endianness = Endianness::Little);
			Word getWord(Word address, Endianness = Endianness::Little);
			HWord getHalfword(Word address, Endianness = Endianness::Little);

			void load(const std::string &);
			void load(const std::filesystem::path &);
			void load(std::istream &);
			void init();
	};
}

#endif
