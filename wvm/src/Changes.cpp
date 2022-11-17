#include "lib/ansi.h"
#include "Changes.h"
#include "VM.h"
#include "VMError.h"
#include "Util.h"

namespace WVM {
	MemoryChange::MemoryChange(const VM &vm, Word address_, Word to_, Size size_):
		address(address_), from(vm.get(address_, size_)), to(to_), size(size_) {}

	void MemoryChange::apply(VM &vm, bool strict) {
		auto lock = vm.lockVM();
		if (strict && vm.get(address, size) != from)
			throw VMError("Unable to apply MemoryChange: memory at address isn't the expected from-value");
		vm.set(address, to, size);
	}

	void MemoryChange::undo(VM &vm, bool strict) {
		auto lock = vm.lockVM();
		if (strict && vm.get(address, size) != to)
			throw VMError("Unable to undo MemoryChange: memory at address isn't the expected to-value");
		vm.set(address, from, size);
	}

	RegisterChange::RegisterChange(UByte reg_, Word from_, Word to_, Wasmc::OperandType from_type,
	Wasmc::OperandType to_type):
		reg(reg_), from(from_), to(to_), fromType(std::move(from_type)), toType(std::move(to_type)) {}

	RegisterChange::RegisterChange(const VM &vm, UByte reg_, Word to_, Wasmc::OperandType to_type):
		reg(reg_), from(vm.registers[reg_]), to(to_), fromType(vm.registers[reg_].type), toType(std::move(to_type)) {}

	void RegisterChange::apply(VM &vm, bool strict) {
		auto lock = vm.lockVM();
		if (strict && vm.registers[reg] != Register(from, fromType))
			throw VMError("Unable to apply RegisterChange: data in register isn't the expected from-value");
		vm.registers[reg] = {to, toType};
		vm.onRegisterChange(reg);
	}

	void RegisterChange::undo(VM &vm, bool strict) {
		auto lock = vm.lockVM();
		if (strict && vm.registers[reg] != Register(to, toType)) {
			error() << "Register: " << Why::registerName(reg) << "\n";
			error() << "Expected: " << to << "\n";
			error() << "Actual:   " << vm.registers[reg].value << "\n";
			throw VMError("Unable to undo RegisterChange: data in register isn't the expected to-value");
		}
		vm.registers[reg] = {from, fromType};
		vm.onRegisterChange(reg);
	}

	JumpChange::JumpChange(const VM &vm, Word to_, bool link_):
		from(vm.programCounter), to(to_), returnFrom(vm.rt()), returnTo(vm.programCounter + Why::instructionSize),
		link(link_) {}

	void JumpChange::apply(VM &vm, bool strict) {
		auto lock = vm.lockVM();

		if (strict) {
			if (vm.programCounter != from) {
				throw VMError("Unable to apply JumpChange: program counter (" + std::to_string(vm.programCounter) + ")"
					" isn't the expected from-value (" + std::to_string(from) + ")");
			}

			if (link && vm.rt().value != returnFrom) {
				throw VMError("Unable to apply JumpChange: return address (" + std::to_string(vm.rt().value) +
					") isn't the expected from-value (" + std::to_string(returnFrom) + ")");
			}
		}

		DBG("Apply: jumping from " << from << " to " << to);
		vm.programCounter = to;
		vm.onJump(from, to);
		if (link) {
			vm.rt().value = returnTo;
			vm.onRegisterChange(Why::returnAddressOffset);
		}
	}

	void JumpChange::undo(VM &vm, bool strict) {
		auto lock = vm.lockVM();

		if (strict) {
			if (vm.programCounter != to)
				throw VMError("Unable to undo JumpChange: program counter (" + std::to_string(vm.programCounter) +
					") isn't the expected to-value (" + std::to_string(to) + ")");

			if (link && vm.rt().value != returnTo)
				throw VMError("Unable to undo JumpChange: return address (" + std::to_string(vm.rt().value) +
					") isn't the expected to-value (" + std::to_string(returnTo) + ")");
		}

		vm.programCounter = from;
		vm.onJump(to, from);
		if (link) {
			vm.rt().value = returnFrom;
			vm.onRegisterChange(Why::returnAddressOffset);
		}
	}

	InterruptTableChange::InterruptTableChange(const VM &vm, Word to_): from(vm.interruptTableAddress), to(to_) {}

	void InterruptTableChange::apply(VM &vm, bool strict) {
		auto lock = vm.lockVM();

		if (strict && vm.interruptTableAddress != from) {
			throw VMError("Unable to apply InterruptTableChange: interrupt table address isn't the expected "
				"from-value");
		}

		vm.interruptTableAddress = to;
		vm.onInterruptTableChange();
	}

	void InterruptTableChange::undo(VM &vm, bool strict) {
		auto lock = vm.lockVM();
		if (strict && vm.interruptTableAddress != to)
			throw VMError("Unable to undo InterruptTableChange: interrupt table address isn't the expected to-value");
		vm.interruptTableAddress = from;
		vm.onInterruptTableChange();
	}

	RingChange::RingChange(const VM &vm, Ring to_): from(vm.ring), to(to_) {}

	void RingChange::apply(VM &vm, bool strict) {
		auto lock = vm.lockVM();
		if (strict && vm.ring != from)
			throw VMError("Unable to apply RingChange: current ring isn't the expected from-value");
		vm.ring = to;
		vm.onRingChange(from, to);
	}

	void RingChange::undo(VM &vm, bool strict) {
		auto lock = vm.lockVM();
		if (strict && vm.ring != to)
			throw VMError("Unable to undo RingChange: current ring isn't the expected to-value");
		vm.ring = from;
		vm.onRingChange(to, from);
	}

	void HaltChange::apply(VM &vm, bool strict) {
		auto lock = vm.lockVM();
		if (strict && !vm.getActive())
			throw VMError("Unable to apply HaltChange: VM is already halted");
		vm.stop();
	}

	void HaltChange::undo(VM &vm, bool strict) {
		auto lock = vm.lockVM();
		if (strict && vm.getActive())
			throw VMError("Unable to undo HaltChange: VM isn't halted");
		vm.start();
	}

	PagingChange::PagingChange(const VM &vm, bool to_): from(vm.pagingOn), to(to_) {}

	void PagingChange::apply(VM &vm, bool strict) {
		auto lock = vm.lockVM();
		if (strict && vm.pagingOn != from)
			throw VMError("Unable to apply PagingChange: current paging isn't the expected from-value");
		vm.pagingOn = to;
		vm.onPagingChange(to);
	}

	void PagingChange::undo(VM &vm, bool strict) {
		auto lock = vm.lockVM();
		if (strict && vm.pagingOn != to)
			throw VMError("Unable to undo PagingChange: current paging isn't the expected to-value");
		vm.pagingOn = from;
		vm.onPagingChange(from);
	}

	P0Change::P0Change(const VM &vm, Word to_): from(vm.p0), to(to_) {}

	void P0Change::apply(VM &vm, bool strict) {
		auto lock = vm.lockVM();
		if (strict && vm.p0 != from)
			throw VMError("Unable to apply P0Change: current p0 isn't the expected from-value");
		vm.p0 = to;
		vm.onP0Change(to);
	}

	void P0Change::undo(VM &vm, bool strict) {
		auto lock = vm.lockVM();
		if (strict && vm.p0 != to)
			throw VMError("Unable to undo P0Change: current p0 isn't the expected to-value");
		vm.p0 = from;
		vm.onP0Change(from);
	}
}
