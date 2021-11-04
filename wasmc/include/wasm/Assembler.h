#pragma once

#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "parser/Parser.h"
#include "util/Util.h"
#include "wasm/Debug.h"
#include "wasm/Expression.h"
#include "wasm/Section.h"
#include "wasm/Types.h"

namespace Wasmc {
	class ASTNode;
	struct WASMInstructionNode;
	struct WASMJeqNode;

	struct RType;
	struct IType;
	struct JType;

	using Statements = std::vector<std::shared_ptr<WASMInstructionNode>>;
	using Strings = std::vector<const std::string *>;

	class Assembler {
		friend class Linker;

		public:
			/** Takes ownership of the ASTNode argument. It's recommended to call srand() beforehand. */
			Assembler(const ASTNode *);

			~Assembler() { delete root; }

			/** Assembles the program and returns the compiled .why data. */
			std::string assemble(bool can_warn = true);

			static Long compileR(Opcode, uint8_t rs, uint8_t rt, uint8_t rd, Funct function, uint8_t flags,
			                     uint8_t condition);

			static Long compileI(Opcode, uint8_t rs, uint8_t rd, uint32_t immediate, uint8_t flags, uint8_t condition);

			static Long compileJ(Opcode, uint8_t rs, uint32_t address, bool link, uint8_t flags, uint8_t condition);

		private:
			static size_t assemblerCount;

			const ASTNode *root;
			std::unordered_map<const std::string *, Long> offsets, dataOffsets;
			// std::vector<Long> meta, text, symbolTable, debugData, assembled;
			std::vector<Long> symbolTable;
			std::unordered_set<const std::string *> allLabels, unknownSymbols;
			std::map<uint32_t, const std::string *> hashes;
			/** Maps labels to types (unknown, function, object). */
			std::map<const std::string *, SymbolType> symbolTypes;
			std::map<const std::string *, std::shared_ptr<Expression>> symbolSizes;
			/** Maps code section counters to instruction nodes. */
			std::map<size_t, WASMInstructionNode *> instructionMap;
			std::vector<std::unique_ptr<DebugEntry>> debugEntries;
			std::vector<uint8_t> concatenated;

			bool verbose = false;

			Section meta {"Meta", &allLabels, 40}, code {"Code", &allLabels}, data {"Data", &allLabels};
			Section symbols {"Symbols", &allLabels};

			Section *currentSection = &code;

			Long & metaOffsetCode()    { return *(Long *) &meta[ 0]; }
			Long & metaOffsetData()    { return *(Long *) &meta[ 8]; }
			Long & metaOffsetSymbols() { return *(Long *) &meta[16]; }
			Long & metaOffsetDebug()   { return *(Long *) &meta[24]; }
			Long & metaOffsetEnd()     { return *(Long *) &meta[32]; }

			const ASTNode *metaNode = nullptr, *includeNode = nullptr, *debugNode = nullptr, *textNode = nullptr;

			static std::string stringify(const std::vector<Long> &);

			Long compileInstruction(const WASMInstructionNode &);

			Long compileR(const WASMInstructionNode &, const RType &) const;

			Long compileI(const WASMInstructionNode &, const IType &) const;

			Long compileJ(const WASMInstructionNode &, const JType &) const;

			void addCode(const WASMInstructionNode &);

			/** Replaces all label references in a given vector of expanded instructions with the corresponding memory
			 *  addresses. Mutates the input vector. */
			Statements & expandLabels(Statements &);

			void processText();

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

			static uint32_t encodeSymbol(const std::string &);

			/** Clears the output bytes, resets the counter to 0, adds the metadata section and sets the symbol table
			 *  offset. */
			void processMetadata();

			Statements expandText();

			WASMInstructionNode * flipSigns(WASMInstructionNode *) const;

			void addJeq(Statements &, const WASMInstructionNode *);
			void addJeqImmediateRHS(Statements &, const WASMJeqNode *, const std::string *m7);

			void addMove(Statements &, const WASMInstructionNode *);

			void addPseudoPrint(Statements &, const WASMInstructionNode *);

			void addIO(Statements &, const WASMInstructionNode *);

			void addStringPrint(Statements &, const WASMInstructionNode *);

			void addCall(Statements &, const WASMInstructionNode *);

			void addStack(Statements &, const std::vector<int> &regs, const Strings &labels, bool is_push,
			              int bang = -1);

			std::vector<Long> createDebugData(const ASTNode *, const Statements &);
	};

}
