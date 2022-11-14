#include "lib/ansi.h"
#include "Operations.h"
#include "Unparser.h"
#include "VM.h"
#include "Why.h"

namespace WVM::Unparser {
	std::string stringify(Wasmc::TypedInstruction typed, const VM *vm) {
		DBG("Typed[" << std::string(typed) << ']');
		const int opcode = (typed.instruction >> 52) & 0xfff;

		if (opcode == OP_NOP)
			return "<>";

		if (Operations::RSet.count(opcode) == 1) {
			int rs, rt, rd;
			Conditions conditions;
			int flags, funct;
			int st, tt, dt;
			Operations::decodeRType(typed, rs, rt, rd, conditions, flags, funct, st, tt, dt);
			return stringifyRType(opcode, rs, rt, rd, conditions, funct, st, tt, dt);
		}

		if (Operations::ISet.count(opcode) == 1) {
			int rs, rd;
			Conditions conditions;
			int flags;
			HWord immediate;
			int st, dt, it;
			Operations::decodeIType(typed, rs, rd, conditions, flags, immediate, st, dt, it);
			return stringifyIType(opcode, rs, rd, conditions, flags, immediate, st, dt, it, vm);
		}

		if (Operations::JSet.count(opcode) == 1) {
			int rs;
			bool link;
			Conditions conditions;
			int flags;
			HWord address;
			int st;
			Operations::decodeJType(typed, rs, link, conditions, flags, address, st);
			return stringifyJType(opcode, rs, link, conditions, flags, address, st, vm);
		}

		DBG(std::hex << std::string(typed) << std::dec);
		return "???";
		// throw std::runtime_error("[U] Unknown opcode: " + std::to_string(opcode));
	}

