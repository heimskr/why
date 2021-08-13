%{
// No subroutine support for now.

#include <cassert>
#include <cstdarg>
#include <initializer_list>

#define NO_YYPARSE
#include "parser/Lexer.h"
#include "parser/ASTNode.h"
#include "parser/Parser.h"
#include "wasm/Nodes.h"

// Disable PVS-Studio warnings about branches that do the same thing.
//-V::1037

template <typename ...Args>
void D(Args && ...args) {
	(void) std::initializer_list<int> {
		((void) delete args, 0)...
	};
}

template <typename T>
const Wasmc::ASTLocation & L(std::initializer_list<const T *> nodes) {
	for (const T *node: nodes)
		if (node)
			return node->location;
	throw std::runtime_error("Couldn't find location: all nodes are null");
}

using namespace Wasmc;
using AN = Wasmc::ASTNode;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
%}

%debug
%defines
%define parse.error verbose
%token-table
%verbose
%glr-parser

%define api.prefix {wasm}

%initial-action {
    Wasmc::wasmParser.root = new Wasmc::ASTNode(Wasmc::wasmParser, WASMTOK_ROOT, {0, 0}, "");
}

%token WASMTOK_ROOT WASMTOK_IDENT

%token WASMTOK_BANG "!"
%token WASMTOK_EQUALS "="
%token WASMTOK_DOT "."
%token WASMTOK_LCURLY "{"
%token WASMTOK_RCURLY "}"
%token WASMTOK_COMMA ","
%token WASMTOK_NULL "null"
%token WASMTOK_PERCENT "%"
%token WASMTOK_AT "@"
%token WASMTOK_LPAR "("
%token WASMTOK_RPAR ")"
%token WASMTOK_LSQUARE "["
%token WASMTOK_RSQUARE "]"
%token WASMTOK_X "x"
%token WASMTOK_LANGLE "<"
%token WASMTOK_RANGLE ">"
%token WASMTOK_MEMSET "memset"
%token WASMTOK_LUI "lui"
%token WASMTOK_ASTERISK "*"
%token WASMTOK_HASH "#"
%token WASMTOK_COLON ":"
%token WASMTOK_PLUS "+"
%token WASMTOK_MINUS "-"
%token WASMTOK_INTO "->"
%token WASMTOK_DOLLAR "$"
%token WASMTOK_AND "&"
%token WASMTOK_NAND "~&"
%token WASMTOK_LAND "&&"
%token WASMTOK_LNAND "!&&"
%token WASMTOK_OR "|"
%token WASMTOK_NOR "~|"
%token WASMTOK_LOR "||"
%token WASMTOK_LNOR "!||"
%token WASMTOK_XNOR "~x"
%token WASMTOK_NOT "~"
%token WASMTOK_SLASH "/"
%token WASMTOK_LXNOR "!xx"
%token WASMTOK_LXOR "xx"
%token WASMTOK_SEMICOLON ";"
%token WASMTOK_UNSIGNED "/u"
%token WASMTOK_LEQ "<="
%token WASMTOK_DEQ "=="
%token WASMTOK_GEQ ">="
%token WASMTOK_BYTE "/b"
%token WASMTOK_HALF "/h"
%token WASMTOK_NEWLINE "\n"
%token WASMTOK_NOTEQUAL "!="
%token WASMTOK_IF "if"
%token WASMTOK_NOP "<>"
%token WASMTOK_INT "%int"
%token WASMTOK_RIT "rit"
%token WASMTOK_TIME "%time"
%token WASMTOK_RING "%ring"
%token WASMTOK_LL "<<"
%token WASMTOK_RL ">>>"
%token WASMTOK_RA ">>"
%token WASMTOK_PRC "prc"
%token WASMTOK_PRINT "print"
%token WASMTOK_PRX "prx"
%token WASMTOK_PRD "prd"
%token WASMTOK_PRB "prb"
%token WASMTOK_HALT "halt"
%token WASMTOK_SLEEP "sleep"
%token WASMTOK_ON "on"
%token WASMTOK_OFF "off"
%token WASMTOK_PAGE "%page"
%token WASMTOK_SETPT "%setpt"
%token WASMTOK_SHORT "/s"
%token WASMTOK_QUESTION "?"
%token WASMTOK_MEM "mem"
%token WASMTOK_P "p"
%token WASMTOK_REG
%token WASMTOK_NUMBER
%token WASMTOK_CHAR
%token WASMTOK_STRING
%token WASMTOK_FLOAT
%token WASMTOK_META_HEADER "#meta"
%token WASMTOK_INCLUDE_HEADER "#include"
%token WASMTOK_DATA_HEADER "#data"
%token WASMTOK_DEBUG_HEADER "#debug"
%token WASMTOK_CODE_HEADER "#code"
%token WASMTOK_VERSION "version"
%token WASMTOK_AUTHOR "author"
%token WASMTOK_ORCID "orcid"
%token WASMTOK_NAME "name"
%token WASMTOK_RET "!ret"

