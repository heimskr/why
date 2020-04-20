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

			std::function<void(unsigned char)> onRegisterChange = [](unsigned char) {};
			std::function<void(Ring, Ring)> onRingChange = [](Ring, Ring) {};
			std::function<void()> onInterruptTableChange = []() {};
			std::function<void(Word)> onUpdateMemory = [](Word) {};
			std::function<void(Word, Word)> onJump = [](Word, Word) {};

			VM(size_t memory_size);

			void setWord(Word address, Word value, Endianness = Endianness::Little);
			void setHalfword(Word address, HWord value, Endianness = Endianness::Little);
			void setByte(Word address, Byte value);
			Word getWord(Word address, Endianness = Endianness::Little) const;
			HWord getHalfword(Word address, Endianness = Endianness::Little) const;
			Byte getByte(Word address) const;
			std::string getString(Word address, int max = -1) const;
			Word getInstruction(Word address) const;
			unsigned char registerID(Word &) const;

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
