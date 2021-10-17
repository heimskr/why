#pragma once

#include <unordered_map>
#include <vector>

#include <stdio.h>

#include "ASTNode.h"

#define WASMSTYPE_IS_DECLARED
typedef Wasmc::ASTNode * WASMSTYPE;

#ifndef NO_YYPARSE
#include "wasmparse.h"
#endif

#ifdef __APPLE__
typedef size_t yysize;
#else
typedef int yysize;
#endif

extern FILE *wasmin;
extern char *wasmtext;
extern yysize wasmleng;
extern int wasm_flex_debug;
extern int wasmdebug;

namespace Wasmc {
	class Parser;

	class Lexer {
		private:
			Parser *parser;
			yysize *leng;
			ASTNode **lval;

		public:
			ASTLocation location = {0, 1};
			std::string line;
			yysize lastYylength = 0;
			std::unordered_map<int, std::string> lines;
			bool failed = false;
			std::vector<std::pair<std::string, ASTLocation>> errors;

			Lexer(Parser &, yysize &, ASTNode *&);
			const std::string * filename(int fileno);
			void advance(const char *);
			void newline();
			void badchar(unsigned char);
			int token(const char *, int symbol);
	};

	extern Lexer wasmLexer;
}

int wasmlex();
int wasmlex_destroy();
int wasmparse();
void wasmerror(const std::string &);
void wasmerror(const std::string &, const Wasmc::ASTLocation &);
