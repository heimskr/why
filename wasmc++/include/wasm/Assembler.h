#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "parser/Parser.h"
#include "util/Util.h"
#include "wasm/Debug.h"

namespace Wasmc {
	struct WASMInstructionNode;
	struct WASMJeqNode;

	struct RType;
	struct IType;
	struct JType;

	using Long = uint64_t;
	using Statements = std::vector<std::shared_ptr<WASMInstructionNode>>;
	using Strings = std::vector<const std::string *>;

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
			std::vector<std::unique_ptr<DebugEntry>> debugEntries;
			size_t dataLength = 0;
			bool verbose = false;

			Long & metaOffsetSymbols() { return meta.at(0); }
			Long & metaOffsetCode()    { return meta.at(1); }
			Long & metaOffsetData()    { return meta.at(2); }
			Long & metaOffsetDebug()   { return meta.at(3); }
			Long & metaOffsetEnd()     { return meta.at(4); }

			ASTNode *metaNode = nullptr, *includeNode = nullptr, *dataNode = nullptr, *debugNode = nullptr,
			        *codeNode = nullptr;

			Long compileInstruction(const WASMInstructionNode &);

			Long compileR(const WASMInstructionNode &, const RType &);

			Long compileI(const WASMInstructionNode &, const IType &);

			Long compileJ(const WASMInstructionNode &, const JType &);

			void addCode(const WASMInstructionNode &);

			/** Replaces all label references in a given vector of expanded instructions with the corresponding memory
			 *  addresses. Mutates the input vector. */
			Statements & expandLabels(Statements &);

			/** Compiles a vector of expanded code into the main code vector. */
			void processCode(const Statements &);

			/** Replaces variable reference placeholders in the data section with the proper values of the pointers. */
			void reprocessData();

			void setDataOffsets();

			/** Throws an exception if there exist more than one of any section type in the AST. */
			void validateSectionCounts();

			void findAllLabels();

			std::vector<Long> createSymbolTable(std::unordered_set<const std::string *> labels, bool skeleton);

			uint32_t encodeSymbol(const std::string *);

			void processMetadata();

			void processData();

			std::vector<Long> convertDataPieces(const ASTNode *);

			Statements expandCode();

			void addJeq(Statements &, const WASMInstructionNode *);
			void addJeqImmediateRHS(Statements &, const WASMJeqNode *, const std::string *m7);

			void addMove(Statements &, const WASMInstructionNode *);

			void addPseudoPrint(Statements &, const WASMInstructionNode *);

			void addStringPrint(Statements &, const WASMInstructionNode *);

			void addCall(Statements &, const WASMInstructionNode *);

			void addStack(Statements &, const std::vector<int> &regs, const Strings &labels, bool is_push,
			              int bang = -1);

			std::vector<Long> createDebugData(const ASTNode *, const Statements &);

			template <typename T>
			std::vector<Long> getLongs(const T &str) const {
				if (str.empty())
					return {0};

				std::vector<Long> out;
				out.reserve(Util::updiv(str.size(), 8ul));

				uint8_t count = 0;
				Long next = 0;
				for (char ch: str) {
					next = (next << 8) | ch;
					if (++count == 8) {
						out.push_back(next);
						next = 0;
						count = 0;
					}
				}

				if (count != 0)
					out.push_back(next << (8 * (8 - count)));

				return out;
			}
	};

}
