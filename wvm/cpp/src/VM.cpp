#include <fstream>
#include <iomanip>
#include <iostream>

#include "lib/ansi.h"
#include "Operations.h"
#include "Util.h"
#include "VM.h"

namespace WVM {
	VM::VM(size_t memory_size, bool keep_initial): memorySize(memory_size), keepInitial(keep_initial) {}

	bool VM::getZ() { return (st() & 0b0001) != 0; }
	bool VM::getN() { return (st() & 0b0010) != 0; }
	bool VM::getC() { return (st() & 0b0100) != 0; }
	bool VM::getO() { return (st() & 0b1000) != 0; }
	void VM::setZ(bool on) { st() = (st() & ~0b0001) |  on;       }
	void VM::setN(bool on) { st() = (st() & ~0b0010) | (on << 1); }
	void VM::setC(bool on) { st() = (st() & ~0b0100) | (on << 2); }
	void VM::setO(bool on) { st() = (st() & ~0b1000) | (on << 3); }

	void VM::setWord(Word address, Word value, Endianness endianness) {
		if (endianness == Endianness::Little) {
			for (char i = 0; i < 8; i++)
				memory[address + i] = (value >> (8*i)) & 0xff;
		} else {
			for (char i = 0; i < 8; i++)
				memory[address + 7 - i] = (value >> (8*i)) & 0xff;
		}
		onUpdateMemory(address - (address % 8));
		if (address % 8 != 0)
			onUpdateMemory(address - (address % 8) + 8);
	}

	void VM::setHalfword(Word address, HWord value, Endianness endianness) {
		if (endianness == Endianness::Little) {
			for (char i = 0; i < 4; i++)
				memory[address + i] = (value >> (8*i)) & 0xff;
		} else {
			for (char i = 0; i < 4; i++)
				memory[address + 3 - i] = (value >> (8*i)) & 0xff;
		}
		onUpdateMemory(address - (address % 8));
		if (4 < address % 8)
			onUpdateMemory(address - (address % 8) + 8);
	}

	void VM::setByte(Word address, Byte value) {
		memory[address] = value;
		onUpdateMemory(address - (address % 8));
	}

	Word VM::getWord(Word address, Endianness endianness) const {
		Word out = 0;

		if (endianness == Endianness::Little) {
			for (char i = 0; i < 8; i++)
				out |= static_cast<Word>(memory[address + i]) << (i * 8);
		} else {
			for (char i = 0; i < 8; i++)
				out |= static_cast<Word>(memory[address + i]) << ((7 - i) * 8);
		}

		return out;
	}

	HWord VM::getHalfword(Word address, Endianness endianness) const {
		HWord out = 0;

		if (endianness == Endianness::Little) {
			for (char i = 0; i < 4; i++)
				out |= static_cast<HWord>(memory[address + i]) << (8*i);
		} else {
			for (char i = 0; i < 4; i++)
				out |= static_cast<HWord>(memory[address + 3 - i]) << (8*i);
		}

		return out;
	}

	Byte VM::getByte(Word address) const {
		return memory[address];
	}

	std::string VM::getString(Word address, int max) const {
		std::string out;
		out.reserve(32);
		int j = 0;
		for (UWord i = address; i < memorySize; ++i) {
			if (j++ == max || memory[i] == '\0')
				break;
			out.push_back(memory[i]);
		}
		return out;
	}

	Word VM::getInstruction(Word address) const {
		return getWord(address, Endianness::Big);
	}

	unsigned char VM::registerID(Word &word) const {
		return &word - registers;
	}

	void VM::reserve(size_t to_reserve) {
		memory.reserve(to_reserve);
		if (memorySize < to_reserve)
			memorySize = to_reserve;
	}

	void VM::jump(Word address, bool should_link) {
		if (should_link)
			link();
		Word old_address = programCounter;
		programCounter = address;
		onJump(old_address, address);
	}

	void VM::link() {
		registers[Why::returnAddressOffset] = programCounter + 8;
	}

	void VM::increment() {
		programCounter += 8;
		onJump(programCounter - 8, programCounter);
	}