%token WASM_RNODE WASM_STATEMENTS WASM_INODE WASM_COPYNODE WASM_LOADNODE WASM_STORENODE WASM_SETNODE WASM_LINODE
%token WASM_SINODE WASM_LNINODE WASM_CHNODE WASM_LHNODE WASM_SHNODE WASM_CMPNODE WASM_CMPINODE WASM_SELNODE WASM_JNODE
%token WASM_JCNODE WASM_JRNODE WASM_JRCNODE WASM_IMMEDIATE WASM_SSNODE WASM_MULTRNODE WASM_MULTINODE WASM_DIVIINODE
%token WASM_LUINODE WASM_STACKNODE WASM_NOPNODE WASM_INTINODE WASM_RITINODE WASM_TIMEINODE WASM_TIMERNODE WASM_RINGINODE
%token WASM_RINGRNODE WASM_PRINTNODE WASM_HALTNODE WASM_SLEEPRNODE WASM_PAGENODE WASM_SETPTINODE WASM_MVNODE WASM_LABEL
%token WASM_SETPTRNODE WASM_SVPGNODE WASM_QUERYNODE WASM_PSEUDOPRINTNODE WASM_INCLUDES WASM_STATEMENT WASM_CALLNODE
%token WASM_ARGS

%start start

%%

start: program;

program: program section { $$ = $1->adopt($2); }
       | { $$ = Wasmc::wasmParser.root; };

section: meta_section | include_section | data_section | debug_section | code_section;

meta_section: "#meta" "\n" { D($2); }
            | meta_section meta_key meta_separator WASMTOK_STRING { $$ = $1->adopt($2->adopt($4)); D($3); }
            | meta_section "\n" { D($2); };
meta_key: "version" | "author" | "orcid" | "name";
meta_separator: ":" | "=";

include_section: "#include" "\n" _includes { $$ = $1->adopt($3); D($2); };
_includes: includes | { $$ = nullptr; };
includes: includes endop string { $$ = $1->adopt($3); D($2); }
        | includes "\n" { D($2); }
        | string { $$ = (new AN(Wasmc::wasmParser, WASM_INCLUDES))->adopt($1, true); };

data_section: "#data" "\n" { D($2); }
            | data_section data_def "\n" { $$ = $1->adopt($2); D($3); }
            | data_section "\n" { D($2); };
data_def: data_key WASMTOK_FLOAT { $$ = $1->adopt($2); }
        | data_key number { $$ = $1->adopt($2); }
        | data_key string { $$ = $1->adopt($2); }
        | data_key "(" number ")" { $$ = $1->adopt($2->adopt($3)); D($4); }
        | data_key "&" ident { $$ = $1->adopt($2->adopt($3)); };
data_key: ident _data_sep { D($2); };
_data_sep: ":" | { $$ = nullptr; };

debug_section: "#debug" "\n" { D($2); }
             | debug_section debug_line "\n" { $$ = $1->adopt($2); D($3); }
             | debug_section "\n" { D($2); };
debug_line: number string { if (*$1->lexerInfo != "1" && *$1->lexerInfo != "2") wasmerror("Invalid debug line type (expected 1 or 2)"); $$ = $1->adopt($2); }
          | number number number number number { if (*$1->lexerInfo != "3") wasmerror("Invalid debug line type (expected 3)"); $$ = $1->adopt({$2, $3, $4, $5}); };

code_section: "#code" "\n" { D($2); };
            | code_section operation { $$ = $1->adopt(new WASMStatementNode($2)); }
            | code_section operation intbang { $$ = $1->adopt(new WASMStatementNode($2, $3)); }
            | code_section label _newlines operation { $$ = $1->adopt(new WASMStatementNode($4, nullptr, $2)); D($3); }
            | code_section label _newlines operation intbang { $$ = $1->adopt(new WASMStatementNode($4, $5, $2)); D($3); }
            | code_section endop { D($2); };
intbang: "!" number { $$ = $1->adopt($2); };

