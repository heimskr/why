#pragma once

#include <cstdint>

#include "parser/Enums.h"
#include "wasm/Instructions.h"

namespace Wasmc {
	using Long = uint64_t;

	enum class SymbolEnum: unsigned {Unknown = 0, KnownPointer, UnknownPointer, Code, Data};
	enum class SymbolType {Unknown, Other, Function, Object};
	enum class LinkerFlags: unsigned {Ignore = 0, KnownSymbol, UnknownSymbol, SymbolID};

	struct Offsets {
		Long symbolTable = 0, code = 0, data = 0, debug = 0, end = 0;
		Offsets() = default;
		Offsets(Long symbol_table, Long code_, Long data_, Long debug_, Long end_):
			symbolTable(symbol_table), code(code_), data(data_), debug(debug_), end(end_) {}
	};

	struct AnyBase {
		enum class Type {Other, R, I, J};

		const Type type;
		Opcode opcode;
		uint8_t rs;
		uint8_t condition;
		uint8_t flags;

		AnyBase(Opcode opcode_, uint8_t rs_, uint8_t condition_, uint8_t flags_, Type type_ = Type::Other):
			type(type_), opcode(opcode_), rs(rs_), condition(condition_), flags(flags_) {}

		virtual ~AnyBase() {}

		virtual Long encode() const { return 0; }
	};

	struct AnyImmediate: AnyBase {
		uint32_t immediate;
		AnyImmediate(Opcode opcode_, uint8_t rs_, uint32_t immediate_, uint8_t condition_, uint8_t flags_,
		Type type_):
			AnyBase(opcode_, rs_, condition_, flags_, type_), immediate(immediate_) {}
	};

	struct AnyR: AnyBase {
		uint8_t rd, rt;
		const Funct function;
		AnyR(Opcode opcode_, uint8_t rs_, uint8_t rt_, uint8_t rd_, Funct function_, uint8_t condition_,
		uint8_t flags_):
			AnyBase(opcode_, rs_, condition_, flags_, Type::R), rd(rd_), rt(rt_), function(function_) {}
		Long encode() const override;
	};

	struct AnyI: AnyImmediate {
		uint8_t rd;
		AnyI(Opcode opcode_, uint8_t rs_, uint8_t rd_, uint32_t immediate_, uint8_t condition_, uint8_t flags_):
			AnyImmediate(opcode_, rs_, immediate_, condition_, flags_, Type::I), rd(rd_) {}
		Long encode() const override;
	};

	struct AnyJ: AnyImmediate {
		bool link;
		AnyJ(Opcode opcode_, uint8_t rs_, bool link_, uint32_t immediate_, uint8_t condition_, uint8_t flags_):
			AnyImmediate(opcode_, rs_, immediate_, condition_, flags_, Type::J), link(link_) {}
		Long encode() const override;
	};
}
