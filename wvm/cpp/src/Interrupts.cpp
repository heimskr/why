#include <iostream>
#include "Interrupts.h"
#include "Registers.h"
#include "VM.h"

namespace WVM {
	void Interrupt::operator()(VM &vm, bool) {
		// TODO: taking newRing and maxPermitted into account.

		std::cerr << "Interrupt: " << int(this->type) << "\n";

		if (vm.interruptTableAddress == 0) {
			std::cerr << "Interrupt " << int(this->type) << " failed: no interrupt table.\n";
			vm.recordChange<HaltChange>();
			vm.stop();
		} else {
			const Word destination = vm.getWord(vm.interruptTableAddress + static_cast<Word>(type) * 8,
				Endianness::Big);
			if (destination == 0) {
				std::cerr << "Interrupt " << int(this->type) << " failed: no destination.\n";
				vm.recordChange<HaltChange>();
				vm.stop();
			} else {
				const Word ring = static_cast<Word>(vm.ring);
				vm.bufferChange<RegisterChange>(vm, REG_E + 0, vm.programCounter);
				vm.recordChange<RegisterChange>(vm, REG_E + 1, ring);
				vm.registers[REG_E + 0] = vm.programCounter;
				vm.registers[REG_E + 1] = ring;
				vm.onRegisterChange(REG_E + 0);
				vm.onRegisterChange(REG_E + 1);
				extra();
				std::cerr << "Interrupt " << int(this->type) << " jumping to " << destination << ".\n";
				vm.jump(destination, false);
			}
		}
	}

	std::unordered_map<InterruptType, Interrupt> interrupts {
		{InterruptType::System, {InterruptType::System, Ring::Zero,    Ring::Invalid}},
		{InterruptType::Timer,  {InterruptType::Timer,  Ring::Invalid, Ring::Zero}},
		{InterruptType::Protec, {InterruptType::Protec, Ring::Zero,    Ring::Two}},
		{InterruptType::Pfault, {InterruptType::Pfault, Ring::Zero,    Ring::Two}},
		{InterruptType::Inexec, {InterruptType::Inexec, Ring::Zero,    Ring::Two}},
		{InterruptType::Bwrite, {InterruptType::Bwrite, Ring::Zero,    Ring::Invalid}},
	};
}
