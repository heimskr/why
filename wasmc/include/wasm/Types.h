#pragma once

#include <array>
#include <cstdint>
#include <map>
#include <set>
#include <stdexcept>
#include <vector>

#include "parser/Enums.h"
#include "util/Hash.h"
#include "wasm/Instructions.h"

namespace Wasmc {
	class ASTNode;

	using Long = uint64_t;
	using TypeInfo = uint32_t;
	struct Section;

	enum class SymbolEnum: unsigned {Unknown = 0, UnknownData, UnknownCode, Data, Code};
	enum class SymbolType {Unknown, Other, Function, Object, Instruction};
	enum class LinkerFlags: unsigned {Ignore = 0, KnownSymbol, UnknownSymbol};
	enum class RelocationType: unsigned {Full = 0, Invalid, Lower4, Upper4};

	bool isUnknown(SymbolEnum);

	struct SymbolNotFound: std::runtime_error {
		const std::string symbol;
		SymbolNotFound(const std::string &symbol_):
			std::runtime_error("Symbol not found: \"" + symbol_ + "\""), symbol(symbol_) {}
	};

	struct StringPtrCompare {
		bool operator()(const std::string *left, const std::string *right) const {
			return *left < *right;
		}
	};

	using StringPtrSet = std::set<const std::string *, StringPtrCompare>;

	template <typename T>
	using StringPtrMap = std::map<const std::string *, T, StringPtrCompare>;

	class RelocationData {
		private:
			RelocationData() = default;

		public:
			bool isData: 1;
			RelocationType type: 2;
			/** The index in the symbol table of the symbol to use for the relocation. */
			ssize_t symbolIndex: 61;
			/** The offset to apply to the symbol's address. */
			long offset: 64;
			/** The offset of the value to relocate within its section. */
			long sectionOffset: 64;
			/** The section in which the relocation will occur, not the section in which the label is defined. */
			Section *section = nullptr;
			const std::string *label = nullptr;
			std::vector<Long> encode() const;
			RelocationData(Long, Long, Long);
			RelocationData(bool is_data, RelocationType type_, size_t symbol_index, long offset_, long section_offset,
			Section *section_ = nullptr, const std::string *label_ = nullptr):
				isData(is_data), type(type_), symbolIndex(symbol_index), offset(offset_), sectionOffset(section_offset),
				section(section_), label(label_) {}
			bool operator==(const RelocationData &other) const;
			operator std::string() const;
	} __attribute__((packed));

	struct Offsets {
		Long code = 0, data = 0, symbolTable = 0, debug = 0, relocation = 0, end = 0;
		Offsets() = default;
		Offsets(Long code_, Long data_, Long symbol_table, Long debug_, Long relocation_, Long end_):
			code(code_), data(data_), symbolTable(symbol_table), debug(debug_), relocation(relocation_), end(end_) {}
	};

	struct TypedInstruction {
		Long instruction;
		TypeInfo typeInfo;
		TypedInstruction(): TypedInstruction(0, 0) {}
		explicit TypedInstruction(Long instruction_, TypeInfo type_info = 0):
			instruction(instruction_), typeInfo(type_info) {}
		std::array<uint8_t, 12> toBytes() const {
			return {
				static_cast<uint8_t>(instruction & 0xff),
				static_cast<uint8_t>((instruction >> 8) & 0xff),
				static_cast<uint8_t>((instruction >> 16) & 0xff),
				static_cast<uint8_t>((instruction >> 24) & 0xff),
				static_cast<uint8_t>((instruction >> 32) & 0xff),
				static_cast<uint8_t>((instruction >> 40) & 0xff),
				static_cast<uint8_t>((instruction >> 48) & 0xff),
				static_cast<uint8_t>((instruction >> 56) & 0xff),
				static_cast<uint8_t>(typeInfo & 0xff),
				static_cast<uint8_t>((typeInfo >> 8) & 0xff),
				static_cast<uint8_t>((typeInfo >> 16) & 0xff),
				static_cast<uint8_t>((typeInfo >> 24) & 0xff),
			};
		}

