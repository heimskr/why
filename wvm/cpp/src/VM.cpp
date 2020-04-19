#include "VM.h"

namespace WVM {
	VM::VM(size_t memory_size) {
		memory.reserve(memory_size);
	}

	void VM::setWord(Word address, Word value, Endianness endianness) {
		if (endianness == Endianness::Little) {
			for (int i = 0; i < 8; i++)
				memory[address + i] = (value >> (8*i)) & 0xff;
		} else {
			for (int i = 0; i < 8; i++)
				memory[address + 7 - i] = (value >> (8*i)) & 0xff;
		}
	}

	Word VM::getWord(Word address, Endianness endianness) {
		Word out = 0;

		if (endianness == Endianness::Little) {
			for (Byte i = 0; i < 8; i++)
				out |= ((Word) memory[address + i]) << (i * 8);
		} else {
			for (Byte i = 0; i < 8; i++)
				out |= ((Word) memory[address + i]) << ((7 - i) * 8);
		}

		return out;
	}
}
