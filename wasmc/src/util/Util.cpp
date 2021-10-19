#include <cstdlib>
#include <iomanip>
#include <stdexcept>

#include "util/Util.h"

namespace Wasmc::Util {
	long parseLong(const std::string &str, int base) {
		const char *c_str = str.c_str();
		char *end;
		long parsed = strtol(c_str, &end, base);
		if (c_str + str.length() != end)
			throw std::invalid_argument("Not an integer: \"" + str + "\"");
		return parsed;
	}

	long parseLong(const std::string *str, int base) {
		return parseLong(*str, base);
	}

	long parseLong(const char *str, int base) {
		return parseLong(std::string(str), base);
	}

	unsigned long parseUlong(const std::string &str, int base) {
		std::size_t pos;
		unsigned long parsed = std::stoul(str, &pos, base);
		if (pos != str.size())
			throw std::invalid_argument("Not an integer: \"" + str + "\"");
		return parsed;
	}

	unsigned long parseUlong(const std::string *str, int base) {
		return parseUlong(*str, base);
	}

	unsigned long parseUlong(const char *str, int base) {
		return parseUlong(std::string(str), base);
	}

	double parseDouble(const std::string &str) {
		const char *c_str = str.c_str();
		char *end;
		double parsed = strtod(c_str, &end);
		if (c_str + str.length() != end)
			throw std::invalid_argument("Not a float: \"" + str + "\"");
		return parsed;
	}

	double parseDouble(const std::string *str) {
		return parseDouble(*str);
	}

	double parseDouble(const char *str) {
		return parseDouble(std::string(str));
	}

	bool isNumeric(const std::string &str) {
		if (str.empty())
			return false;
		for (char c: str) {
			if (c < '0' || '9' < c)
				return false;
		}
		return true;
	}

	bool isNumeric(const std::string *str) {
		return isNumeric(*str);
	}

	bool isNumeric(const char *str) {
		return isNumeric(std::string(str));
	}

	bool isHex(const char ch) {
		return ('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'f') || ('A' <= ch && ch <= 'F');
	}

	std::string escape(const std::string &str) {
		std::stringstream out;
		for (char ch: str) {
			if (ch == '"' || ch == '\\')
				out << '\\' << ch;
			else if (ch == '\t')
				out << "\\t";
			else if (ch == '\n')
				out << "\\n";
			else if (ch == '\r')
				out << "\\r";
			else
				out << ch;
		}
		return out.str();
	}

	std::string unescape(const std::string &str) {
		const size_t size = str.size();
		if (size == 0)
			return "";
		std::stringstream out;
		for (size_t i = 0; i < size; ++i) {
			char ch = str[i];
			if (ch == '\\') {
				if (i == size - 1)
					throw std::runtime_error("Backslash at end of string");
				switch (str[++i]) {
					case 'n':  out << '\n'; break;
					case 'r':  out << '\r'; break;
					case 't':  out << '\t'; break;
					case '\\': out << '\\'; break;
					case 'x': {
						if (size <= i + 2)
							throw std::runtime_error("Hexadecimal escape is too close to end of string");
						const char first = str[++i], second = str[++i];
						if (!isxdigit(first) || !isxdigit(second))
							throw std::runtime_error(std::string("Invalid hexadecimal escape: \\x") + first + second);
						out << static_cast<char>(strtol((std::string(1, first) + second).c_str(), nullptr, 16));
						break;
					}
					default: throw std::runtime_error("Unrecognized character: \\" + std::string(1, str[i]));
				}
			} else
				out << ch;
		}
		return out.str();
	}

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

	std::string toHex(size_t n) {
		std::stringstream ss;
		ss.imbue(std::locale::classic());
		ss << "0x" << std::hex << n;
		return ss.str();
	}

	std::string toHex(size_t n, int pad) {
		std::stringstream ss;
		ss.imbue(std::locale::classic());
		ss << "0x" << std::hex << std::right << std::setw(pad) << std::setfill('0') << n;
		return ss.str();
	}

	std::string toHex(const void *ptr) {
		std::stringstream ss;
		ss.imbue(std::locale::classic());
		ss << ptr;
		return ss.str();
	}

	uint64_t swapEndian(uint64_t n) {
		return __builtin_bswap64(n);
	}
}
