#include <charconv>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <sstream>

#include "Util.h"
#include "wasm/Types.h"

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

	bool parseUL(std::string_view str, uint64_t &out, int base) {
		if (str.substr(0, 2) == "0x") {
			str.remove_prefix(2);
			base = 16;
		}
		auto [ptr, ec] = std::from_chars(str.begin(), str.end(), out, base);
		return ec == std::errc();
	}

	bool parseLong(const std::string &str, int64_t &out, int base) {
		char *endptr;
		if (str.substr(0, 2) == "0x") {
			const std::string hex = str.substr(2);
			out = strtoll(hex.c_str(), &endptr, 16);
			return static_cast<unsigned long>(endptr - hex.c_str()) == hex.size();
		}
		out = strtoll(str.c_str(), &endptr, base);
		return static_cast<unsigned long>(endptr - str.c_str()) == str.size();
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

	size_t nanotime() {
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch())
			.count();
	}
}

Wasmc::TypedInstruction::operator std::string() const {
	return WVM::Util::toHex(instruction, 16) + "/" + WVM::Util::toHex(typeInfo, 8);
}