	std::string stringifyRType(int opcode, int rs, int rt, int rd, Conditions conditions, int funct, int st, int tt,
	                           int dt) {
		switch (opcode) {
			case OP_RMATH: {
				std::string oper, suffix;
				switch (funct) {
					case FN_ADD:    oper = "+"; break;
					case FN_SUB:    oper = "-"; break;
					case FN_MULT:   return color(rs, st) + " " + colorOper("*") + " " + color(rd, dt);
					case FN_SLL:    oper = "<<"; break;
					case FN_SRL:    oper = ">>>"; break;
					case FN_SRA:    oper = ">>"; break;
					case FN_MOD:    oper = "%"; break;
					case FN_DIV:    oper = "/"; break;
					case FN_SEXT:   return "\e[1msext32\e[22m " + color(rs, st) + into + color(rd, dt);
				}
				return rAltOp(rs, rt, rd, oper, st, tt, dt, suffix);
			}
			case OP_RLOGIC: {
				std::string oper;
				switch (funct) {
					case FN_AND:   oper = "&";   break;
					case FN_NAND:  oper = "~&";  break;
					case FN_NOR:   oper = "~|";  break;
					case FN_NOT:
						return "\e[1m~\e[22m" + color(rs, st) + into + color(rd, dt);
					case FN_OR:
						if (rs == Why::zeroOffset)
							return color(rt, tt) + into + color(rd, dt);
						if (rt == Why::zeroOffset)
							return color(rs, st) + into + color(rd, dt);
						oper = "|";
						break;
					case FN_XNOR:  oper = "~x";  break;
					case FN_XOR:   oper = "x";   break;
					case FN_LAND:  oper = "&&";  break;
					case FN_LNAND: oper = "~&&"; break;
					case FN_LNOR:  oper = "~||"; break;
					case FN_LNOT:
						if (rs == rd)
							return colorOper("!") + color(rs, st) + colorOper(".");
						return colorOper("!") + color(rs, st) + into + color(rd, dt);
					case FN_LOR:   oper = "||";  break;
					case FN_LXNOR: oper = "~xx"; break;
					case FN_LXOR:  oper = "xx";  break;
				}
				return rAltOp(rs, rt, rd, oper, st, tt, dt);
			}
			case OP_RCOMP: {
				if (funct == FN_CMP)
					return color(rs, st) + " " + colorOper("~") + " " + color(rt, tt);
				std::string out = color(rs, st) + " \e[1m";
				if (funct == FN_SL) out += "<";
				else if (funct == FN_SLE) out += "<=";
				else if (funct == FN_SEQ) out += "==";
				else out += "?";
				out += "\e[22m " + color(rt, tt) + into + color(rd, dt);
				return out;
			}
			case OP_RJUMP: {
				switch (funct) {
					case FN_JR:   return "\e[2m" + jumpConditions(conditions) + ":\e[22m " + color(rd, dt);
					case FN_JRC:  return "\e[2m:\e[22m " + color(rd, dt) + " \e[31mif\e[39m " + color(rs, st);
					case FN_JRL:  return "\e[2m" + jumpConditions(conditions) + "::\e[22m " + color(rd, dt);
					case FN_JRLC: return "\e[2m::\e[22m " + color(rd, dt) + " \e[31mif\e[39m " + color(rs, st);
				}
				break;
			}
			case OP_RMEM:
				switch (funct) {
					case FN_C:     return left + color(rs, st) + right + into + left + color(rd, dt) + right;
					case FN_L:     return left + color(rs, st) + right + into + color(rd, dt);
					case FN_S:     return color(rs, st) + into + left + color(rd, dt) + right;
					case FN_CB:    return left + color(rs, st) + right + into + left + color(rd, dt) + right + " /b";
					case FN_LB:    return left + color(rs, st) + right + into + color(rd, dt) + " /b";
					case FN_SB:    return color(rs, st) + into + left + color(rd, dt) + right + " /b";
					case FN_SPUSH: return "\e[2m[\e[22m " + color(rs, st);
					case FN_SPOP:  return "\e[2m]\e[22m " + color(rd, dt);
					case FN_CH:    return left + color(rs, st) + right + into + left + color(rd, dt) + "] /h";
					case FN_LH:    return left + color(rs, st) + right + into + color(rd, dt) + " /h";
					case FN_SH:    return color(rs, st) + into + left + color(rd, dt) + right + " /h";
					case FN_MS:    return "\e[36mmemset\e[39m " + color(rs, st) + " x " + color(rt, tt) + into + color(rd, dt);
					case FN_CS:    return left + color(rs, st) + right + into + left + color(rd, dt) + "] /s";
					case FN_LS:    return left + color(rs, st) + right + into + color(rd, dt) + " /s";
					case FN_SS:    return color(rs, st) + into + left + color(rd, dt) + right + " /s";
				}
				break;
			case OP_REXT:
				switch (funct) {
					case FN_PR:    return "<\e[36mprint\e[39m " + color(rs, st) + ">";
					case FN_HALT:  return "<\e[36mhalt\e[39m>";
					case FN_EVAL:  return "<\e[36meval\e[39m "  + color(rs, st) + ">";
					case FN_PRC:   return "<\e[36mprc\e[39m "   + color(rs, st) + ">";
					case FN_PRD:   return "<\e[36mprd\e[39m "   + color(rs, st) + ">";
					case FN_PRX:   return "<\e[36mprx\e[39m "   + color(rs, st) + ">";
					case FN_SLEEP: return "<\e[36msleep\e[39m " + color(rs, st) + ">";
					case FN_REST:  return "<\e[36mrest\e[39m>";
					case FN_IO:    return "<\e[36mio\e[39m>";
				}
				break;
			case OP_TIME: return "\e[36m%time\e[39m " + color(rs, st);
			case OP_RING: return "\e[36m%ring\e[39m " + color(rs, st);
			case OP_SEL: {
				std::string oper;
				if (conditions == Conditions::Zero) oper = "=";
				else if (conditions == Conditions::Positive) oper = ">";
				else if (conditions == Conditions::Negative) oper = "<";
				else if (conditions == Conditions::Nonzero) oper = "!=";
				else oper = "?";
				return "\e[2m[\e[22m" + Why::coloredRegister(rs) + " \e[2m" + oper + "\e[22m "
					+ Why::coloredRegister(rt) + "\e[2m] -> \e[22m" + Why::coloredRegister(rd);
			}
			case OP_PAGE:
				switch (funct) {
					case FN_PGON:  return "\e[36m%page\e[39m on";
					case FN_PGOFF: return "\e[36m%page\e[39m off";
					case FN_SETPT:
						if (rt == 0)
							return "\e[36m%setpt\e[39m " + color(rs, st);
						return "\e[2m:\e[22m \e[36m%setpt\e[39m " + color(rs, st) + " " + color(rt, tt);
					case FN_SVPG:  return "\e[36m%page\e[39m \e[2m->\e[22m " + color(rd, dt);
					case FN_PPUSH: return "\e[2m[\e[22m \e[36m%page\e[39m";
					case FN_PPOP:
						if (rs == 0)
							return "\e[2m]\e[22m \e[36m%page\e[39m";
						return "\e[2m: ]\e[22m \e[36m%page\e[39m " + color(rs, st);
				}
				break;
			case OP_QUERY:
				switch (funct) {
					case FN_QM: return "? \e[36mmem\e[39m \e[2m->\e[22m " + color(rd, dt);
				}
				break;
			case OP_INTERRUPTS:
				switch (funct) {
					case FN_DI: return "\e[36m%di\e[39m";
					case FN_EI: return "\e[36m%ei\e[39m";
				}
				break;
		}

		return "R: Opcode[" + std::to_string(opcode) + "], " + Why::coloredRegister(rs) + " "
			+ Why::coloredRegister(rt) + " -> " + Why::coloredRegister(rd) + ", Funct[" + std::to_string(funct) + "]";
	}

