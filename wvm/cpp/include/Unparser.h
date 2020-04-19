#ifndef WVM_UNPARSER_H_
#define WVM_UNPARSER_H_

#include <string>

#include "Defs.h"

namespace WVM::Unparser {
	std::string stringify(UWord instruction);

	std::string stringifyRType(int opcode, int rs, int rt, int rd, Conditions, int funct);
	std::string stringifyIType(int opcode, int rs, int rd, Conditions, HWord immediate);
	std::string stringifyJType(int opcode, int rs, bool link, Conditions, HWord address);
}

#endif
