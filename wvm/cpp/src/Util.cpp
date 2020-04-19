#include <cstdlib>

#include "Util.h"

namespace WVM::Util {
	std::vector<std::string> split(const std::string &str, const std::string &delimiter, bool condense) {
		if (str.empty())
			return {};

		size_t next = str.find(delimiter);
		if (next == std::string::npos)
			return {str};

		std::vector<std::string> out {};
		const size_t delimiter_length = delimiter.size();
		size_t last = 0;

		out.push_back(str.substr(0, next));

		while (next != std::string::npos) {
			last = next;
			next = str.find(delimiter, last + delimiter_length);
			std::string sub = str.substr(last + delimiter_length, next - last - delimiter_length);
			if (!sub.empty() || !condense)
				out.push_back(std::move(sub));
		}

		return out;
	}

	bool parseUL(const std::string &str, unsigned long int &out, int base) {
		char *endptr;
		out = strtoul(str.c_str(), &endptr, base);
		return static_cast<unsigned long>(endptr - str.c_str()) == str.size();
	}
}
