#pragma once

#include <string>

namespace WVM {
	enum class Endianness: char {Big='B', Little='L'};

	enum class Primitive: char {Void = 'v', Char = 'c', Short = 's', Int = 'i', Long = 'l'};

	class OperandType {
		public:
			bool isSigned = true;
			Primitive primitive = Primitive::Void;
			int pointerLevel = -1;
			OperandType(): isSigned(true), primitive(Primitive::Void), pointerLevel(-1) {}
			OperandType(bool is_signed, Primitive primitive_, int pointer_level = 0):
				isSigned(is_signed), primitive(primitive_), pointerLevel(pointer_level) {}
			OperandType(uint8_t);
			operator std::string() const;

		private:
			static Primitive getPrimitive(uint8_t);
	};

	/** Contains constants pertaining to the Why architecture. */
	struct Why {
		constexpr static int wordSize = 8; // in bytes
		constexpr static int totalRegisters = 128;
		constexpr static int instructionSize = 12;

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
		static int registerID(std::string);
		static std::string stringifyType(int);
	};
}
