#include <fstream>
#include <iostream>

#include "VM.h"

namespace WVM {
	VM::VM(size_t memory_size): memorySize(memory_size) {
		memory.reserve(memory_size);
	}

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
	}

	void VM::setHalfword(Word address, HWord value, Endianness endianness) {
		if (endianness == Endianness::Little) {
			for (char i = 0; i < 4; i++)
				memory[address + i] = (value >> (8*i)) & 0xff;
		} else {
			for (char i = 0; i < 4; i++)
				memory[address + 3 - i] = (value >> (8*i)) & 0xff;
		}
	}

	void VM::setByte(Word address, Byte value) {
		memory[address] = value;
	}

	Word VM::getWord(Word address, Endianness endianness) {
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

	HWord VM::getHalfword(Word address, Endianness endianness) {
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

	Byte VM::getByte(Word address) {
		return memory[address];
	}

	std::string VM::getString(Word address) {
		std::string out;
		out.reserve(32);
		for (UWord i = address; i < memorySize; ++i) {
			Byte byte = memory[i];
			if (byte == '\0')
				break;
			out.push_back(byte);
		}
		return out;
	}

	void VM::jump(Word address) {
		programCounter = address;
	}

	void VM::link() {
		registers[Why::returnAddressOffset] = programCounter + 8;
	}

	void VM::increment() {
		programCounter += 8;
	}

	bool VM::changeRing(Ring new_ring) {
		if (static_cast<int>(new_ring) < Why::ringMin || Why::ringMax < static_cast<int>(new_ring)) {
			intProtec();
			return false;
		} else if (new_ring < ring) {
			intProtec();
			return false;
		}

		ring = new_ring;
		return true;
	}

	void VM::updateFlags(Word result) {
		st() = 0;
		if (result == 0)
			setZ(true);
		else if (result < 0)
			setN(true);
	}

	void VM::intProtec() {}

	void VM::load(const std::string &path) {
		load(std::filesystem::path(path));
	}

	void VM::load(const std::filesystem::path &path) {
		std::ifstream stream;
		stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		stream.open(path);
		stream.exceptions(std::ifstream::goodbit);
		load(stream);
		stream.close();
	}

	void VM::load(std::istream &stream) {
		std::string line;
		int lineno = 0;
		while (std::getline(stream, line)) {
			++lineno;
			char *endptr;
			UWord word = strtoul(line.c_str(), &endptr, 16);
			if (line.size() != 16 || endptr - line.c_str() != 16)
				throw std::runtime_error("Invalid line (" + std::to_string(lineno) + ")");
			setWord(8 * (lineno - 1), word, Endianness::Little);
		}

		init();
	}

	void VM::init() {
		programCounter = getWord(8);
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
}
