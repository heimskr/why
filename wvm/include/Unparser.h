#pragma once

#include <string>

#include "Defs.h"

namespace WVM::Unparser {
	std::string stringify(UWord instruction, const VM *vm = nullptr);

	std::string stringifyRType(int opcode, int rs, int rt, int rd, Conditions, int funct, int st, int tt, int dt);
	std::string stringifyIType(int opcode, int rs, int rd, Conditions, int flags, HWord immediate, int st, int dt,
	                           int it, const VM * = nullptr);
	std::string stringifyJType(int opcode, int rs, bool link, Conditions, int flags, HWord address, int st,
	                           const VM * = nullptr);

	std::string rAltOp(int rs, int rt, int rd, const std::string &oper, int st, int tt, int dt,
	                   const std::string &suffix = "");
	std::string iAltOp(int rs, int rd, const std::string &immediate, const std::string &oper, int st, int dt, int it);
	std::string iAltOpInv(int rs, int rd, const std::string &immediate, const std::string &oper, int st, int dt,
	                      int it);
	std::string iMath(int rs, int rd, const std::string &immediate, const std::string &oper, int st, int dt, int it);
	std::string iComp(int rs, int rd, const std::string &immediate, const std::string &oper, int st, int dt, int it);

	std::string jumpConditions(Conditions);
	std::string color(int reg, int type);
	std::string colorNum(int reg, int type);
	std::string colorOper(const std::string &oper);

	std::string getSymbol(Word location, const VM &);

	constexpr const char *immColor = "\e[38;5;164m";
	constexpr const char *into = " \e[2m->\e[22m ";
	constexpr const char *left = "[", *right = "]";
}
