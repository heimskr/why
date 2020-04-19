#include "VM.h"

namespace WVM {
	VM::VM(size_t memory_size) {
		memory.reserve(memory_size);
	}
}
