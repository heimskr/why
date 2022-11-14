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

			void applyRelocation(std::vector<RelocationData> &relocation,
			                     const std::vector<SymbolTableEntry> &symbols,
			                     const std::map<std::string, size_t> &symbol_indices,
			                     std::vector<Long> &data, std::vector<TypedInstruction> &code,
			                     Long data_offset, Long code_offset);

			static std::string findSymbolFromAddress(Long address, const SymbolTable &, Long end_offset);

			static std::string findSymbolFromID(uint32_t id, const SymbolTable &);

			static size_t countStringTypes(const std::vector<std::shared_ptr<DebugEntry>> &);

			static void
			detectSymbolCollisions(const std::map<std::string, size_t> &, const std::map<std::string, size_t> &,
	                               const std::vector<SymbolTableEntry> &, const std::vector<SymbolTableEntry> &);

			static std::vector<Long> encodeDebugData(const std::vector<std::shared_ptr<Wasmc::DebugEntry>> &);

			static std::vector<Long> encodeSymbolTable(const std::vector<SymbolTableEntry> &);

			static std::vector<Long> encodeRelocationData(const std::vector<RelocationData> &);
	};
}