endop: "\n" | ";";
newlines: "\n" | newlines "\n" { $$ = $1->adopt($2); };
_newlines: newlines | { $$ = nullptr; };

operation: op_r    | op_mult  | op_multi | op_lui   | op_i      | op_c     | op_l    | op_s    | op_set   | op_divii
         | op_li   | op_si    | op_ms    | op_lni   | op_ch     | op_lh    | op_sh   | op_cmp  | op_cmpi  | op_sel
         | op_j    | op_jc    | op_jr    | op_jrc   | op_mv     | op_spush | op_spop | op_nop  | op_int   | op_rit
         | op_time | op_timei | op_ext   | op_ringi | op_sspush | op_sspop | op_ring | op_page | op_setpt | op_svpg
         | op_qmem | op_ret   | call;

label: "@" ident { $$ = new WASMLabelNode($2); D($1); };

call: ident "(" args ")" { $$ = new WASMCallNode($1, $3); D($2, $4); }
    | ident "(" ")"      { $$ = new WASMCallNode($1); D($2, $3); };
args: args "," arg { $$ = $1->adopt($3); D($2); }
    | arg { $$ = (new AN(Wasmc::wasmParser, WASM_ARGS))->adopt($1, true); };
arg: "&" ident { $$ = $1->adopt($2); }
   | "*" ident { $$ = $1->adopt($2); }
   | number
   | reg;

op_r: reg basic_oper reg "->" reg _unsigned { $$ = new RNode($1, $2, $3, $5, $6); D($4); }
    | reg shorthandable "=" reg _unsigned   { $$ = new RNode($1, $2, $4, $1, $5); D($3); }
    | "~" reg "->" reg { $$ = new RNode($2, $1, $1, $4, nullptr); D($3); }  // rt will be "~" to indicate this is unary
    | "!" reg "->" reg { $$ = new RNode($2, $1, $1, $4, nullptr); D($3); }; // Same here, but with "!".
basic_oper: shorthandable | "<" | "<=" | "==" | ">" | ">=" | "!";
shorthandable: "+" | "-" | "&" | "|" | "&&" | "||" | "x" | "~x" | "!&&" | "!||" | "~&" | "~|" | "/" | "!xx" | "xx" | "%"
              | "<<" | ">>>" | ">>";
_unsigned: "/u" | { $$ = nullptr; };

op_mult: reg "*" reg _unsigned { $$ = new WASMMultRNode($1, $3, $4); D($2); };

op_multi: reg "*" immediate _unsigned { $$ = new WASMMultINode($1, $3, $4); D($2); };

op_lui: "lui" ":" immediate "->" reg { $$ = new WASMLuiNode($3, $5); D($1, $2, $4); };

op_i: reg basic_oper immediate "->" reg _unsigned { $$ = new INode($1, $2, $3, $5, $6); D($4); }
    | reg shorthandable "=" immediate _unsigned   { $$ = new INode($1, $2, $4, $1, $5); D($3); };

op_c: "[" reg "]" "->" "[" reg "]" _byte { $$ = new WASMCopyNode($2, $6, $8); D($1, $3, $4, $5, $7); };
_byte: "/b" | { $$ = nullptr; };

op_l: "[" reg "]" "->" reg _byte { $$ = new WASMLoadNode($2, $5, $6); D($1, $3, $4); };

op_s: reg "->" "[" reg "]" _byte { $$ = new WASMStoreNode($1, $4, $6); D($2, $3, $5); };

op_set: immediate "->" reg { $$ = new WASMSetNode($1, $3); D($2); };

op_divii: immediate "/" reg "->" reg _unsigned { $$ = new WASMDiviINode($1, $3, $5, $6); D($2, $4); };

op_li: "[" immediate "]" "->" reg _byte { $$ = new WASMLiNode($2, $5, $6); D($1, $3, $4); };

op_si: reg "->" "[" immediate "]" _byte { $$ = new WASMSiNode($1, $4, $6); D($2, $3, $5); };

op_ms: "memset" reg "x" reg "->" reg { $$ = new RNode($2, $1, $4, $6, nullptr); D($5); };

op_lni: "[" immediate "]" "->" "[" reg "]" _byte { $$ = new WASMLniNode($2, $6, $8); D($1, $3, $4, $5, $7); };

op_ch: "[" reg "]" "->" "[" reg "]" "/h" { $$ = new WASMChNode($2, $6); D($1, $3, $4, $5, $7, $8); };

