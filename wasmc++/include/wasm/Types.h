#pragma once

#include <cstdint>

#include "parser/Enums.h"
#include "wasm/Instructions.h"

namespace Wasmc {
	using Long = uint64_t;

	enum class SymbolType: unsigned {Unknown = 0, KnownPointer, UnknownPointer, Code, Data};
	enum class LinkerFlags: unsigned {Ignore = 0, KnownSymbol, UnknownSymbol, SymbolID};

	struct AnyBase {
		Opcode opcode;
		uint16_t rs;
		uint16_t condition;
		uint16_t flags;
		AnyBase(Opcode opcode_, uint16_t rs_, uint16_t condition_, uint16_t flags_):
			opcode(opcode_), rs(rs_), condition(condition_), flags(flags_) {}
	};

	struct AnyImmediate: AnyBase {
		uint32_t immediate;
		AnyImmediate(Opcode opcode_, uint16_t rs_, uint32_t immediate_, uint16_t condition_, uint16_t flags_):
			AnyBase(opcode_, rs_, condition_, flags_), immediate(immediate_) {}
	};

	struct AnyR: AnyBase {
		uint16_t rd, rt;
		const Funct function;
		AnyR(Opcode opcode_, uint16_t rs_, uint16_t rt_, uint16_t rd_, Funct function_, uint16_t condition_,
		uint16_t flags_):
			AnyBase(opcode_, rs_, condition_, flags_), rd(rd_), rt(rt_), function(function_) {}
	};

	struct AnyI: AnyImmediate {
		uint16_t rd;
		AnyI(Opcode opcode_, uint16_t rs_, uint16_t rd_, uint32_t immediate_, uint16_t condition_, uint16_t flags_):
			AnyImmediate(opcode_, rs_, immediate_, condition_, flags_), rd(rd_) {}
	};

	struct AnyJ: AnyImmediate {
		bool link;
		AnyJ(Opcode opcode_, uint16_t rs_, bool link_, uint32_t immediate_, uint16_t condition_, uint16_t flags_):
			AnyImmediate(opcode_, rs_, immediate_, condition_, flags_), link(link_) {}
	};
}
