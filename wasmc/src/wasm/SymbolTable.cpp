#include "util/Util.h"
#include "wasm/Assembler.h"
#include "wasm/SymbolTable.h"

namespace Wasmc {
	SymbolTableEntry::SymbolTableEntry(uint32_t id_, Long address_, SymbolEnum type_):
		id(id_), address(address_), type(type_) {}

	SymbolTableEntry::SymbolTableEntry(const std::string &label_, Long address_, SymbolEnum type_):
		id(Assembler::encodeSymbol(label_)), address(address_), type(type_), label(label_) {}

	SymbolTableEntry::SymbolTableEntry(uint32_t id_, const std::string &label_, Long address_, SymbolEnum type_):
	id(id_), address(address_), type(type_), label(label_) {
		if (Assembler::encodeSymbol(label_) != id_)
			throw std::invalid_argument("Symbol ID " + Util::toHex(id_) + " doesn't match computed symbol ID for "
				"symbol \"" + label_ + "\"");
	}

	std::vector<Long> SymbolTableEntry::encode(const std::string &name) const {
		std::vector<Long> out;
		size_t name_words = Util::updiv(name.size(), 8ul);
		out.reserve(3 + name_words);
		out.push_back((uint64_t(id) << 32ul) | (uint32_t(type) << 16) | uint16_t(name_words));
		out.push_back(address);
		const std::vector<Long> name_longs = Util::getLongs(name);
		out.insert(out.end(), name_longs.begin(), name_longs.end());
		return out;
	}

	std::vector<Long> SymbolTableEntry::encode() const {
		if (label.empty())
			throw std::runtime_error("Can't encode SymbolTableEntry: label is empty");
		return encode(label);
	}
}
