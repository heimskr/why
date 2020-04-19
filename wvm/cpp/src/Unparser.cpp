#include "Operations.h"
#include "Unparser.h"

namespace WVM::Unparser {
	std::string stringify(UWord instruction) {
		int opcode = (instruction >> 52) & 0xfff;
		if (Operations::RSet.count(opcode) == 1) {
			int rs, rt, rd;
			Conditions conditions;
			int flags, funct;
			Operations::decodeRType(instruction, rs, rt, rd, conditions, flags, funct);
			return stringifyRType(opcode, rs, rt, rd, conditions, funct);
		} else if (Operations::ISet.count(opcode) == 1) {
			int rs, rd;
			Conditions conditions;
			int flags;
			HWord immediate;
			Operations::decodeIType(instruction, rs, rd, conditions, flags, immediate);
			return stringifyIType(opcode, rs, rd, conditions, immediate);
		} else if (Operations::JSet.count(opcode) == 1) {
			int rs;
			bool link;
			Conditions conditions;
			int flags;
			HWord address;
			Operations::decodeJType(instruction, rs, link, conditions, flags, address);
			return stringifyJType(opcode, rs, link, conditions, address);
		} else throw std::runtime_error("Unknown opcode: " + std::to_string(opcode));
	}

	std::string stringifyRType(int opcode, int rs, int rt, int rd, Conditions conditions, int funct) {

	}

	std::string stringifyIType(int opcode, int rs, int rd, Conditions conditions, HWord immediate) {

	}

	std::string stringifyJType(int opcode, int rs, bool link, Conditions conditions, HWord address) {

	}
}