	bool VM::changeRing(Ring new_ring) {
		Ring old_ring = ring;
		if (static_cast<int>(new_ring) < Why::ringMin || Why::ringMax < static_cast<int>(new_ring)) {
			intProtec();
			return false;
		} else if (new_ring < ring) {
			intProtec();
			return false;
		}

		ring = new_ring;
		if (old_ring != new_ring)
			onRingChange(old_ring, new_ring);
		return true;
	}

	void VM::updateFlags(Word result) {
		Word old_value = st();
		st() = 0;
		if (result == 0)
			setZ(true);
		else if (result < 0)
			setN(true);
		if (old_value != st())
			onRegisterChange(Why::statusOffset);
	}

	bool VM::checkConditions(Conditions conditions) {
		switch (conditions) {
			case Conditions::Positive: return !getN() && !getZ();
			case Conditions::Negative: return getN();
			case Conditions::Zero:     return getZ();
			case Conditions::Nonzero:  return !getZ();
			case Conditions::Disabled: return true;
		}

		throw std::runtime_error("Invalid conditions flag: " + std::to_string(static_cast<int>(conditions)));
	}

	bool VM::interrupt(int) {
		return true;
	}

	bool VM::checkRing(Ring check) {
		if (static_cast<int>(check) != -1 && static_cast<int>(check) < static_cast<int>(ring)) {
			intProtec();
			return false;
		}

		return true;
	}

	void VM::intProtec() {}

	void VM::start() {
		active = true;
	}

	void VM::stop() {
		active = false;
	}

	bool VM::tick() {
		UWord instruction = getWord(programCounter, Endianness::Big);
		Operations::execute(*this, instruction);

		++cycles;
		return active;
	}

	void VM::load(const std::string &path) {
		load(std::filesystem::path(path));
	}

	void VM::load(const std::filesystem::path &path) {
		loadedFrom = path;
		std::ifstream stream;
		try {
			stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			stream.open(path);
			stream.exceptions(std::ifstream::goodbit);
			load(stream);
			stream.close();
		} catch (std::exception &err) {
			error() << "Couldn't open \e[1m" << path << "\e[22m: " << err.what() << "\n";
			throw;
		}
	}

	void VM::load(std::istream &stream) {
		std::string line;
		int lineno = 0;
		memory.clear();
		memory.reserve(memorySize);
		while (std::getline(stream, line)) {
			++lineno;
			char *endptr;
			UWord word = strtoul(line.c_str(), &endptr, 16);
			if (line.size() != 16 || endptr - line.c_str() != 16)
				throw std::runtime_error("Invalid line (" + std::to_string(lineno) + ")");
			setWord(8 * (lineno - 1), word, Endianness::Big);
		}

		if (keepInitial)
			initial = memory;

		init();
	}

	void VM::init() {
		symbolsOffset = getWord(0, Endianness::Big);
		codeOffset = programCounter = getWord(8, Endianness::Big);
		dataOffset = getWord(16, Endianness::Big);
		endOffset = getWord(24, Endianness::Big);
		registers[Why::globalAreaPointerOffset] = endOffset;
		sp() = memorySize - 8;
		onRegisterChange(Why::globalAreaPointerOffset);
		onRegisterChange(Why::stackPointerOffset);
		loadSymbols();
	}

	void VM::reset() {
		if (keepInitial)
			memory = initial;
		else if (!loadedFrom.empty())
			load(loadedFrom);
		else
			throw std::runtime_error("Unable to reset VM: no initial memory or path was stored");
		init();
	}

	void VM::loadSymbols() {
		symbolTable.clear();
		for (Word i = symbolsOffset; i < codeOffset;) {
			const HWord hash = getHalfword(i, Endianness::Big);
			const HWord length = getHalfword(i + 4, Endianness::Big);
			const Word location = getWord(i + 8, Endianness::Big);
			const std::string name = getString(i + 16, length * 8);
			symbolTable.emplace(name, Symbol(hash, location));
			i += 16 + length * 8;
		}
	}

	Word & VM::hi() {
		return registers[Why::hiOffset];
	}

	Word & VM::lo() {
		return registers[Why::loOffset];
	}

	Word & VM::st() {
		return registers[Why::statusOffset];
	}

	Word & VM::sp() {
		return registers[Why::stackPointerOffset];
	}

	Word & VM::fp() {
		return registers[Why::framePointerOffset];
	}
}
