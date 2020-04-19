#ifndef WVM_UTIL_H_
#define WVM_UTIL_H_

#include <string>
#include <vector>

namespace WVM::Util {
	std::vector<std::string> split(const std::string &str, const std::string &delimiter, bool condense = true);
	bool parseUL(const std::string &str, unsigned long int &out, int base = 10);
}

#endif