	std::string stringifyIType(int opcode, int rs, int rd, Conditions, int, HWord immediate, int st, int dt, int it,
	                           const VM *vm) {
		std::string coloredImm;
		if (vm) {
			const std::string symbol = getSymbol(immediate, *vm);
			if (!symbol.empty())
				coloredImm = immColor + symbol + "\e[39m";
		}
		if (coloredImm.empty())
			coloredImm = colorNum(immediate, it);
		switch (opcode) {
			case OP_ADDI:   return iMath(rs, rd, coloredImm, "+", st, dt, it);
			case OP_SUBI:   return iMath(rs, rd, coloredImm, "-", st, dt, it);
			case OP_MULTI:  return color(rs, st) + " \e[1m*\e[22m " + coloredImm;
			case OP_MODI:   return iAltOp(rs, rd, coloredImm, "%", st, dt, it);
			case OP_ANDI:   return iAltOp(rs, rd, coloredImm, "&", st, dt, it);
			case OP_NANDI:  return iAltOp(rs, rd, coloredImm, "~&", st, dt, it);
			case OP_NORI:   return iAltOp(rs, rd, coloredImm, "~|", st, dt, it);
			case OP_ORI:    return iAltOp(rs, rd, coloredImm, "|", st, dt, it);
			case OP_XNORI:  return iAltOp(rs, rd, coloredImm, "~x", st, dt, it);
			case OP_XORI:   return iAltOp(rs, rd, coloredImm, "x", st, dt, it);
			case OP_SLLI:   return iAltOp(rs, rd, coloredImm, "<<", st, dt, it);
			case OP_SRLI:   return iAltOp(rs, rd, coloredImm, ">>>", st, dt, it);
			case OP_SRAI:   return iAltOp(rs, rd, coloredImm, ">>", st, dt, it);
			case OP_LUI:    return "\e[2mlui:\e[22m " + coloredImm + into + color(rd, dt);
			case OP_LI:     return left + coloredImm + right + into + color(rd, dt);
			case OP_LNI:    return left + coloredImm + right + into + left + color(rd, dt) + right;
			case OP_SI:     return color(rs, st) + into + left + coloredImm + right;
			case OP_SET:    return coloredImm + into + color(rd, dt);
			case OP_SLI:    return iComp(rs, rd, coloredImm, "<", st, dt, it);
			case OP_SLEI:   return iComp(rs, rd, coloredImm, "<=", st, dt, it);
			case OP_SGI:    return iComp(rs, rd, coloredImm, ">", st, dt, it);
			case OP_SGEI:   return iComp(rs, rd, coloredImm, ">=", st, dt, it);
			case OP_SEQI:   return iComp(rs, rd, coloredImm, "==", st, dt, it);
			case OP_RIT:    return "\e[36mrit\e[39m " + coloredImm;
			case OP_TIMEI:  return "\e[36mtime\e[39m " + coloredImm;
			case OP_RINGI:  return "\e[36mring\e[39m " + coloredImm;
			case OP_CMPI:   return color(rs, st) + " " + colorOper("~") + " " + coloredImm;
			case OP_SSPUSH: return "[:" + coloredImm + " " + color(rs, st);
			case OP_SSPOP:  return "]:" + coloredImm + " " + color(rd, dt);
			case OP_DIVI:   return iAltOp(rs, rd, coloredImm, "/", st, dt, it);
			case OP_DIVII:  return iAltOpInv(rs, rd, coloredImm, "/", st, dt, it);
			case OP_SLLII:  return iAltOpInv(rs, rd, coloredImm, "<<", st, dt, it);
			case OP_SRLII:  return iAltOpInv(rs, rd, coloredImm, ">>>", st, dt, it);
			case OP_SRAII:  return iAltOpInv(rs, rd, coloredImm, ">>", st, dt, it);
			case OP_INT: {
				const std::string base = "\e[36mint\e[39m ";
				switch (immediate) {
					case INT_NULL:   return base + "null";
					case INT_SYSTEM: return base + "system";
					case INT_TIMER:  return base + "timer";
					case INT_PROTEC: return base + "protec";
					default: return base + coloredImm;
				}
			}

		}
		return "I: Opcode[" + std::to_string(opcode) + "], " + color(rs, st) + " " + coloredImm + " -> " +
			color(rd, dt);
	}

