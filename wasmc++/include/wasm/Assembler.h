#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "parser/Parser.h"

namespace Wasmc {
	using Long = uint64_t;

	enum class SymbolType: unsigned {Unknown, KnownPointer, UnknownPointer, Code, Data};

	class Assembler {
		public:
			Assembler(Parser &);

			void assemble();

		private:
			Parser &parser;
			std::unordered_map<const std::string *, Long> offsets, dataOffsets;
			std::vector<Long> meta, data, code, symbolTable, debugData, assembled;
			std::unordered_set<const std::string *> allLabels, unknownSymbols;
			std::unordered_map<const std::string *, const std::string *> dataVariables;
			std::unordered_map<uint32_t, const std::string *> hashes;

			Long & metaOffsetSymbols() { return meta.at(0); }
			Long & metaOffsetCode()    { return meta.at(1); }
			Long & metaOffsetData()    { return meta.at(2); }
			Long & metaOffsetDebug()   { return meta.at(3); }
			Long & metaOffsetEnd()     { return meta.at(4); }

			ASTNode *metaNode = nullptr, *includeNode = nullptr, *dataNode = nullptr, *debugNode = nullptr,
			        *codeNode = nullptr;

			/** Throws an exception if there exist more than one of any section type in the AST. */
			void validateSectionCounts();

			void findAllLabels();

			std::vector<Long> createSymbolTable(std::unordered_set<const std::string *> labels, bool skeleton);

			uint32_t encodeSymbol(const std::string *);

			std::vector<Long> str2longs(const std::string &);

			void processMetadata();
	};

}
