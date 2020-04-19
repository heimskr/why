#ifndef WVM_VM_H_
#define WVM_VM_H_

#include <filesystem>
#include <istream>
#include <map>
#include <vector>

#include "Defs.h"
#include "Symbol.h"
#include "Why.h"

namespace WVM {

	enum class Ring: int {Zero, One, Two, Three};

	class VM {
		private:
			std::vector<UByte> memory;
			size_t memorySize;
			Ring ring = Ring::Zero;
			bool active = false;
			size_t cycles = 0;

			bool getZ();
			bool getN();
			bool getC();
			bool getO();
			void setZ(bool);
			void setN(bool);
			void setC(bool);
			void setO(bool);

		public:
			Word programCounter = -1;
			Word interruptTableAddress = 0;
			Word registers[Why::totalRegisters];
			std::map<std::string, Symbol> symbolTable;
			Word symbolsOffset = -1;
			Word codeOffset = -1;
			Word dataOffset = -1;
			Word  endOffset = -1;

			VM(size_t memory_size);

			void setWord(Word address, Word value, Endianness = Endianness::Little);
			void setHalfword(Word address, HWord value, Endianness = Endianness::Little);
			void setByte(Word address, Byte value);
			Word getWord(Word address, Endianness = Endianness::Little);
			HWord getHalfword(Word address, Endianness = Endianness::Little);
			Byte getByte(Word address);
			std::string getString(Word address, int max = -1);
			Word getInstruction(Word address);

			void jump(Word, bool should_link = false);
			void link();
			void increment();
			bool changeRing(Ring);
			void updateFlags(Word);
			bool checkConditions(Conditions);
			bool interrupt(int);
			bool checkRing(Ring);
			void intProtec();
			void start();
			void stop();
			bool tick();

			void load(const std::string &);
			void load(const std::filesystem::path &);
			void load(std::istream &);
			void init();
			void loadSymbols();

			Word & hi();
			Word & lo();
			Word & st();
			Word & sp();
			Word & fp();
	};
}

#endif
