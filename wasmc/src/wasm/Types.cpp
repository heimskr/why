#include "wasm/Assembler.h"
#include "wasm/Types.h"

namespace Wasmc {
	std::vector<Long> RelocationData::encode() const {
		const Long *ptr = reinterpret_cast<const Long *>(this);
		return {ptr[0], ptr[1], ptr[2]};
	}

	bool RelocationData::operator==(const RelocationData &other) const {
		return type == other.type && symbolIndex == other.symbolIndex && offset == other.offset &&
			sectionOffset == other.sectionOffset;
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
