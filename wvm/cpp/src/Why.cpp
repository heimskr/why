#include <iomanip>
#include <sstream>
#include <tuple>

#include "Util.h"
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
			{exceptionOffset,   exceptionCount,   'e'},
			{assemblerOffset,   assemblerCount,   'm'},
			{floatingOffset,    floatingCount,    'f'},
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

	int Why::registerID(std::string name) {
		if (name.empty() || name == "$")
			return -1;

		if (name.front() == '$')
			name.erase(0, 1);

		if (name == "0")  return zeroOffset;
		if (name == "g")  return globalAreaPointerOffset;
		if (name == "sp") return stackPointerOffset;
		if (name == "fp") return framePointerOffset;
		if (name == "rt") return returnAddressOffset;
		if (name == "lo") return loOffset;
		if (name == "hi") return hiOffset;
		if (name == "st") return statusOffset;

		const char front = name.front();
		if (name.size() == 1 || std::string("ratskemf").find(front) == std::string::npos)
			return -1;

		unsigned long int offset;
		if (!Util::parseUL(name.substr(1), offset, 16))
			return -1;

		switch (front) {
			case 'r': return returnValueOffset + offset;
			case 'a': return    argumentOffset + offset;
			case 't': return   temporaryOffset + offset;
			case 's': return       savedOffset + offset;
			case 'k': return      kernelOffset + offset;
			case 'e': return   exceptionOffset + offset;
			case 'm': return   assemblerOffset + offset;
			case 'f': return    floatingOffset + offset;
			default:  return -1;
		}
	}
}
