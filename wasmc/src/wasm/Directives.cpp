#include "wasm/Directives.h"
#include "parser/ASTNode.h"
#include "parser/Lexer.h"
#include "parser/Parser.h"
#include "parser/StringSet.h"

namespace Wasmc {
	Directive::Directive(int symbol_): ASTNode(wasmParser, symbol_) {}

	TypeDirective::TypeDirective(const std::string *symbol_, SymbolType type_):
		Directive(WASM_TYPEDIR), symbol(symbol_), type(type_) {}

	TypeDirective::TypeDirective(const std::string &symbol_, SymbolType type_):
		TypeDirective(StringSet::intern(symbol_), type_) {}

	TypeDirective::TypeDirective(const ASTNode *symbol_, const ASTNode *type_): Directive(WASM_TYPEDIR) {
		symbol = symbol_->symbol == WASMTOK_STRING? symbol_->extracted() : symbol_->lexerInfo;
		delete symbol_;

		const int type_symbol = type_->symbol;
		delete type_;

		switch (type_symbol) {
			case WASMTOK_FUNCTION: type = SymbolType::Function; break;
			case WASMTOK_OBJECT:   type = SymbolType::Object;   break;
			default: throw std::runtime_error("Invalid TypeDirective type symbol: " +
				std::string(wasmParser.getName(type_symbol)));
		}
	}

	SizeDirective::SizeDirective(const std::string *symbol_, ASTNode *expression_):
		Directive(WASM_SIZEDIR), symbol(symbol_), expression(expression_) {}

	SizeDirective::SizeDirective(const ASTNode *symbol_, ASTNode *expression_):
		SizeDirective(symbol_->lexerInfo, expression_) {}

	StringDirective::StringDirective(const std::string *string_, bool null_terminate):
		Directive(WASM_STRINGDIR), string(string_), nullTerminate(null_terminate) {}

	StringDirective::StringDirective(const ASTNode *string_, bool null_terminate):
	StringDirective(string_->extracted(), null_terminate) {
		delete string_;
	}

	uint8_t ValueDirective::getSize(const ASTNode *node) {
		switch (node->symbol) {
			case WASMTOK_DIR_8B: return 8;
			case WASMTOK_DIR_4B: return 4;
			case WASMTOK_DIR_2B: return 2;
			case WASMTOK_DIR_1B: return 1;
			default: throw std::invalid_argument("Invalid input to ValueDirective::getSize: " + node->debugExtra());
		}
	}

	ValueDirective::ValueDirective(const ASTNode *size_, ASTNode *expression_):
	Directive(WASM_VALUEDIR), size(getSize(size_)), expression(expression_) {
		delete size_;
	}

	AlignDirective::AlignDirective(long alignment_): Directive(WASM_ALIGNDIR), alignment(alignment_) {}

	FillDirective::FillDirective(long count_, long value_): Directive(WASM_FILLDIR), count(count_), value(value_) {}
}
