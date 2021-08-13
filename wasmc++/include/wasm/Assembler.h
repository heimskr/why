#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "parser/Parser.h"

namespace Wasmc {
	class Assembler {
		public:
			Assembler(Parser &);

			void assemble();

		private:
			Parser &parser;
			std::unordered_map<const std::string *, size_t> offsets, dataOffsets;
			std::vector<uint64_t> meta, data, code, symbolTable, debugData, assembled;
			std::unordered_set<const std::string *> unknownSymbols;
			std::unordered_map<const std::string *, const std::string *> dataVariables;

			/** Throws an exception if there exist more than one of any section type in the AST. */
			void validateSectionCounts() const;

			std::vector<const std::string *> findAllLabels() const;
	};

}
