#pragma once

#include <functional>
#include <unordered_map>

#include "Defs.h"

namespace WVM {
	class VM;

	enum class InterruptType: int {System = 1, Timer, Protec, Pfault, Inexec, Bwrite, Keybrd};

	struct Interrupt {
		InterruptType type;
		Ring newRing, maxPermitted;
		bool canDisable;
		std::function<void()> extra = [] {};

		Interrupt(InterruptType type_, Ring new_ring, Ring max_permitted, bool can_disable):
			type(type_), newRing(new_ring), maxPermitted(max_permitted), canDisable(can_disable) {}

		Interrupt(InterruptType type_, Ring new_ring, Ring max_permitted, bool can_disable, 
		          const std::function<void()> &extra_):
			type(type_), newRing(new_ring), maxPermitted(max_permitted), canDisable(can_disable), extra(extra_) {}

		void operator()(VM &, bool force) const;
	};

	extern std::unordered_map<InterruptType, Interrupt> interrupts;
}
