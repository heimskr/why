#pragma once

#include <cstdint>

#include "compiler/Immediate.h"
#include "parser/Enums.h"
#include "wasm/Instructions.h"

namespace Wasmc {
	using Long = uint64_t;

	enum class SymbolType: unsigned {Unknown = 0, KnownPointer, UnknownPointer, Code, Data};
	enum class LinkerFlags: unsigned {Ignore = 0, KnownSymbol, UnknownSymbol, SymbolID};

	struct AnyBase {
		Opcode opcode;
		const std::string *rs;
		Condition condition;
		LinkerFlags flags;
		AnyBase(Opcode opcode_, const std::string *rs_, Condition condition_, LinkerFlags flags_):
			opcode(opcode_), rs(rs_), condition(condition_), flags(flags_) {}
	};

	struct AnyImmediate: AnyBase {
		Immediate immediate;
		AnyImmediate(Opcode opcode_, const std::string *rs_, const Immediate &immediate_, Condition condition_,
		LinkerFlags flags_):
			AnyBase(opcode_, rs_, condition_, flags_), immediate(immediate_) {}
	};

	struct AnyR: AnyBase {
		const std::string *rd, *rt;
		const Funct function;
		AnyR(Opcode opcode_, const std::string *rs_, const std::string *rt_, const std::string *rd_, Funct function_,
		Condition condition_, LinkerFlags flags_):
			AnyBase(opcode_, rs_, condition_, flags_), rd(rd_), rt(rt_), function(function_) {}
	};

	struct AnyI: AnyImmediate {
		const std::string *rd;
		AnyI(Opcode opcode_, const std::string *rs_, const std::string *rd_, const Immediate &immediate_,
		Condition condition_, LinkerFlags flags_):
			AnyImmediate(opcode_, rs_, immediate_, condition_, flags_), rd(rd_) {}
	};

	struct AnyJ: AnyImmediate {
		bool link;
		AnyJ(Opcode opcode_, const std::string *rs_, bool link_, const Immediate &immediate_,
		Condition condition_, LinkerFlags flags_):
			AnyImmediate(opcode_, rs_, immediate_, condition_, flags_), link(link_) {}
	};
}
