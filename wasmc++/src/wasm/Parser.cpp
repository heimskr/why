#include "util/Util.h"
#include "wasm/Instructions.h"
#include "wasm/Nodes.h"
#include "wasm/Parser.h"

namespace Wasmc {
	AnyBase * parseInstruction(const Long instruction) {
		auto get = [&](int offset, int length) -> Long {
			return (instruction >> (64 - offset)) & ((1 << length) - 1);
		};

		const auto opcode = static_cast<Opcode>(get(0, 12));

		if (opcode == 0)
			return new AnyBase(0, 0, 0, 0);

		if (RTYPES.count(opcode) != 0) {
			const auto rs = get(19, 7);
			const auto rt = get(12, 7);
			const auto rd = get(26, 7);
			const auto function = get(52, 12);
			const auto condition = get(46, 4);
			const auto flags = get(50, 2);
			return new AnyR(opcode, rs, rt, rd, function, condition, flags);
		}
		
		if (ITYPES.count(opcode) != 0) {
			const auto rs = get(18, 7);
			const auto rd = get(25, 7);
			const auto immediate = get(64, 32);
			const auto condition = get(12, 4);
			const auto flags = get(16, 2);
			return new AnyI(opcode, rs, rd, immediate, condition, flags);
		}

		if (JTYPES.count(opcode) != 0) {
			const auto rs = get(12, 7);
			const auto link = get(19, 1);
			const auto address = get(64, 32);
			const auto condition = get(26, 4);
			const auto flags = get(30, 2);
			return new AnyJ(opcode, rs, link, address, condition, flags);
		}

		throw std::runtime_error("Invalid instruction: " + Util::toHex(instruction));
	}
}
