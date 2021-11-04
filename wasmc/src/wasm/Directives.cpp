#include "wasm/Directives.h"
#include "parser/ASTNode.h"
#include "parser/Lexer.h"
#include "parser/Parser.h"
#include "parser/StringSet.h"
#include "wasm/Expression.h"

namespace Wasmc {
	Directive::Directive(int symbol_): ASTNode(wasmParser, symbol_) {}

	TypeDirective::TypeDirective(const std::string *symbol_name, SymbolType type_):
		Directive(WASM_TYPEDIR), symbolName(symbol_name), type(type_) {}

	TypeDirective::TypeDirective(const std::string &symbol_name, SymbolType type_):
		TypeDirective(StringSet::intern(symbol_name), type_) {}

	TypeDirective::TypeDirective(const ASTNode *symbol_name, const ASTNode *type_): Directive(WASM_TYPEDIR) {
		symbolName = symbol_name->symbol == WASMTOK_STRING? symbol_name->extracted() : symbol_name->lexerInfo;
		delete symbol_name;

		const int type_symbol = type_->symbol;
		delete type_;

		switch (type_symbol) {
			case WASMTOK_FUNCTION: type = SymbolType::Function; break;
			case WASMTOK_OBJECT:   type = SymbolType::Object;   break;
			default: throw std::runtime_error("Invalid TypeDirective type symbol: " +
				std::string(wasmParser.getName(type_symbol)));
		}
	}

	SizeDirective::SizeDirective(const std::string *symbol_name, Expression *expression_):
		Directive(WASM_SIZEDIR), symbolName(symbol_name), expression(expression_) {}

	SizeDirective::SizeDirective(const ASTNode *symbol_name, Expression *expression_):
		SizeDirective(symbol_name->lexerInfo, expression_) {}

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

	ValueDirective::ValueDirective(const ASTNode *value_size, Expression *expression_):
	Directive(WASM_VALUEDIR), valueSize(getSize(value_size)), expression(expression_) {
		delete value_size;
	}

	AlignDirective::AlignDirective(size_t alignment_): Directive(WASM_ALIGNDIR), alignment(alignment_) {}

	FillDirective::FillDirective(long count_, long value_): Directive(WASM_FILLDIR), count(count_), value(value_) {}

	DataDirective::DataDirective(): Directive(WASM_DATADIR) {}

	CodeDirective::CodeDirective(): Directive(WASM_CODEDIR) {}
}
