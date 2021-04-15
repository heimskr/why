#pragma once

#include <functional>
#include <unordered_map>

#include "Defs.h"

namespace WVM {
	class VM;

	enum class InterruptType: int {System = 1, Timer = 2, Protec = 3};

	struct Interrupt {
		InterruptType type;
		Ring newRing, maxPermitted;
		std::function<void()> extra = [] {};

		Interrupt(InterruptType type_, Ring new_ring, Ring max_permitted):
			type(type_), newRing(new_ring), maxPermitted(max_permitted) {}

		Interrupt(InterruptType type_, Ring new_ring, Ring max_permitted, const std::function<void()> &extra_):
			type(type_), newRing(new_ring), maxPermitted(max_permitted), extra(extra_) {}

		void operator()(VM &);
	};

	extern std::unordered_map<InterruptType, Interrupt> interrupts;
}
