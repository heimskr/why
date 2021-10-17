#pragma once

#include <map>
#include <string>

namespace Wasmc::Why {
	constexpr int wordSize = 8; // in bytes
	constexpr int totalRegisters = 128;

	constexpr int returnValueOffset = 7, returnValueCount = 16;
	constexpr int    argumentOffset = 23,   argumentCount = 16;
	constexpr int   temporaryOffset = 39,  temporaryCount = 23;
	constexpr int       savedOffset = 62,      savedCount = 23;
	constexpr int      kernelOffset = 85,     kernelCount = 16;
	constexpr int   assemblerOffset = 102, assemblerCount = 16;
	constexpr int    floatingOffset = 118,  floatingCount = 4;
	constexpr int   exceptionOffset = 122, exceptionCount = 6;

	constexpr int zeroOffset = 0,
	 globalAreaPointerOffset = 1,
	      stackPointerOffset = 2,
	      framePointerOffset = 3,
	     returnAddressOffset = 4,
	                loOffset = 5,
	                hiOffset = 6,
	            statusOffset = 101;

	constexpr int ringMin = 0, ringMax = 3;

	constexpr int halfFloatWidth = 4, floatWidth = 8, doubleWidth = 8, pointerWidth = 8; // in bytes
	constexpr int generalPurposeRegisters = temporaryCount + savedCount;

	extern std::map<std::string, int> ioIDs;
}
