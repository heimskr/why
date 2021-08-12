#include <cstring>
#include <fstream>
#include <sstream>

#include "parser/Parser.h"
#include "parser/Lexer.h"
#include "parser/StringSet.h"

int wasmwrap() { return 1; }

extern YY_BUFFER_STATE wasm_scan_buffer(char *, size_t);
extern void wasm_delete_buffer(YY_BUFFER_STATE);

namespace Wasmcpp {
	void Parser::open(const std::string &filename_) {
		errorCount = 0;
		filename = filename_;
		wasmin = fopen(filename.c_str(), "r");
	}

	void Parser::in(const std::string &text) {
		errorCount = 0;
		buffer = new char[text.size() + 2];
		std::strncpy(buffer, text.c_str(), text.size() + 1);
		buffer[text.size() + 1] = '\0'; // Input to flex needs two null terminators.
		bufferState = wasm_scan_buffer(buffer, text.size() + 2);
	}

	void Parser::debug(bool flex, bool bison) {
		wasm_flex_debug = flex;
		wasmdebug = bison;
	}

	void Parser::parse() {
		wasmparse();
	}

	void Parser::done() {
		wasmlex_destroy();
		delete root;

		if (buffer) {
			delete buffer;
			buffer = nullptr;
		}

		if (bufferState) {
			// Causes a double free—does wasmparse already do this?
			// wasm_delete_buffer(bufferState);
			// bufferState = nullptr;
		}
	}

	const char * Parser::getName(int symbol) {
		return getNameWASM(symbol);
	}

	std::string Parser::getBuffer() const {
		return buffer? buffer : "";
	}

	Parser wasmParser;
}
