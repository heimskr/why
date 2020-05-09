#include "Changes.h"
#include "VM.h"
#include "VMError.h"

namespace WVM {
	void MemoryChange::apply(VM &vm, bool strict) {
		if (strict && vm.get(address, size) != from)
			throw VMError("Unable to apply MemoryChange: memory at address isn't the expected from-value");
		vm.set(address, to, size);
	}

	void MemoryChange::undo(VM &vm, bool strict) {
		if (strict && vm.get(address, size) != to)
			throw VMError("Unable to undo MemoryChange: memory at address isn't the expected to-value");
		vm.set(address, from, size);
	}

	void RegisterChange::apply(VM &vm, bool strict) {
		if (strict && vm.registers[reg] != from)
			throw VMError("Unable to apply RegisterChange: data in register isn't the expected from-value");
		vm.registers[reg] = to;
		vm.onRegisterChange(reg);
	}

	void RegisterChange::undo(VM &vm, bool strict) {
		if (strict && vm.registers[reg] != to)
			throw VMError("Unable to undo RegisterChange: data in register isn't the expected to-value");
		vm.registers[reg] = from;
		vm.onRegisterChange(reg);
	}

	void JumpChange::apply(VM &vm, bool strict) {
		if (strict) {
			if (vm.programCounter != from)
				throw VMError("Unable to apply JumpChange: program counter isn't the expected from-value");
			if (link && vm.programCounter != returnFrom) {
				throw VMError("Unable to apply JumpChange: program counter isn't the expected return address from-"
					"value");
			}
		}

		vm.jump(to, link);
	}

	void JumpChange::undo(VM &vm, bool strict) {
		if (strict) {
			if (vm.programCounter != to)
				throw VMError("Unable to undo JumpChange: program counter isn't the expected to-value");
			if (link && vm.rt() != returnTo)
				throw VMError("Unable to undo JumpChange: return address isn't the expected to-value");
		}

		vm.programCounter = from;
		vm.onJump(to, from);
		if (link) {
			vm.rt() = returnFrom;
			vm.onRegisterChange(Why::returnAddressOffset);
		}
	}
}
