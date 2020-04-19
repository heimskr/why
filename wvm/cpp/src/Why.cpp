#include <iomanip>
#include <sstream>
#include <tuple>

#include "Why.h"

namespace WVM {
	bool Why::isSpecialPurpose(int reg) {
		return 0 <= reg && reg < 128 && (reg < temporaryOffset || savedOffset + savedCount <= reg);
	}

	std::string Why::registerName(int reg) {
		switch (reg) {
			case              zeroOffset: return "0";
			case globalAreaPointerOffset: return "g";
			case      stackPointerOffset: return "sp";
			case      framePointerOffset: return "fp";
			case     returnAddressOffset: return "rt";
			case                loOffset: return "lo";
			case                hiOffset: return "hi";
			case            statusOffset: return "st";
		}

		static std::initializer_list<std::tuple<const int, const int, char>> list {
			{returnValueOffset, returnValueCount, 'r'},
			{argumentOffset,    argumentCount,    'a'},
			{temporaryOffset,   temporaryCount,   't'},
			{savedOffset,       savedCount,       's'},
			{kernelOffset,      kernelCount,      'k'},
			{assemblerOffset,   assemblerCount,   'm'},
			{floatingOffset,    floatingCount,    'f'},
			{exceptionOffset,   exceptionCount,   'e'},
		};

		for (auto [offset, count, prefix]: list) {
			if (offset <= reg && reg < offset + count) {
				std::stringstream out;
				out << prefix << std::hex << (reg - offset);
				return out.str();
			}
		}

		return "[" + std::to_string(reg) + "?]";
	}
}
