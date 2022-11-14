#include "lib/ansi.h"
#include "Operations.h"
#include "Unparser.h"
#include "VM.h"
#include "Why.h"

namespace WVM::Unparser {
	std::string stringify(UWord instruction, const VM *vm) {
		int opcode = (instruction >> 52) & 0xfff;
		if (opcode == OP_NOP) {
			return "<>";
		} else if (Operations::RSet.count(opcode) == 1) {
			int rs, rt, rd;
			Conditions conditions;
			int flags, funct;
			int st, tt, dt;
			Operations::decodeRType(instruction, rs, rt, rd, conditions, flags, funct, st, tt, dt);
			return stringifyRType(opcode, rs, rt, rd, conditions, funct, st, tt, dt);
		} else if (Operations::ISet.count(opcode) == 1) {
			int rs, rd;
			Conditions conditions;
			int flags;
			HWord immediate;
			int st, dt, it;
			Operations::decodeIType(instruction, rs, rd, conditions, flags, immediate, st, dt, it);
			return stringifyIType(opcode, rs, rd, conditions, flags, immediate, st, dt, it, vm);
		} else if (Operations::JSet.count(opcode) == 1) {
			int rs;
			bool link;
			Conditions conditions;
			int flags;
			HWord address;
			int st;
			Operations::decodeJType(instruction, rs, link, conditions, flags, address, st);
			return stringifyJType(opcode, rs, link, conditions, flags, address, vm);
		} else {
			DBG(std::hex << instruction << std::dec);
			return "???";
			throw std::runtime_error("[U] Unknown opcode: " + std::to_string(opcode));
		}
	}

