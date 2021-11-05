#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "util/Hash.h"
#include "wasm/Types.h"

namespace Wasmc {
	struct SymbolTableEntry {
		uint32_t id;
		Long address;
		SymbolEnum type;
		SymbolTableEntry(uint32_t id_, Long address_, SymbolEnum type_): id(id_), address(address_), type(type_) {}
		std::vector<Long> encode(const std::string &name) const;
	};

	using SymbolTable = std::unordered_map<std::string, SymbolTableEntry>;
}

namespace std {
	template <>
	class hash<Wasmc::SymbolTableEntry>: public Wasmc::Util::Hash<Wasmc::SymbolTableEntry> {};
}
