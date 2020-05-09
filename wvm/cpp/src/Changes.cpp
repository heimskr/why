#include "Changes.h"
#include "VM.h"
#include "VMError.h"

namespace WVM {
	MemoryChange::MemoryChange(const VM &vm, Word address_, Word to_, Size size_):
		address(address_), from(vm.get(address_, size_)), to(to_), size(size_) {}

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

	RegisterChange::RegisterChange(const VM &vm, UByte reg_, Word to_):
		reg(reg_), from(vm.registers[reg_]), to(to_) {}

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

	JumpChange::JumpChange(const VM &vm, Word to_, bool link_):
		from(vm.programCounter), to(to_), returnFrom(vm.rt()), returnTo(vm.programCounter + 8), link(link_) {}

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
