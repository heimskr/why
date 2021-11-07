#ifndef WVM_SYMBOL_H_
#define WVM_SYMBOL_H_

#include "Defs.h"

namespace WVM {
	enum class SymbolEnum: short {Unknown = 0, KnownPointer, UnknownPointer, Data, Code};

	struct Symbol {
		HWord hash;
		Word location;
		SymbolEnum type;
		Symbol(HWord hash_, Word location_, SymbolEnum type_): hash(hash_), location(location_), type(type_) {}
	};
}

#endif