op_lh: "[" reg "]" "->" reg "/h" { $$ = new WASMLhNode($2, $5); D($1, $3, $4, $6); };

op_sh: reg "->" "[" reg "]" "/h" { $$ = new WASMShNode($1, $4); D($2, $3, $5, $6); };

op_cmp: reg "~" reg { $$ = new WASMCmpNode($1, $3); D($2); };

op_cmpi: reg "~" immediate { $$ = new WASMCmpiNode($1, $3); D($2); };

op_sel: "[" reg selop reg "]" "->" reg { $$ = new WASMSelNode($2, $3, $4, $7); D($1, $5, $6); };
selop: "=" | "<" | ">" | "!=";

op_j: _jcond colons immediate { $$ = new WASMJNode($1, $2, $3); };
_jcond: jcond | { $$ = nullptr; };
jcond: zero | "+" | "-" | "*";
colons: ":" ":" { $$ = $1->adopt($2); } | ":";

op_jc: op_j "if" reg { $$ = new WASMJcNode(dynamic_cast<WASMJNode *>($1), $3); D($2); };

op_jr: _jcond colons reg { $$ = new WASMJrNode($1, $2, $3); };

op_jrc: op_jr "if" reg { $$ = new WASMJrcNode(dynamic_cast<WASMJrNode *>($1), $3); D($2); };

op_mv: reg "->" reg { $$ = new WASMMvNode($1, $3); D($2); };

op_spush: "[" reg { $$ = new WASMStackNode($2, true); D($1); };

op_spop: "]" reg { $$ = new WASMStackNode($2, false); D($1); };

op_nop: "<>" { $$ = new WASMNopNode(); D($1); };

op_int: "%int" immediate { $$ = new WASMIntINode($2); D($1); };

op_rit: "%rit" immediate { $$ = new WASMRitINode($2); D($1); };

op_time: "%time" reg { $$ = new WASMTimeRNode($2); D($1); };

op_timei: "%time" immediate { $$ = new WASMTimeINode($2); D($1); };

op_ring: "%ring" reg { $$ = new WASMRingRNode($2); D($1); };

op_ringi: "%ring" immediate { $$ = new WASMRingINode($2); D($1); };

op_sspush: "[" ":" number reg { $$ = new WASMSizedStackNode($3, $4, true);  D($1, $2); };

op_sspop:  "]" ":" number reg { $$ = new WASMSizedStackNode($3, $4, false); D($1, $2); };

op_ext: op_print | op_pprint | op_sleep | op_halt;

op_sleep: "<" "sleep" reg ">" { $$ = new WASMSleepRNode($3); D($1, $2, $4); };

op_print: "<" printop reg ">" { $$ = new WASMPrintNode($3, $2); D($1, $4); };
printop: "print" | "prx" | "prd" | "prc" | "prb";

op_pprint: "<" "prc" character ">" { $$ = new WASMPseudoPrintNode($3); D($1, $2, $4); }
         | "<" "p"   immediate ">" { $$ = new WASMPseudoPrintNode($3); D($1, $2, $4); };

op_halt: "<" "halt" ">" { $$ = new WASMHaltNode(); D($1, $2, $3); };

op_page: "%page" "on"  { $$ = new WASMPageNode(true);  D($1, $2); };
       | "%page" "off" { $$ = new WASMPageNode(false); D($1, $2); };

op_setpt: "%setpt" reg { $$ = new WASMSetptRNode($2); D($1); };

op_svpg: "%page" "->" reg { $$ = new WASMSvpgNode($3); D($1, $2); };

op_qmem: "?" "mem" "->" reg { $$ = new WASMQueryNode(QueryType::Memory, $4); D($1, $2, $3); };

op_ret: "!ret" { $$ = new WASMJrNode(Condition::None, false, "$rt"); D($1); };

immediate: _immediate { $$ = new WASMImmediateNode($1); };
_immediate: number | ident | character;

ident: "memset" | "lui" | "if" | "halt" | "on" | "off" | "sleep"
     | "version" | "author" | "orcid" | "name" | printop | "p" | WASMTOK_IDENT;

zero: number { if (*$1->lexerInfo != "0") { wasmerror("Invalid number in jump condition: " + *$1->lexerInfo); } };

reg: WASMTOK_REG;
number: WASMTOK_NUMBER;
character: WASMTOK_CHAR;
string: WASMTOK_STRING;

%%

#pragma GCC diagnostic pop

const char * Wasmc::Parser::getNameWASM(int symbol) {
    return yytname[YYTRANSLATE(symbol)];
}
