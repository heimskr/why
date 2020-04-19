#ifndef WVM_SYMBOL_H_
#define WVM_SYMBOL_H_

#include "Defs.h"

namespace WVM {
	struct Symbol {
		HWord hash;
		Word location;
		Symbol(HWord hash_, Word location_): hash(hash_), location(location_) {}
	};
}

#endif
