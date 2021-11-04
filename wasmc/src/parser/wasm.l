%{
#include "parser/Lexer.h"
#ifdef YY_USER_ACTION
#error "YY_USER_ACTION is already defined"
#endif
#define YY_USER_ACTION { Wasmc::wasmLexer.advance(wasmtext); }
#define WASMRTOKEN(x) return Wasmc::wasmLexer.token(wasmtext, WASMTOK_##x);

// Disable PVS-Studio warnings about branches that do the same thing.
//-V::1037

%}

%option 8bit
%option debug
%option nobackup
%option nodefault
%option noinput
%option nounput
%option noyywrap
%option warn

WASM_LETTER             [A-Za-z_]
WASM_DIGIT              [0-9]
WASM_DECIMAL            ([\-+]?{WASM_DIGIT}+)
WASM_HEXADECIMAL        ([\-+]?0x[a-fA-F0-9]+)
WASM_FLOAT              ([\-+]?((([0-9]*"."[0-9]+|[0-9]+"."[0-9]*)([eE][\-+]?[0-9]+)?)|([0-9]+[eE][\-+]?[0-9]+)))
WASM_CHAR               ('([^'\\\n]|("\\""\\"?[nratb])|\$[()])')
WASM_NUMBER             ({WASM_DECIMAL}|{WASM_HEXADECIMAL})
WASM_IDENT              ([-a-zA-Z._][-a-zA-Z._0-9]*)
WASM_STRING             (\"([^\\"\n]|\\([\\'"nt]|x[0-9a-fA-F][0-9a-fA-F]))*\")
WASM_SGCOMMENT          ("//"[^\n]*)
WASM_MLCOMMENT          ("/*"([^*]|"*"[^/])*"*/")
WASM_REG                ("$"([0g]|[fs]"p"|"r"("t"|[0-9a-f])|"lo"|"hi"|[akm][0-9a-f]|[st]([0-9a-f]|"1"[0-6])|"st"|"f"[0-3]|"e"[0-5]))
WASM_META_HEADER        ("#meta"|"#m")
WASM_INCLUDE_HEADER     ("#includes"|"#include")
WASM_DEBUG_HEADER       ("#debug"|"#dbg")
WASM_TEXT_HEADER        ("#text"|"#t")
WASM_INT_TYPE           ("#i"[0-9]+)

%%

{WASM_SGCOMMENT}			{ }
{WASM_MLCOMMENT}			{ }
[ \t]+						{ }
\n							{ Wasmc::wasmLexer.newline(); WASMRTOKEN(NEWLINE) }

{WASM_REG}					{ WASMRTOKEN(REG) }
{WASM_CHAR}					{ WASMRTOKEN(CHAR) }
{WASM_STRING}				{ WASMRTOKEN(STRING) }
{WASM_FLOAT}				{ WASMRTOKEN(FLOAT) }

{WASM_INT_TYPE}				{ WASMRTOKEN(INT_TYPE) }

{WASM_META_HEADER}			{ WASMRTOKEN(META_HEADER) }
{WASM_INCLUDE_HEADER}		{ WASMRTOKEN(INCLUDE_HEADER) }
{WASM_DEBUG_HEADER}			{ WASMRTOKEN(DEBUG_HEADER) }
{WASM_TEXT_HEADER}			{ WASMRTOKEN(TEXT_HEADER) }

"%type"						{ WASMRTOKEN(DIR_TYPE) }
"%size"						{ WASMRTOKEN(DIR_SIZE) }
"%string"					{ WASMRTOKEN(DIR_STRING) }
"%stringz"					{ WASMRTOKEN(DIR_STRINGZ) }
"%1b"						{ WASMRTOKEN(DIR_1B) }
"%2b"						{ WASMRTOKEN(DIR_2B) }
"%4b"						{ WASMRTOKEN(DIR_4B) }
"%8b"						{ WASMRTOKEN(DIR_8B) }
"%align"					{ WASMRTOKEN(DIR_ALIGN) }
"%fill"						{ WASMRTOKEN(DIR_FILL) }
"function"					{ WASMRTOKEN(FUNCTION) }
"object"					{ WASMRTOKEN(OBJECT) }
"%data"						{ WASMRTOKEN(DATA) }
"%code"						{ WASMRTOKEN(CODE) }

"version"					{ WASMRTOKEN(VERSION) }
"memset"					{ WASMRTOKEN(MEMSET) }
"author"					{ WASMRTOKEN(AUTHOR) }
"orcid"						{ WASMRTOKEN(ORCID) }
"print"						{ WASMRTOKEN(PRINT) }
"sleep"						{ WASMRTOKEN(SLEEP) }
"rest"						{ WASMRTOKEN(REST) }
"%setpt"					{ WASMRTOKEN(SETPT) }
"name"						{ WASMRTOKEN(NAME) }
"%time"						{ WASMRTOKEN(TIME) }
"%ring"						{ WASMRTOKEN(RING) }
"halt"						{ WASMRTOKEN(HALT) }
"%page"						{ WASMRTOKEN(PAGE) }
"lui"						{ WASMRTOKEN(LUI) }
"%int"						{ WASMRTOKEN(INT) }
"%rit"						{ WASMRTOKEN(RIT) }
"%di"						{ WASMRTOKEN(DI) }
"%ei"						{ WASMRTOKEN(EI) }
"!ret"						{ WASMRTOKEN(RET) }
"prc"						{ WASMRTOKEN(PRC) }
"prx"						{ WASMRTOKEN(PRX) }
"prd"						{ WASMRTOKEN(PRD) }
"prb"						{ WASMRTOKEN(PRB) }
"off"						{ WASMRTOKEN(OFF) }
"mem"						{ WASMRTOKEN(MEM) }
">>>"						{ WASMRTOKEN(RL) }
"!&&"						{ WASMRTOKEN(LNAND) }
"!||"						{ WASMRTOKEN(LNOR) }
"!xx"						{ WASMRTOKEN(LXNOR) }
"if"						{ WASMRTOKEN(IF) }
"on"						{ WASMRTOKEN(ON) }
"->"						{ WASMRTOKEN(INTO) }
"&&"						{ WASMRTOKEN(LAND) }
"||"						{ WASMRTOKEN(LOR) }
"xx"						{ WASMRTOKEN(LXOR) }
"~&"						{ WASMRTOKEN(NAND) }
"~|"						{ WASMRTOKEN(NOR) }
"~x"						{ WASMRTOKEN(XNOR) }
"/u"						{ WASMRTOKEN(UNSIGNED) }
"<="						{ WASMRTOKEN(LEQ) }
"=="						{ WASMRTOKEN(DEQ) }
">="						{ WASMRTOKEN(GEQ) }
"/b"						{ WASMRTOKEN(BYTE) }
"/h"						{ WASMRTOKEN(HALF) }
"/s"						{ WASMRTOKEN(SHORT) }
"!="						{ WASMRTOKEN(NOTEQUAL) }
"<>"						{ WASMRTOKEN(NOP) }
"<<"						{ WASMRTOKEN(LL) }
">>"						{ WASMRTOKEN(RA) }
"++"						{ WASMRTOKEN(INC) }
"--"						{ WASMRTOKEN(DEC) }
"io"						{ WASMRTOKEN(IO) }
"#fn"						{ WASMRTOKEN(FUNCTION_TYPE) }

";"							{ WASMRTOKEN(SEMICOLON) }
"/"							{ WASMRTOKEN(SLASH) }
"~"							{ WASMRTOKEN(NOT) }
"&"							{ WASMRTOKEN(AND) }
"|"							{ WASMRTOKEN(OR) }
"!"							{ WASMRTOKEN(BANG) }
"="							{ WASMRTOKEN(EQUALS) }
"."							{ WASMRTOKEN(DOT) }
"{"							{ WASMRTOKEN(LCURLY) }
"}"							{ WASMRTOKEN(RCURLY) }
","							{ WASMRTOKEN(COMMA) }
"%"							{ WASMRTOKEN(PERCENT) }
"@"							{ WASMRTOKEN(AT) }
"("							{ WASMRTOKEN(LPAR) }
")"							{ WASMRTOKEN(RPAR) }
"["							{ WASMRTOKEN(LSQUARE) }
"]"							{ WASMRTOKEN(RSQUARE) }
"x"							{ WASMRTOKEN(X) }
"<"							{ WASMRTOKEN(LANGLE) }
">"							{ WASMRTOKEN(RANGLE) }
"*"							{ WASMRTOKEN(ASTERISK) }
"#"							{ WASMRTOKEN(HASH) }
":"							{ WASMRTOKEN(COLON) }
"+"							{ WASMRTOKEN(PLUS) }
"-"							{ WASMRTOKEN(MINUS) }
"?"							{ WASMRTOKEN(QUESTION) }
"p"							{ WASMRTOKEN(P) }

{WASM_NUMBER}				{ WASMRTOKEN(NUMBER) }
{WASM_IDENT}				{ WASMRTOKEN(IDENT) }
.							{ Wasmc::wasmLexer.badchar(*yytext); }

%%

#undef WASMRTOKEN