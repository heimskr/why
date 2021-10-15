#include "Interrupts.h"
#include "Registers.h"
#include "VM.h"

namespace WVM {
	void Interrupt::operator()(VM &vm, bool) {
		// TODO: taking newRing and maxPermitted into account.

		if (vm.interruptTableAddress == 0) {
			vm.recordChange<HaltChange>();
			vm.stop();
		} else {
			const Word destination = vm.getWord(vm.interruptTableAddress + static_cast<Word>(type) * 8,
				Endianness::Big);
			if (destination == 0) {
				vm.recordChange<HaltChange>();
				vm.stop();
			} else {
				const Word ring = static_cast<Word>(vm.ring);
				vm.recordChange<RegisterChange>(vm, REG_E + 0, vm.programCounter);
				vm.recordChange<RegisterChange>(vm, REG_E + 1, ring);
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
		{InterruptType::System, {InterruptType::System, Ring::Zero,    Ring::Invalid}},
		{InterruptType::Timer,  {InterruptType::Timer,  Ring::Invalid, Ring::Zero}},
		{InterruptType::Protec, {InterruptType::Protec, Ring::Zero,    Ring::Two}},
		{InterruptType::Pfault, {InterruptType::Pfault, Ring::Zero,    Ring::Two}},
		{InterruptType::Inexec, {InterruptType::Inexec, Ring::Zero,    Ring::Two}},
	};
}
