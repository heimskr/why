#pragma once

#include <filesystem>
#include <set>
#include <vector>

#include "wasm/SymbolTable.h"
#include "wasm/Types.h"

namespace Wasmc {
	class ASTNode;

	class Linker {
		public:
			Linker() = default;

			void addFile(const std::filesystem::path &);

			std::string link();

			const std::vector<std::vector<Long>> & getSubunits() const { return subunits; }

		private:
			std::set<std::filesystem::path> paths;
			bool firstDone = false;
			const ASTNode *root = nullptr;
			std::vector<Long> mainUnit;
			std::vector<std::vector<Long>> subunits;
			std::vector<Long> linked;

			/** Replaces pointers inside all pointer variables of a symbol table
			 *  with the encoded names of the symbols they point to. */
			static void depointerize(const SymbolTable &, std::vector<Long> &data, Long data_offset);

			static std::unordered_map<std::string, SymbolType> collectSymbolTypes(const Offsets &, const SymbolTable &);

			static SymbolType getSymbolType(const Offsets &, const SymbolTable &, const std::string &symbol);

			static void desymbolize(std::vector<Long> &, const Offsets &, const SymbolTable &);

			static std::string findSymbolFromAddress(Long address, const SymbolTable &, Long end_offset);

			static std::string findSymbolFromID(uint32_t id, const SymbolTable &);

			static size_t countStringTypes(std::vector<std::shared_ptr<DebugEntry>> &);

			static void detectSymbolCollisions(const SymbolTable &, const SymbolTable &);

			static std::vector<Long> encodeDebugData(const std::vector<std::shared_ptr<Wasmc::DebugEntry>> &);

			static std::vector<Long> encodeSymbolTable(const SymbolTable &);

			static void resymbolize(std::vector<Long> &, const SymbolTable &);
	};
}
