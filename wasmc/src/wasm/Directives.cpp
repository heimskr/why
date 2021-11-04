#include "wasm/Directives.h"
#include "parser/ASTNode.h"
#include "parser/Lexer.h"
#include "parser/Parser.h"
#include "parser/StringSet.h"

namespace Wasmc {
	Directive::Directive(int symbol_): ASTNode(wasmParser, symbol_) {}

	TypeDirective::TypeDirective(const std::string *symbol_, Type type_):
		Directive(WASM_TYPEDIR), symbol(symbol_), type(type_) {}

	TypeDirective::TypeDirective(const std::string &symbol_, Type type_):
		TypeDirective(StringSet::intern(symbol_), type_) {}

	TypeDirective::TypeDirective(const ASTNode *symbol_, const ASTNode *type_): Directive(WASM_TYPEDIR) {
		symbol = symbol_->symbol == WASMTOK_STRING? symbol_->extracted() : symbol_->lexerInfo;
		delete symbol_;

		const int type_symbol = type_->symbol;
		delete type_;

		switch (type_symbol) {
			case WASMTOK_FUNCTION: type = Type::Function; break;
			case WASMTOK_OBJECT:   type = Type::Object;   break;
			default: throw std::runtime_error("Invalid TypeDirective type symbol: " +
				std::string(wasmParser.getName(type_symbol)));
		}
	}

	SizeDirective::SizeDirective(const std::string *symbol_, const ASTNode *expression_):
		Directive(WASM_SIZEDIR), symbol(symbol_), expression(expression_) {}

	SizeDirective::SizeDirective(const ASTNode *symbol_, const ASTNode *expression_):
		SizeDirective(symbol_->lexerInfo, expression_) {}

	SizeDirective::~SizeDirective() {
		delete expression;
	}

	StringDirective::StringDirective(const std::string *string_, bool null_terminate):
		Directive(WASM_STRINGDIR), string(string_), nullTerminate(null_terminate) {}

	StringDirective::StringDirective(const ASTNode *string_, bool null_terminate):
	StringDirective(string_->lexerInfo, null_terminate) {
		delete string_;
	}
}
