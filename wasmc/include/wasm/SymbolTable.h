#pragma once

#include <map>
#include <string>
#include <vector>

#include "util/Hash.h"
#include "wasm/Types.h"

namespace Wasmc {
	struct SymbolTableEntry {
		uint32_t id;
		Long address;
		SymbolEnum type;
		std::string label;
		SymbolTableEntry(uint32_t id_, Long address_, SymbolEnum type_);
		SymbolTableEntry(const std::string &label_, Long address_, SymbolEnum type_);
		SymbolTableEntry(uint32_t id_, const std::string &label_, Long address_, SymbolEnum type_);
		std::vector<Long> encode(const std::string &name) const;
		std::vector<Long> encode() const;
	};

	using SymbolTable = std::map<std::string, SymbolTableEntry>;
}

namespace std {
	template <>
	class hash<Wasmc::SymbolTableEntry>: public Wasmc::Util::Hash<Wasmc::SymbolTableEntry> {};
}