	std::string stringifyJType(int opcode, int rs, bool link, Conditions conditions, int, HWord address, int st,
	                           const VM *vm) {
		std::string coloredAddress;
		if (vm != nullptr) {
			const std::string symbol = getSymbol(address, *vm);
			if (!symbol.empty())
				coloredAddress = immColor + symbol + "\e[39m";
		}
		if (coloredAddress.empty())
			coloredAddress = colorNum(address, Why::instructionType);
		if (opcode != OP_J && opcode != OP_JC) {
			return "J: Opcode[" + std::to_string(opcode) + "], " + color(rs, st) + ", " + (link? "" : "don't ")
				+ "link, " + coloredAddress;
		}
		const std::string base = jumpConditions(conditions) + "\e[2m" + (link? "::" : ":") + "\e[22m " + coloredAddress;
		return opcode == OP_JC? base + " \e[38;5;90mif\e[39m " + color(rs, st) : base;
	}

	std::string rAltOp(int rs, int rt, int rd, const std::string &oper, int st, int tt, int dt,
	                   const std::string &suffix) {
		if (rs == rd || rt == rd) {
			const std::string source      = color(rs == rd? rs : rt, rs == rd? st : tt);
			const std::string destination = color(rs == rd? rt : rs, rs == rd? tt : st);
			return source + " " + colorOper(oper + "=") + " " + destination + suffix;
		}

		return color(rs, st) + " " + colorOper(oper) + " " + color(rt, tt) + into + color(rd, dt);
	}

	std::string iAltOp(int rs, int rd, const std::string &immediate, const std::string &oper, int st, int dt, int it) {
		std::string out = color(rs, st) + " \e[1m" + oper + (rs == rd? "=" : "") + "\e[22m " + immediate;
		return rs == rd? out : out + into + color(rd, dt);
	}

	std::string iAltOpInv(int rs, int rd, const std::string &immediate, const std::string &oper, int st, int dt,
	                      int it) {
		return immediate + " \e[1m" + oper + "\e[22m " + color(rs, st) + into + color(rd, dt);
	}

	std::string iMath(int rs, int rd, const std::string &immediate, const std::string &oper, int st, int dt, int it) {
		if (rs == rd) {
			if (immediate == colorNum(1, it))
				return color(rs, st) + colorOper(oper + oper);
			return color(rs, st) + " " + colorOper(oper + "=") + " " + immediate;
		}
		return color(rs, st) + " " + colorOper(oper) + " " + immediate + into + color(rd, dt);
	}

	std::string iComp(int rs, int rd, const std::string &immediate, const std::string &oper, int st, int dt, int it) {
		return color(rs, st) + " " + colorOper(oper) + " " + immediate + into + color(rd, dt);
	}

	std::string jumpConditions(Conditions conditions) {
		switch (conditions) {
			case Conditions::Positive: return "+";
			case Conditions::Negative: return "-";
			case Conditions::Zero: return "0";
			case Conditions::Nonzero: return "*";
			case Conditions::Disabled: return "";
		}
		throw std::runtime_error("Invalid conditions: " + std::to_string(static_cast<int>(conditions)));
	}

	std::string color(int reg, int type) {
		return Why::coloredRegister(reg) + std::string(OperandType(type));
	}

	std::string colorNum(HWord immediate, int it) {
		return immColor + std::to_string(immediate) + "\e[39m";
	}

	std::string colorOper(const std::string &oper) {
		return "\e[1m" + oper + "\e[22m";
	}

	std::string getSymbol(Word location, const VM &vm) {
		for (const auto &[name, symbol]: vm.symbolTable) {
			Word address = symbol.location;
			switch (symbol.type) {
				case SymbolEnum::Data:
					address += vm.dataOffset;
					break;
				case SymbolEnum::Code:
					address += vm.codeOffset;
					break;
				default:
					break;
			}
			if (location == address)
				return name;
		}
		return {};
	}
}
