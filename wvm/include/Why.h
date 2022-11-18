#pragma once

#include <ostream>
#include <string>

namespace WVM {
	enum class Endianness: char {Big='B', Little='L'};

	enum class Primitive: char {Void = 'v', Char = 'c', Short = 's', Int = 'i', Long = 'l'};

	struct OperandType {
		bool isSigned = true;
		Primitive primitive = Primitive::Void;
		int pointerLevel = -1;

		static Primitive getPrimitive(uint8_t triple) {
			switch (triple & 0b111) {
				case 0b000: return Primitive::Void;
				case 0b001: return Primitive::Char;
				case 0b010: return Primitive::Short;
				case 0b011: return Primitive::Int;
				case 0b100: return Primitive::Long;
				default: throw std::invalid_argument("Invalid primitive triple: " + std::to_string(triple & 0b111));
			}
		}

		OperandType(): isSigned(true), primitive(Primitive::Void), pointerLevel(-1) {}

		OperandType(uint8_t type):
			isSigned(((type >> 3) & 1) == 1), primitive(getPrimitive(type)), pointerLevel((type >> 4) & 0b1111) {}

		OperandType(bool is_signed, Primitive primitive_, int pointer_level = 0):
			isSigned(is_signed), primitive(primitive_), pointerLevel(pointer_level) {}

		operator std::string() const {
			std::ostringstream oss;
			oss << '{';
			if (primitive != Primitive::Void)
				oss << (isSigned? 's' : 'u');
			oss << static_cast<char>(primitive);
			oss << std::string(static_cast<size_t>(pointerLevel < 0? 0 : pointerLevel), '*');
			oss << '}';
			return oss.str();
		}

		explicit operator uint8_t() const {
			uint8_t out = ((pointerLevel & 0xf) << 4) | (isSigned? 0b1000 : 0b0000);
			switch (primitive) {
				case Primitive::Void: break;
				case Primitive::Char:
					out |= 0b001;
					break;
				case Primitive::Short:
					out |= 0b010;
					break;
				case Primitive::Int:
					out |= 0b011;
					break;
				case Primitive::Long:
					out |= 0b100;
					break;
				default:
					throw std::invalid_argument("Invalid primitive: " + std::to_string(static_cast<int>(primitive)));
			}
			return out;
		}

		bool operator==(const OperandType &other) const {
			return this == &other ||
				(isSigned == other.isSigned && primitive == other.primitive && pointerLevel == other.pointerLevel);
		}

		bool operator==(uint8_t other) const {
			return static_cast<uint8_t>(*this) == other;
		}

		bool isNumber() const {
			return primitive != Primitive::Void && pointerLevel == 0;
		}

		static OperandType VOID_PTR;
		static OperandType ULONG;
	};

	/** Contains constants pertaining to the Why architecture. */
	struct Why {
		constexpr static int wordSize = 8; // in bytes
		constexpr static int totalRegisters = 128;
		constexpr static int instructionSize = 12;

		constexpr static uint8_t instructionType = 0b00010000;

		constexpr static int returnValueOffset = 7, returnValueCount = 16;
		constexpr static int    argumentOffset = 23,   argumentCount = 16;
		constexpr static int   temporaryOffset = 39,  temporaryCount = 23;
		constexpr static int       savedOffset = 62,      savedCount = 23;
		constexpr static int      kernelOffset = 85,     kernelCount = 16;
		constexpr static int   assemblerOffset = 102, assemblerCount = 16;
		constexpr static int    floatingOffset = 118,  floatingCount = 4;
		constexpr static int   exceptionOffset = 122, exceptionCount = 6;

		constexpr static int zeroOffset = 0,
		        globalAreaPointerOffset = 1,
		             stackPointerOffset = 2,
		             framePointerOffset = 3,
		            returnAddressOffset = 4,
		                       loOffset = 5,
		                       hiOffset = 6,
		                   statusOffset = 101;

		constexpr static int ringMin = 0, ringMax = 3;

		constexpr static int halfFloatWidth = 4, floatWidth = 8, doubleWidth = 8, pointerWidth = 8; // in bytes
		constexpr static int generalPurposeRegisters = temporaryCount + savedCount;

		static bool isSpecialPurpose(int);
		static std::string registerName(int);
		static std::string coloredRegister(int);
		static int registerID(std::string_view);
		static std::string stringifyType(int);
	};

	std::ostream & operator<<(std::ostream &, const WVM::OperandType &);
}
