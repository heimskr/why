#ifndef WVM_UTIL_H_
#define WVM_UTIL_H_

#include <iostream>
#include <string>
#include <vector>

namespace WVM {
	namespace Util {
		std::vector<std::string> split(const std::string &str, const std::string &delimiter, bool condense = true);
		bool parseUL(const std::string &str, uint64_t &out, int base = 10);
		bool parseLong(const std::string &str, int64_t &out, int base = 10);
	}

	constexpr const char *errorPrefix = "\e[2m[\e[22;31m!\e[39;2m]\e[22m ";
	constexpr const char *warnPrefix  = "\e[2m[\e[22;33m~\e[39;2m]\e[22m ";
	constexpr const char *infoPrefix  = "\e[2m[\e[22;36mi\e[39;2m]\e[22m ";

	inline std::ostream & warn() {
		return std::cerr << warnPrefix  << "\e[33mWarning: \e[39m";
	}

	inline std::ostream & error() {
		return std::cerr << errorPrefix << "\e[31mError: \e[39m";
	}

	inline std::ostream & info() {
		return std::cerr << infoPrefix;
	}
}

#endif
