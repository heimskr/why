#ifndef WVM_UTIL_H_
#define WVM_UTIL_H_

#include <iostream>
#include <string>
#include <vector>


namespace WVM {
	namespace Util {
		std::vector<std::string> split(const std::string &str, const std::string &delimiter, bool condense = true);
		bool parseUL(const std::string &str, unsigned long int &out, int base = 10);
	}

	inline std::ostream & warn() {
		return std::cerr << "\e[2m[\e[22;33m!\e[39;2m]\e[22;33m Warning: \e[39m";
	}

	inline std::ostream & error() {
		return std::cerr << "\e[2m[\e[22;31m!\e[39;2m]\e[22;31m Error: \e[39m";
	}

	inline std::ostream & info() {
		return std::cerr << "\e[2m[\e[22;36mi\e[39;2m]\e[22m ";
	}
}

#endif
