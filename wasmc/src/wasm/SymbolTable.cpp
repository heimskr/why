#include "util/Util.h"
#include "wasm/SymbolTable.h"

namespace Wasmc {
	std::vector<Long> SymbolTableEntry::encode(const std::string &name) const {
		std::vector<Long> out;
		size_t name_words = Util::updiv(name.size(), 8ul);
		out.reserve(3 + name_words);
		out.push_back((uint64_t(id) << 32ul) | (uint32_t(type) << 16) | uint16_t(name_words));
		const std::vector<Long> name_longs = Util::getLongs(name);
		out.insert(out.end(), name_longs.begin(), name_longs.end());
		return out;
	}
}