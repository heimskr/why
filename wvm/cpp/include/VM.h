#ifndef WVM_VM_H_
#define WVM_VM_H_

#include <filesystem>
#include <istream>
#include <vector>

#include "Defs.h"
#include "Why.h"

namespace WVM {

	enum class Ring: int {Zero, One, Two, Three};

	class VM {
		private:
			std::vector<UByte> memory;
			size_t memorySize;
			Ring ring = Ring::Zero;

			bool getZ();
			bool getN();
			bool getC();
			bool getO();
			void setZ(bool);
			void setN(bool);
			void setC(bool);
			void setO(bool);

		public:
			Word programCounter = 0;
			Word registers[Why::totalRegisters];

			VM(size_t memory_size);

			void setWord(Word address, Word value, Endianness = Endianness::Little);
			void setHalfword(Word address, HWord value, Endianness = Endianness::Little);
			void setByte(Word address, Byte value);
			Word getWord(Word address, Endianness = Endianness::Little);
			HWord getHalfword(Word address, Endianness = Endianness::Little);
			Byte getByte(Word address);
			std::string getString(Word address);

			void jump(Word);
			void link();
			void increment();
			bool changeRing(Ring);
			void updateFlags(Word);
			bool checkConditions(Conditions);
			void intProtec();

			void load(const std::string &);
			void load(const std::filesystem::path &);
			void load(std::istream &);
			void init();

			Word & hi();
			Word & lo();
			Word & st();
	};
}

#endif
