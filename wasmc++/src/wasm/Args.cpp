#include <iostream>

#include "parser/ASTNode.h"
#include "parser/Lexer.h"
#include "wasm/Args.h"
#include "wasm/Registers.h"

namespace Wasmc {
	RegisterArg::RegisterArg(ASTNode *node) {
		reg = registerMap.at(node->lexerInfo);
	}

	RegisterArg::operator std::string() const {
		return *registerArray[reg];
	}

	AddressArg::AddressArg(ASTNode *node): ident(node->lexerInfo) {}

	AddressArg::operator std::string() const {
		return "&" + *ident;
	}

	ValueArg::ValueArg(ASTNode *node): ident(node->lexerInfo) {}

	ValueArg::operator std::string() const {
		return "*" + *ident;
	}

	NumberArg::NumberArg(ASTNode *node): value(node->atoi()) {}

	NumberArg::operator std::string() const {
		return std::to_string(value);
	}

	Args::Args(ASTNode *node) {
		if (node) {
			for (ASTNode *arg: *node) {
				switch (arg->symbol) {
					case WASMTOK_REG:
						args.emplace_back(new RegisterArg(arg));
						break;
					case WASMTOK_AND:
						args.emplace_back(new AddressArg(arg->front()));
						break;
					case WASMTOK_ASTERISK:
						args.emplace_back(new ValueArg(arg->front()));
						break;
					case WASMTOK_NUMBER:
						args.emplace_back(new NumberArg(arg));
						break;
					default:
						arg->debug();
						throw std::runtime_error("Unrecognized argument in function call at "
							+ std::string(node->location));
				}
			}
			delete node;
		}
	}
}
