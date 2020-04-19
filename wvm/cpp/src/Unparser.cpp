#include "Operations.h"
#include "Unparser.h"
#include "Why.h"

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
		switch (opcode) {
			case OP_RMATH: {
				std::string oper, suffix;
				switch (funct) {
					case FN_ADD:   oper = "+"; break;
					case FN_SUB:   oper = "-"; break;
					case FN_MULT:  
					case FN_MULTU:
					case FN_SLL:   oper = "<<"; break;
					case FN_SRL:   oper = ">>>"; break;
					case FN_SRA:   oper = ">>"; break;
					case FN_MOD:   oper = "%"; break;
					case FN_DIV:   oper = "/"; break;
					case FN_DIVU:  oper = "/"; suffix = "/u"; break;
				}
				return rAltOp(rs, rt, rd, oper, suffix);
			}
			case OP_RLOGIC:
				switch (funct) {
					case FN_AND:   break;
					case FN_NAND:  break;
					case FN_NOR:   break;
					case FN_NOT:   break;
					case FN_OR:    break;
					case FN_XNOR:  break;
					case FN_XOR:   break;
					case FN_LAND:  break;
					case FN_LNAND: break;
					case FN_LNOR:  break;
					case FN_LNOT:  break;
					case FN_LOR:   break;
					case FN_LXNOR: break;
					case FN_LXOR:  break;
				}
				break;
			case OP_RCOMP:
				switch (funct) {
					case FN_SL:   break;
					case FN_SLE:  break;
					case FN_SEQ:  break;
					case FN_SLU:  break;
					case FN_SLEU: break;
				}
				break;
			case OP_RJUMP:
				switch (funct) {
					case FN_JR:   break;
					case FN_JRC:  break;
					case FN_JRL:  break;
					case FN_JRLC: break;
				}
				break;
			case OP_RMEM:
				switch (funct) {
					case FN_C:     break;
					case FN_L:     break;
					case FN_S:     break;
					case FN_CB:    break;
					case FN_LB:    break;
					case FN_SB:    break;
					case FN_SPUSH: break;
					case FN_SPOP:  break;
					case FN_CH:    break;
					case FN_LH:    break;
					case FN_SH:    break;
				}
				break;
			case OP_REXT:
				switch (funct) {
					case FN_PR:    break;
					case FN_HALT:  break;
					case FN_EVAL:  break;
					case FN_PRC:   break;
					case FN_PRD:   break;
					case FN_PRX:   break;
					case FN_SLEEP: break;
				}
				break;
			case OP_TIME: break;
			case OP_RING: break;
		}

		return "R: Opcode[" + std::to_string(opcode) + "], " + Why::coloredRegister(rs) + " "
			+ Why::coloredRegister(rt) + " -> " + Why::coloredRegister(rd) + ", Funct[" + std::to_string(funct) + "]";
	}

	std::string stringifyIType(int opcode, int rs, int rd, Conditions conditions, HWord immediate) {
		return "I: Opcode[" + std::to_string(opcode) + "], " + Why::coloredRegister(rs) + " "
			+ std::to_string(immediate) + " -> " + Why::coloredRegister(rd);
	}

	std::string stringifyJType(int opcode, int rs, bool link, Conditions conditions, HWord address) {
		return "J: Opcode[" + std::to_string(opcode) + "], " + Why::coloredRegister(rs) + ", " + (link? "" : "don't ")
			+ "link, " + std::to_string(address);
	}

	std::string rAltOp(int rs, int rt, int rd, const std::string &oper, const std::string &suffix) {
		if (rs == rd || rt == rd) {
			const std::string source      = Why::coloredRegister(rs == rd? rs : rt);
			const std::string destination = Why::coloredRegister(rs == rd? rt : rs);
			return source + " \e[1m" + oper + "=\e[22m " + destination + suffix;
		}

		return Why::coloredRegister(rs) + " \e[1m" + oper + "\e[22m" + Why::coloredRegister(rt) + " \e[2m->\e[22m "
			+ Why::coloredRegister(rd);
	}
}
