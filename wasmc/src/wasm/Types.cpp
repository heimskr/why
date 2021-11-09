#include <cassert>

#include "wasm/Assembler.h"
#include "wasm/Types.h"

namespace Wasmc {
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
}
