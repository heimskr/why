#pragma once

#include <cstdint>
#include <map>
#include <set>
#include <vector>

#include "parser/Enums.h"
#include "util/Hash.h"
#include "wasm/Instructions.h"

namespace Wasmc {
	using Long = uint64_t;
	struct Section;

	enum class SymbolEnum: unsigned {Unknown = 0, KnownPointer, UnknownPointer, Data, Code};
	enum class SymbolType {Unknown, Other, Function, Object, Instruction};
	enum class LinkerFlags: unsigned {Ignore = 0, KnownSymbol, UnknownSymbol};
	enum class RelocationType: unsigned {Full = 0, Invalid, Lower4, Upper4};

	struct StringPtrCompare {
		bool operator()(const std::string *left, const std::string *right) const {
			return *left < *right;
		}
	};

	using StringPtrSet = std::set<const std::string *, StringPtrCompare>;

	template <typename T>
	using StringPtrMap = std::map<const std::string *, T, StringPtrCompare>;

	struct RelocationData {
		RelocationType type: 2;
		ssize_t symbolIndex: 62;
		long offset: 64;
		long sectionOffset: 64;
		Section *section;
		const std::string *label;
		std::vector<Long> encode() const;
		RelocationData(RelocationType type_, size_t symbol_index, long offset_, long section_offset,
		Section *section_ = nullptr, const std::string *label_ = nullptr):
			type(type_), symbolIndex(symbol_index), offset(offset_), sectionOffset(section_offset), section(section_),
			label(label_) {}
		bool operator==(const RelocationData &other) const;
	} __attribute__((packed));

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

namespace std {
	template <>
	class hash<Wasmc::RelocationData>: public Wasmc::Util::Hash<Wasmc::RelocationData> {};
}