		explicit operator std::string() const;
	};

	struct AnyBase {
		enum class Type {Other, R, I, J};

		const Type type;
		Opcode opcode;
		uint8_t rs;
		uint8_t condition;
		uint8_t flags;
		uint8_t rsType;

		AnyBase(Opcode opcode_, uint8_t rs_, uint8_t condition_, uint8_t flags_, uint8_t rs_type,
		        Type type_ = Type::Other):
			type(type_), opcode(opcode_), rs(rs_), condition(condition_), flags(flags_), rsType(rs_type) {}

		virtual ~AnyBase() {}

		virtual TypedInstruction encode() const { return {}; }
	};

	struct AnyImmediate: AnyBase {
		uint32_t immediate;
		uint8_t immType;
		AnyImmediate(Opcode opcode_, uint8_t rs_, uint32_t immediate_, uint8_t condition_, uint8_t flags_,
		             uint8_t rs_type, uint8_t imm_type, Type type_):
			AnyBase(opcode_, rs_, condition_, flags_, rs_type, type_), immediate(immediate_), immType(imm_type) {}
	};

	struct AnyR: AnyBase {
		uint8_t rd;
		uint8_t rt;
		const Funct function;
		uint8_t rdType;
		uint8_t rtType;
		AnyR(Opcode opcode_, uint8_t rs_, uint8_t rt_, uint8_t rd_, Funct function_, uint8_t condition_,
		     uint8_t flags_, uint8_t rs_type, uint8_t rd_type, uint8_t rt_type):
			AnyBase(opcode_, rs_, condition_, flags_, rs_type, Type::R), rd(rd_), rt(rt_), function(function_),
			rdType(rd_type), rtType(rt_type) {}
		TypedInstruction encode() const override;
	};

	struct AnyI: AnyImmediate {
		uint8_t rd;
		uint8_t rdType;
		AnyI(Opcode opcode_, uint8_t rs_, uint8_t rd_, uint32_t immediate_, uint8_t condition_, uint8_t flags_,
		     uint8_t rs_type, uint8_t rd_type, uint8_t imm_type):
			AnyImmediate(opcode_, rs_, immediate_, condition_, flags_, rs_type, imm_type, Type::I), rd(rd_),
			rdType(rd_type) {}
		TypedInstruction encode() const override;
	};

	struct AnyJ: AnyImmediate {
		bool link;
		AnyJ(Opcode opcode_, uint8_t rs_, bool link_, uint32_t immediate_, uint8_t condition_, uint8_t flags_,
		     uint8_t rs_type, uint8_t imm_type):
			AnyImmediate(opcode_, rs_, immediate_, condition_, flags_, rs_type, imm_type, Type::J), link(link_) {}
		TypedInstruction encode() const override;
	};

	enum class Primitive: char {Void = 'v', Char = 'c', Short = 's', Int = 'i', Long = 'l'};

	struct OperandType {
		bool isSigned = true;
		Primitive primitive = Primitive::Void;
		int pointerLevel = -1;
		OperandType(): isSigned(true), primitive(Primitive::Void), pointerLevel(-1) {}
		OperandType(bool is_signed, Primitive primitive_, int pointer_level = 0):
			isSigned(is_signed), primitive(primitive_), pointerLevel(pointer_level) {}
		OperandType(const ASTNode *);
		operator std::string() const;
		explicit operator uint8_t() const;

		const static OperandType VOID_PTR;
	};

	struct TypedReg {
		OperandType type;
		const std::string *reg = nullptr;
		bool valid = true;
		TypedReg(): valid(false) {}
		TypedReg(const OperandType &type_, const std::string *reg_):
			type(type_), reg(reg_) {}
		TypedReg(const ASTNode *node);
		operator std::string() const {
			if (!valid)
				return "$?";
			return (reg? *reg : "$?") + std::string(type);
		}
		operator bool() const { return valid; }
	};
}

namespace std {
	template <>
	class hash<Wasmc::RelocationData>: public Wasmc::Util::Hash<Wasmc::RelocationData> {};
}
