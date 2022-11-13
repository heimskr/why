#include <cassert>

#include "parser/ASTNode.h"
#include "wasm/Assembler.h"
#include "wasm/Types.h"

namespace Wasmc {
	const OperandType OperandType::VOID_PTR {false, Primitive::Void, 1};

	bool isUnknown(SymbolEnum type) {
		return type == SymbolEnum::Unknown || type == SymbolEnum::UnknownCode || type == SymbolEnum::UnknownData;
	}

	RelocationData::RelocationData(Long first, Long second, Long third):
		isData((first & 1) == 1), type(RelocationType((first >> 1) & 3)), symbolIndex(first >> 3), offset(second),
		sectionOffset(third) {}

	std::vector<Long> RelocationData::encode() const {
		return {Long(isData? 1 : 0) | ((Long(type) & 3) << 1) | (symbolIndex << 3), Long(offset), Long(sectionOffset)};
	}

	bool RelocationData::operator==(const RelocationData &other) const {
		return type == other.type && symbolIndex == other.symbolIndex && offset == other.offset &&
			sectionOffset == other.sectionOffset;
	}

	RelocationData::operator std::string() const {
		return "(" + std::string(isData? "data" : "code") + ": symbolIndex[" + std::to_string(symbolIndex) +
			"], offset[" + std::to_string(offset) + "], sectionOffset[" + std::to_string(sectionOffset) + "], label[" +
			(label? *label : "nullptr") + "], section[" + (section? section->name : "nullptr") + "])";
	}

	Long AnyR::encode() const {
		return Assembler::compileR(opcode, rs, rt, rd, function, flags, condition);
	}

	Long AnyI::encode() const {
		return Assembler::compileI(opcode, rs, rd, immediate, flags, condition);
	}

	Long AnyJ::encode() const {
		return Assembler::compileJ(opcode, rs, immediate, link, flags, condition);
	}

	OperandType::OperandType(const ASTNode *node) {
		if (node == nullptr)
			return;

		auto inner = std::string_view(*node->lexerInfo);
		inner.remove_prefix(1); // Remove initial '{'
		if (inner.front() == 'v') {
			primitive = Primitive::Void;
			inner.remove_prefix(1);
			isSigned = false;
		} else {
			isSigned = inner.front() == 's';
			inner.remove_prefix(1);
			switch (inner.front()) {
				case 'c': primitive = Primitive::Char;  break;
				case 's': primitive = Primitive::Short; break;
				case 'i': primitive = Primitive::Int;   break;
				case 'l': primitive = Primitive::Long;  break;
			}
			inner.remove_prefix(1);
		}

		pointerLevel = static_cast<int>(inner.size()) - 1; // Inner matches /^\**\}$/ at this point.
	}

	OperandType::operator std::string() const {
		std::ostringstream oss("{");
		if (primitive != Primitive::Void)
			oss << (isSigned? 's' : 'u');
		oss << static_cast<char>(primitive);
		oss << std::string(static_cast<size_t>(pointerLevel < 0? 0 : pointerLevel), '*');
		oss << '}';
		return oss.str();
	}

	TypedReg::TypedReg(const ASTNode *node):
		type(node), reg(node? node->front()->lexerInfo : nullptr) {}

	TypedReg::operator std::string() const {
		if (!valid)
			return "$?";
		return (reg? *reg : "$?") + std::string(type);
	}

	TypedReg::operator bool() const {
		return valid;
	}
}
