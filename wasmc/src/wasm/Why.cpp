#include "wasm/Why.h"

namespace Wasmc::Why {
	std::map<std::string, int> ioIDs {
		{"devcount",  0},
		{"seekabs",   1},
		{"seekrel",   2},
		{"read",      3},
		{"write",     4},
		{"getsize",   5},
		{"getcursor", 6},
	};
}
