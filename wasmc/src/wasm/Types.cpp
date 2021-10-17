#include "wasm/Assembler.h"
#include "wasm/Types.h"

namespace Wasmc {
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
