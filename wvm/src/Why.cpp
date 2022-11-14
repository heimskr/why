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

	std::string Why::coloredRegister(int reg) {
		switch (reg) {
			case              zeroOffset: return "\e[90m$0\e[39m";
			case globalAreaPointerOffset: return "\e[36m$g\e[39m";
			case      stackPointerOffset: return "\e[31m$sp\e[39m";
			case      framePointerOffset: return "\e[35m$fp\e[39m";
			case     returnAddressOffset: return "\e[32m$rt\e[39m";
			case                loOffset: return "\e[31m$lo\e[39m";
			case                hiOffset: return "\e[31m$hi\e[39m";
			case            statusOffset: return "$st";
		}

		static std::initializer_list<std::tuple<const int, const int, const char *>> list {
			{returnValueOffset, returnValueCount, "\e[31m$r"},
			{argumentOffset,    argumentCount,    "\e[38;5;202m$a"},
			{temporaryOffset,   temporaryCount,   "\e[33m$t"},
			{savedOffset,       savedCount,       "\e[32m$s"},
			{kernelOffset,      kernelCount,      "\e[38;5;33m$k"},
			{exceptionOffset,   exceptionCount,   "\e[38;5;88m$e"},
			{assemblerOffset,   assemblerCount,   "\e[38;5;129m$m"},
			{floatingOffset,    floatingCount,    "\e[38;5;126m$f"},
		};

		for (auto [offset, count, prefix]: list) {
			if (offset <= reg && reg < offset + count) {
				std::stringstream out;
				out << prefix << std::hex << (reg - offset) << "\e[39m";
				return out.str();
			}
		}

		return "$[" + std::to_string(reg) + "?]";
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

		uint64_t offset;
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

	OperandType::OperandType(uint8_t packed):
		isSigned((packed >> 3) & 1), primitive(getPrimitive(packed)), pointerLevel(packed >> 4) {}

	Primitive OperandType::getPrimitive(uint8_t packed) {
		switch (packed & 0b111) {
			case 0b000: return Primitive::Void;
			case 0b001: return Primitive::Char;
			case 0b010: return Primitive::Short;
			case 0b011: return Primitive::Int;
			case 0b100: return Primitive::Long;
			default: throw std::invalid_argument("Unknown primitive type: " + std::to_string(packed & 0b111));
		}
	}

	OperandType::operator std::string() const {
		std::ostringstream oss;
		oss << '{';
		if (primitive != Primitive::Void)
			oss << (isSigned? 's' : 'u');
		oss << static_cast<char>(primitive);
		oss << std::string(static_cast<size_t>(pointerLevel < 0? 0 : pointerLevel), '*');
		oss << '}';
		return oss.str();
	}
}
