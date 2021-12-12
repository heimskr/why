#include <iostream>
#include "Interrupts.h"
#include "Registers.h"
#include "VM.h"

namespace WVM {
	void Interrupt::operator()(VM &vm, bool) const {
		// TODO: take newRing and maxPermitted into account.
		auto lock = vm.lockVM();

		if (vm.interruptTableAddress == 0) {
			vm.recordChange<HaltChange>();
			vm.stop();
		} else {
			const Word destination = vm.getWord(vm.interruptTableAddress + Word(type) * 8,
				Endianness::Little);
			if (destination == 0) {
				vm.recordChange<HaltChange>();
				vm.stop();
			} else {
				const Word ring = Word(vm.ring);
				vm.bufferChange<RegisterChange>(vm, REG_E + 0, vm.programCounter);
				vm.bufferChange<RegisterChange>(vm, REG_E + 1, ring);
				vm.registers[REG_E + 0] = vm.programCounter;
				vm.registers[REG_E + 1] = ring;
				vm.onRegisterChange(REG_E + 0);
				vm.onRegisterChange(REG_E + 1);
				extra();
				vm.jump(destination, false);
			}
		}
	}

	std::unordered_map<InterruptType, Interrupt> interrupts {
		{InterruptType::System, {InterruptType::System, Ring::Zero,    Ring::Invalid, false}},
		{InterruptType::Timer,  {InterruptType::Timer,  Ring::Invalid, Ring::Zero,    true}},
		{InterruptType::Protec, {InterruptType::Protec, Ring::Zero,    Ring::Two,     false}},
		{InterruptType::Pfault, {InterruptType::Pfault, Ring::Zero,    Ring::Two,     false}},
		{InterruptType::Inexec, {InterruptType::Inexec, Ring::Zero,    Ring::Two,     false}},
		{InterruptType::Bwrite, {InterruptType::Bwrite, Ring::Zero,    Ring::Invalid, false}},
		{InterruptType::Keybrd, {InterruptType::Keybrd, Ring::Zero,    Ring::Two,     true}},
	};
}