	std::string stringifyRType(int opcode, int rs, int rt, int rd, Conditions conditions, int funct, int st, int tt,
	                           int dt) {
		switch (opcode) {
			case OP_RMATH: {
				std::string oper, suffix;
				switch (funct) {
					case FN_ADD:    oper = "+"; break;
					case FN_SUB:    oper = "-"; break;
					case FN_MULT:   return color(rs) + " " + colorOper("*") + " " + color(rd);
					case FN_SLL:    oper = "<<"; break;
					case FN_SRL:    oper = ">>>"; break;
					case FN_SRA:    oper = ">>"; break;
					case FN_MOD:    oper = "%"; break;
					case FN_DIV:    oper = "/"; break;
					case FN_SEXT:   return "\e[1msext32\e[22m " + color(rs) + into + color(rd);
				}
				return rAltOp(rs, rt, rd, oper, suffix, st, tt, dt);
			}
			case OP_RLOGIC: {
				std::string oper;
				switch (funct) {
					case FN_AND:   oper = "&";   break;
					case FN_NAND:  oper = "~&";  break;
					case FN_NOR:   oper = "~|";  break;
					case FN_NOT:
						return "\e[1m~\e[22m" + color(rs) + into + color(rd);
					case FN_OR:
						if (rs == Why::zeroOffset)
							return color(rt) + into + color(rd);
						if (rt == Why::zeroOffset)
							return color(rs) + into + color(rd);
						oper = "|";
						break;
					case FN_XNOR:  oper = "~x";  break;
					case FN_XOR:   oper = "x";   break;
					case FN_LAND:  oper = "&&";  break;
					case FN_LNAND: oper = "~&&"; break;
					case FN_LNOR:  oper = "~||"; break;
					case FN_LNOT:
						if (rs == rd)
							return colorOper("!") + color(rs) + colorOper(".");
						return colorOper("!") + color(rs) + into + color(rd);
					case FN_LOR:   oper = "||";  break;
					case FN_LXNOR: oper = "~xx"; break;
					case FN_LXOR:  oper = "xx";  break;
				}
				return rAltOp(rs, rt, rd, oper, st, tt, dt);
			}
			case OP_RCOMP: {
				if (funct == FN_CMP)
					return color(rs) + " " + colorOper("~") + " " + color(rt);
				std::string out = color(rs) + " \e[1m";
				if (funct == FN_SL  || funct == FN_SLU) out += "<";
				else if (funct == FN_SLE || funct == FN_SLEU) out += "<=";
				else if (funct == FN_SEQ) out += "==";
				else out += "?";
				out += "\e[22m " + color(rt) + into + color(rd);
				if (funct == FN_SLU || funct == FN_SLEU) out += " /u";
				return out;
			}
			case OP_RJUMP: {
				switch (funct) {
					case FN_JR:   return "\e[2m" + jumpConditions(conditions) + ":\e[22m " + color(rd);
					case FN_JRC:  return "\e[2m:\e[22m " + color(rd) + " \e[31mif\e[39m " + color(rs);
					case FN_JRL:  return "\e[2m" + jumpConditions(conditions) + "::\e[22m " + color(rd);
					case FN_JRLC: return "\e[2m::\e[22m " + color(rd) + " \e[31mif\e[39m " + color(rs);
				}
				break;
			}
			case OP_RMEM:
				switch (funct) {
					case FN_C:     return left + color(rs) + right + into + left + color(rd) + right;
					case FN_L:     return left + color(rs) + right + into + color(rd);
					case FN_S:     return color(rs) + into + left + color(rd) + right;
					case FN_CB:    return left + color(rs) + right + into + left + color(rd) + right + " /b";
					case FN_LB:    return left + color(rs) + right + into + color(rd) + " /b";
					case FN_SB:    return color(rs) + into + left + color(rd) + right + " /b";
					case FN_SPUSH: return "\e[2m[\e[22m " + color(rs);
					case FN_SPOP:  return "\e[2m]\e[22m " + color(rd);
					case FN_CH:    return left + color(rs) + right + into + left + color(rd) + "] /h";
					case FN_LH:    return left + color(rs) + right + into + color(rd) + " /h";
					case FN_SH:    return color(rs) + into + left + color(rd) + right + " /h";
					case FN_MS:    return "\e[36mmemset\e[39m " + color(rs) + " x " + color(rt) + into + color(rd);
					case FN_CS:    return left + color(rs) + right + into + left + color(rd) + "] /s";
					case FN_LS:    return left + color(rs) + right + into + color(rd) + " /s";
					case FN_SS:    return color(rs) + into + left + color(rd) + right + " /s";
				}
				break;
			case OP_REXT:
				switch (funct) {
					case FN_PR:    return "<\e[36mprint\e[39m " + color(rs) + ">";
					case FN_HALT:  return "<\e[36mhalt\e[39m>";
					case FN_EVAL:  return "<\e[36meval\e[39m "  + color(rs) + ">";
					case FN_PRC:   return "<\e[36mprc\e[39m "   + color(rs) + ">";
					case FN_PRD:   return "<\e[36mprd\e[39m "   + color(rs) + ">";
					case FN_PRX:   return "<\e[36mprx\e[39m "   + color(rs) + ">";
					case FN_SLEEP: return "<\e[36msleep\e[39m " + color(rs) + ">";
					case FN_REST:  return "<\e[36mrest\e[39m>";
					case FN_IO:    return "<\e[36mio\e[39m>";
				}
				break;
			case OP_TIME: return "\e[36m%time\e[39m " + color(rs);
			case OP_RING: return "\e[36m%ring\e[39m " + color(rs);
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
							return "\e[36m%setpt\e[39m " + color(rs);
						return "\e[2m:\e[22m \e[36m%setpt\e[39m " + color(rs) + " " + color(rt);
					case FN_SVPG:  return "\e[36m%page\e[39m \e[2m->\e[22m " + color(rd);
					case FN_PPUSH: return "\e[2m[\e[22m \e[36m%page\e[39m";
					case FN_PPOP:
						if (rs == 0)
							return "\e[2m]\e[22m \e[36m%page\e[39m";
						return "\e[2m: ]\e[22m \e[36m%page\e[39m " + color(rs);
				}
				break;
			case OP_QUERY:
				switch (funct) {
					case FN_QM: return "? \e[36mmem\e[39m \e[2m->\e[22m " + color(rd);
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

	std::string stringifyIType(int opcode, int rs, int rd, Conditions, int, HWord immediate, const VM *vm) {
		std::string coloredImm;
		if (vm) {
			const std::string symbol = getSymbol(immediate, *vm);
			if (!symbol.empty())
				coloredImm = immColor + symbol + "\e[39m";
		}
		if (coloredImm.empty())
			coloredImm = colorNum(immediate);
		switch (opcode) {
			case OP_ADDI:   return iMath(rs, rd, coloredImm, "+");
			case OP_SUBI:   return iMath(rs, rd, coloredImm, "-");
			case OP_MULTI:  return color(rs) + " \e[1m*\e[22m " + coloredImm;
			case OP_MULTUI: return color(rs) + " \e[1m*\e[22m " + coloredImm + " /u";
			case OP_MODI:   return iAltOp(rs, rd, coloredImm, "%");
			case OP_ANDI:   return iAltOp(rs, rd, coloredImm, "&");
			case OP_NANDI:  return iAltOp(rs, rd, coloredImm, "~&");
			case OP_NORI:   return iAltOp(rs, rd, coloredImm, "~|");
			case OP_ORI:    return iAltOp(rs, rd, coloredImm, "|");
			case OP_XNORI:  return iAltOp(rs, rd, coloredImm, "~x");
			case OP_XORI:   return iAltOp(rs, rd, coloredImm, "x");
			case OP_SLLI:   return iAltOp(rs, rd, coloredImm, "<<");
			case OP_SRLI:   return iAltOp(rs, rd, coloredImm, ">>>");
			case OP_SRAI:   return iAltOp(rs, rd, coloredImm, ">>");
			case OP_LUI:    return "\e[2mlui:\e[22m " + coloredImm + into + color(rd);
			case OP_LI:     return left + coloredImm + right + into + color(rd);
			case OP_LBI:    return left + coloredImm + right + into + color(rd) + " /b";
			case OP_LNI:    return left + coloredImm + right + into + left + color(rd) + right;
			case OP_LBNI:   return left + coloredImm + right + into + left + color(rd) + right + " /b";
			case OP_SI:     return color(rs) + into + left + coloredImm + right;
			case OP_SBI:    return color(rs) + into + left + coloredImm + right + " /b";
			case OP_SET:    return coloredImm + into + color(rd);
			case OP_SLI:    return iComp(rs, rd, coloredImm, "<");
			case OP_SLUI:   return iComp(rs, rd, coloredImm, "<") + " /u";
			case OP_SLEI:   return iComp(rs, rd, coloredImm, "<=");
			case OP_SLEUI:  return iComp(rs, rd, coloredImm, "<=") + " /u";
			case OP_SGI:    return iComp(rs, rd, coloredImm, ">");
			case OP_SGEI:   return iComp(rs, rd, coloredImm, ">=");
			case OP_SEQI:   return iComp(rs, rd, coloredImm, "==");
			case OP_RIT:    return "\e[36mrit\e[39m " + coloredImm;
			case OP_TIMEI:  return "\e[36mtime\e[39m " + coloredImm;
			case OP_RINGI:  return "\e[36mring\e[39m " + coloredImm;
			case OP_CMPI:   return color(rs) + " " + colorOper("~") + " " + coloredImm;
			case OP_SSPUSH: return "[:" + coloredImm + " " + color(rs);
			case OP_SSPOP:  return "]:" + coloredImm + " " + color(rd);
			case OP_DIVI:   return iAltOp(rs, rd, coloredImm, "/");
			case OP_DIVUI:  return iAltOp(rs, rd, coloredImm, "/") + " /u";
			case OP_DIVII:  return iAltOpInv(rs, rd, coloredImm, "/");
			case OP_DIVUII: return iAltOpInv(rs, rd, coloredImm, "/") + " /u";
			case OP_SLLII:  return iAltOpInv(rs, rd, coloredImm, "<<");
			case OP_SRLII:  return iAltOpInv(rs, rd, coloredImm, ">>>");
			case OP_SRAII:  return iAltOpInv(rs, rd, coloredImm, ">>");
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
		return "I: Opcode[" + std::to_string(opcode) + "], " + color(rs) + " " + coloredImm + " -> " + color(rd);
	}

	std::string stringifyJType(int opcode, int rs, bool link, Conditions conditions, int, HWord address, const VM *vm) {
		std::string coloredAddress;
		if (vm) {
			const std::string symbol = getSymbol(address, *vm);
			if (!symbol.empty())
				coloredAddress = immColor + symbol + "\e[39m";
		}
		if (coloredAddress.empty())
			coloredAddress = colorNum(address);
		if (opcode != OP_J && opcode != OP_JC) {
			return "J: Opcode[" + std::to_string(opcode) + "], " + color(rs) + ", " + (link? "" : "don't ")
				+ "link, " + coloredAddress;
		}
		const std::string base = jumpConditions(conditions) + "\e[2m" + (link? "::" : ":") + "\e[22m " + coloredAddress;
		return opcode == OP_JC? base + " \e[38;5;90mif\e[39m " + color(rs) : base;
	}

	std::string rAltOp(int rs, int rt, int rd, const std::string &oper, int st, int tt, int dt,
	                   const std::string &suffix) {
		if (rs == rd || rt == rd) {
			const std::string source      = color(rs == rd? rs : rt, rs == rd? st : tt);
			const std::string destination = color(rs == rd? rt : rs, rs == rd? tt : st);
			return source + " " + colorOper(oper + "=") + " " + destination + suffix;
		}

		return color(rs) + " " + colorOper(oper) + " " + color(rt) + into + color(rd);
	}

	std::string iAltOp(int rs, int rd, const std::string &immediate, const std::string &oper) {
		std::string out = color(rs) + " \e[1m" + oper + (rs == rd? "=" : "") + "\e[22m " + immediate;
		return rs == rd? out : out + into + color(rd);
	}

	std::string iAltOpInv(int rs, int rd, const std::string &immediate, const std::string &oper) {
		return immediate + " \e[1m" + oper + "\e[22m " + color(rs) + into + color(rd);
	}

	std::string iMath(int rs, int rd, const std::string &immediate, const std::string &oper) {
		if (rs == rd) {
			if (immediate == colorNum(1))
				return color(rs) + colorOper(oper + oper);
			return color(rs) + " " + colorOper(oper + "=") + " " + immediate;
		}
		return color(rs) + " " + colorOper(oper) + " " + immediate + into + color(rd);
	}

	std::string iComp(int rs, int rd, const std::string &immediate, const std::string &oper) {
		return color(rs) + " " + colorOper(oper) + " " + immediate + into + color(rd);
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
		return Why::coloredRegister(reg);
	}

	std::string colorNum(HWord immediate) {
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
