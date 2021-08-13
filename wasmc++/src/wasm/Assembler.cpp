#include <iostream>

#include "parser/Lexer.h"
#include "wasm/Assembler.h"
#include "wasm/Nodes.h"

namespace Wasmc {
	Assembler::Assembler(Parser &parser_): parser(parser_) {}

	void Assembler::assemble() {
		validateSectionCounts();
		findAllLabels();
		for (const std::string *label: allLabels)
			std::cout << "[" << *label << "]\n";
	}

	void Assembler::validateSectionCounts() {
		bool meta_found = false, include_found = false, data_found = false, debug_found = false, code_found = false;
		for (ASTNode *node: *parser.root)
			switch (node->symbol) {
				case WASMTOK_META_HEADER:
					if (meta_found)
						throw std::runtime_error("Multiple meta sections detected");
					meta_found = true;
					metaNode = node;
					break;
				case WASMTOK_INCLUDE_HEADER:
					if (include_found)
						throw std::runtime_error("Multiple include sections detected");
					include_found = true;
					includeNode = node;
					break;
				case WASMTOK_DATA_HEADER:
					if (data_found)
						throw std::runtime_error("Multiple data sections detected");
					data_found = true;
					dataNode = node;
					break;
				case WASMTOK_DEBUG_HEADER:
					if (debug_found)
						throw std::runtime_error("Multiple debug sections detected");
					debug_found = true;
					debugNode = node;
					break;
				case WASMTOK_CODE_HEADER:
					if (code_found)
						throw std::runtime_error("Multiple code sections detected");
					code_found = true;
					codeNode = node;
					break;
				default:
					throw std::runtime_error("Unexpected symbol found at root level: "
						+ std::string(parser.getName(node->symbol)));
			}
	}

	void Assembler::findAllLabels() {
		allLabels.clear();

		if (dataNode) {
			for (ASTNode *node: *dataNode) {
				if (node->symbol != WASMTOK_IDENT)
					throw std::runtime_error("Unexpected symbol found in data section at "
						+ std::string(node->location) + ": " + std::string(parser.getName(node->symbol)));
				allLabels.insert(node->lexerInfo);
			}
		}

		if (codeNode) {
			for (ASTNode *node: *codeNode) {
				if (node->symbol != WASM_STATEMENT)
					throw std::runtime_error("Unexpected symbol found in code section at "
						+ std::string(node->location) + ": " + std::string(parser.getName(node->symbol)));
				for (const std::string *label: dynamic_cast<WASMStatementNode *>(node)->labels)
					allLabels.insert(label);
			}
		}
	}
}
