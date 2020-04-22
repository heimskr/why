#ifndef WVM_VM_H_
#define WVM_VM_H_

#include <filesystem>
#include <functional>
#include <istream>
#include <map>
#include <unordered_set>
#include <vector>

#include "Defs.h"
#include "Symbol.h"
#include "Why.h"

namespace WVM {

	enum class Ring: int {Zero, One, Two, Three};

	class VM {
		private:
			std::vector<UByte> memory, initial;
			std::filesystem::path loadedFrom;
			size_t memorySize;
			bool keepInitial;
			Ring ring = Ring::Zero;
			bool active = false;
			size_t cycles = 0;
			std::unordered_set<Word> breakpoints;

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
			std::function<void(const std::string &)> onPrint = [](const std::string &) {};
			std::function<void(Word)> onAddBreakpoint = [](Word) {};
			std::function<void(Word)> onRemoveBreakpoint = [](Word) {};

			VM(size_t memory_size, bool keep_initial = true);

			void setWord(Word address, Word value, Endianness = Endianness::Little);
			void setHalfword(Word address, HWord value, Endianness = Endianness::Little);
			void setByte(Word address, Byte value);
			Word getWord(Word address, Endianness = Endianness::Little) const;
			HWord getHalfword(Word address, Endianness = Endianness::Little) const;
			Byte getByte(Word address) const;
			std::string getString(Word address, int max = -1) const;
			Word getInstruction(Word address) const;
			unsigned char registerID(Word &) const;
			void resize(size_t);

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

			void addBreakpoint(Word);
			void removeBreakpoint(Word);
			const std::unordered_set<Word> & getBreakpoints() const;
			bool hasBreakpoint(Word) const;

			void load(const std::string &);
			void load(const std::filesystem::path &);
			void load(std::istream &);
			void init();
			void reset();
			void loadSymbols();

			size_t getMemorySize() { return memorySize; }

			Word & hi();
			Word & lo();
			Word & st();
			Word & sp();
			Word & fp();
			const Word & hi() const;
			const Word & lo() const;
			const Word & st() const;
			const Word & sp() const;
			const Word & fp() const;
	};
}

#endif
