#include <fstream>
#include <iostream>

#include "VM.h"

namespace WVM {
	VM::VM(size_t memory_size) {
		memory.reserve(memory_size);
	}

	void VM::setWord(Word address, Word value, Endianness endianness) {
		if (endianness == Endianness::Little) {
			for (char i = 0; i < 8; i++)
				memory[address + i] = (value >> (8*i)) & 0xff;
		} else {
			for (char i = 0; i < 8; i++)
				memory[address + 7 - i] = (value >> (8*i)) & 0xff;
		}
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
	}
}
