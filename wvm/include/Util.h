#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace WVM {
	namespace Util {
		std::vector<std::string> split(const std::string &str, const std::string &delimiter, bool condense = true);
		bool parseUL(std::string_view, uint64_t &out, int base = 10);
		bool parseLong(const std::string &, int64_t &out, int base = 10);

		std::string toHex(size_t);
		std::string toHex(const void *);
		std::string toHex(size_t, int pad);

		uint64_t swapEndian(uint64_t n);

		size_t nanotime();

		template <typename T>
		inline T upalign(T num, long alignment) {
			return num + ((alignment - (num % alignment)) % alignment);
		}

		template <typename T>
		inline T updiv(T n, T d) {
			return n / d + (n % d? 1 : 0);
		}

		template <typename Iter>
		std::string join(Iter begin, Iter end, const std::string &delimiter = " ") {
			std::stringstream out;
			bool first = true;
			for (; begin != end; ++begin) {
				if (first)
					first = false;
				else
					out << delimiter;
				out << *begin;
			}
			return out.str();
		}
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
