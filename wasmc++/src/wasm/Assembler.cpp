#include "parser/Lexer.h"
#include "wasm/Assembler.h"

namespace Wasmc {
	Assembler::Assembler(Parser &parser_): parser(parser_) {}

	void Assembler::assemble() {
		validateSectionCounts();
	}

	void Assembler::validateSectionCounts() const {
		bool meta_found = false, include_found = false, data_found = false, debug_found = false, code_found = false;
		for (ASTNode *node: *parser.root)
			switch (node->symbol) {
				case WASMTOK_META_HEADER:
					if (meta_found)
						throw std::runtime_error("Multiple meta sections detected");
					meta_found = true;
					break;
				case WASMTOK_INCLUDE_HEADER:
					if (include_found)
						throw std::runtime_error("Multiple include sections detected");
					include_found = true;
					break;
				case WASMTOK_DATA_HEADER:
					if (data_found)
						throw std::runtime_error("Multiple data sections detected");
					data_found = true;
					break;
				case WASMTOK_DEBUG_HEADER:
					if (debug_found)
						throw std::runtime_error("Multiple debug sections detected");
					debug_found = true;
					break;
				case WASMTOK_CODE_HEADER:
					if (code_found)
						throw std::runtime_error("Multiple code sections detected");
					code_found = true;
					break;
				default:
					throw std::runtime_error("Unexpected symbol found at root level: "
						+ std::string(parser.getName(node->symbol)));
			}
	}

	std::vector<const std::string *> Assembler::findAllLabels() const {
		std::vector<const std::string *> out;
		return out;
	}
}
