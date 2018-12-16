@{%
"use strict";

const S = Symbol.for;

const {EXTS} = require("./constants.js");

const special = {
	chars: "@$&*\t \":`./",
	words: "+ - / * ^ -> < > <= >= = == [ ] :".split(" ")
};

const filter = (arr, index=null, ...remove) => arr.filter((item) => !(remove.length? remove : [null]).includes(typeof index == "number" && 0 <= index? item[index] : item));
const select = (arr, ...indices) => indices.map((i) => arr[i]);

const compileObject = (entries) => {
	let obj = {};
	filter(entries).forEach((entry) => obj[entry[0]] = entry[1]);
	return obj;
};

const compileData = (entries) => {
	let obj = {};
	filter(entries).forEach((entry) => obj[entry[1]] = select(entry, 0, 2));
	return obj;
};

const compileSubroutine = (name, args, code) => {
	return [
		[name, "push", ...args],
		...code.map((item) => item[1] == S("done")? [item[0], "j", 0, ["label", `${name}$done`], false, null] : item),
		[`${name}$done`, "pop", ...args.reverse()],
		[`${name}$end`, "jr", 0, 0, ["register", "return", 0]]
	];
};

const compileCode = (statements) => {
	const out = [];
	statements.forEach((statement) => {
		if (statement != null && statement[0] == "subroutine") {
			statement[1].forEach((substatement) => out.push(substatement));
		} else if (statement != null) {
			out.push(statement);
		};
	});

	return out;
};

const subify = (x) => Object.assign(x, {inSubroutine: true});

%}

@builtin "postprocessors.ne"

main			-> program									{% d => compileObject(d[0]) %}
program			-> lineend:* section:*						{% d => filter(d[1], 0, null, "\n") %}

lineend			-> (single newline | multi | newline) 		{% d => null %}
sep				-> ";"										{% d => null %}
				 | lineend									{% d => null %}

par[OPER]		-> "(" _ $OPER _ ")"						{% d => d[2][0] %}
brc[OPER]		-> "{" _ $OPER _ "}"						{% d => d[2][0] %}
epar			-> "(" _ ")"								{% d => null %}

string			-> dqstring									{% nth(0) %}
int_hex			-> "-":? "0x" [0-9a-fA-F]:+					{% d => parseInt((d[0] || "") + d[2].join(""), 16) %}
int_bin			-> "-":? "0b" [01]:+						{% d => parseInt((d[0] || "") + d[2].join(""), 2 ) %}
int_dec			-> "-":? [0-9]:+							{% d => parseInt((d[0] || "") + d[1].join("")    ) %}
char			-> "'" sstrchar "'"							{% d => d[1].charCodeAt(0) %}
int				-> (int_hex | int_bin | int_dec | char)		{% d => d[0][0] %}
float			-> "-":? [0-9]:+ "." [0-9]:*				{% d => parseFloat((d[0] || "") + d[1].join("") + d[2] + d[3].join("")) %}
				 | ("-":? ".") [0-9]:+						{% d => parseFloat(filter(d[0]).join("") + d[1].join("")) %}

section			-> (data_section | code_section | meta_section | handlers_section | include_section)
															{% d => d[0][0] %}

meta_section	-> _ meta_header _ sep meta:*				{% d => ["meta", compileObject(d[4])] %}
meta_header		-> "#meta" | "#m"							{% d => null %}
meta			-> _ metakey _ [:=] _ string				{% d => [d[1][0], d[5]] %}
				 | _ "library"								{% d => ["library", true] %}
				 | _ sep									{% d => null %}
metakey			-> ("orcid" | "version" | "author" | "name"){% nth(0) %}

handlers_section-> _ handlers_header _ sep handler:*		{% d => ["handlers", compileObject(d[4])] %}
handlers_header	-> "#handlers" | "#h"						{% d => null %}
handler			-> _ var (_ ":" _ | __) var_addr _ sep		{% d => [d[1], d[3]] %}
				 | _ var (_ ":" _ | __) int _ sep			{% d => [d[1], d[3]] %}
				 | _ sep									{% d => null %}

data_section	-> _ data_header _ sep datadef:*			{% d => ["data", compileData(d[4])] %}
data_header		-> "#data" | "#d"							{% d => null %}
datakey			-> _ var (_ ":" _ | __)						{% d => d[1] %}
datadef			-> datakey float  _ sep						{% d => ["float",  d[0], d[1]] %}
				 | datakey int    _ sep						{% d => ["int",    d[0], d[1]] %}
				 | datakey string _ sep						{% d => ["string", d[0], d[1]] %}
				 | datakey par[int] _ sep					{% d => ["bytes",  d[0], d[1]] %}
				 | datakey "&" xvar							{% d => ["var",    d[0], d[2]] %}
				 | _ sep 									{% d => null %}

code_section	-> _ code_header _ sep statement:*			{% d => ["code", compileCode(d[4])] %}
code_header		-> "#code" | "#c"							{% d => null %}
statement		-> _ op _ sep								{% d => [null, ...d[1][0]] %}
				 | _ label (_ lineend):* _ op _ sep			{% d => [d[1], ...d[4][0]] %}
				 | _ sep									{% d => null %}
				 | _ subroutine								{% d => ["subroutine", d[1]] %}

subroutine		-> "sub" __ var _ par[sub_saved] _ brc[subroutine_code:*]
															{% d => compileSubroutine(d[2], d[4], filter(d[6])) %}
				 | "sub" __ var _ epar _ brc[subroutine_code:*]
															{% d => compileSubroutine(d[2],   [], filter(d[6])) %}
subroutine_code -> _ op										{% d => subify([null, ...d[1][0]]) %}
				 | _ label (_ lineend):* _ op				{% d => subify([d[1], ...d[4][0]]) %}
				 | _ sep									{% d =>  null %}
				 | ___ "!done"								{% d => [null, S("done")] %}
				 | _ label (lineend | __):+ "!done"			{% d => [d[1], S("done")] %}
sub_saved		-> reg (_ "," _ reg):*						{% d => [d[0], ...d[1].map((x) => x[3])] %}

include_section	-> _ include_header _ sep inclusion:*		{% d => ["includes", filter(d[4])] %}
include_header	-> ("#include" | "#includes" | "#i")		{% d => null %}
inclusion		-> .:+ "\n"									{% (d, l, r) => { const t = d[0].join("").trim(); return ["", "#data"].includes(t)? r : t } %}
				 | _ "\n" 									{% d => null %}

label			-> "@" var									{% nth(1) %}
xvar			-> (var | ".end")							{% d => d[0][0] %}
ptr_ref			-> "&" xvar									{% nth(1) %}
var_addr		-> ptr_ref									{% d => ["address", d[0]] %}

# Matches either a register or a variable dereference (*var) and returns ["register", ...] or ["label", ...] respectively.
rv				-> reg										{% nth(0) %}
				 | "*" var									{% d => ["label", d[1]] %}

# Represents an unordered pair of registers and/or variable dereferences with an operator between them.
pair[OPER]		-> reg _ $OPER _ reg						{% d => [d[0], d[4]] %}
				 | reg _ $OPER _ "*" var					{% d => [d[0], ["label", d[5]]] %}
				 | "*" var _ $OPER _ reg					{% d => [["label", d[1]], d[5]] %}
				 | "*" var _ $OPER _ "*" var				{% d => [["label", d[1]], ["label", d[6]]] %}

riap[OPER]		-> pair[$OPER]								{% d => d[0].reverse() %}

op				-> call | op_add | op_sub | op_mult | op_addi | op_subi | op_multi
				 | op_and | op_nand | op_nor | op_not | op_or | op_xnor | op_xor
				 | op_land | op_lnand | op_lnor | op_lnot | op_lor | op_lxnor | op_lxor
				 | op_andi | op_nandi | op_nori | op_ori | op_xnori | op_xori
				 | op_multu | op_multui | op_mod | op_modi
				 | op_sl | op_sle | op_seq | op_sge | op_sg | op_sli | op_slei | op_seqi | op_sgei | op_sgi
				 | op_slu | op_sleu | op_sgeu | op_sgu | op_slui | op_sleui | op_sgeui | op_sgui
				 | op_lui | op_cb | op_lb | op_sb | op_c | op_l | op_s | op_lbi | op_sbi | op_li | op_si | op_set
				 | op_lni | op_lbni
				 | op_j | op_jc | op_jr | op_jrc | op_jrl | op_jrlc
				 | op_mv | op_ret | op_push | op_pop | op_jeq | op_nop | op_int | op_rit | op_time | op_timei | op_ring
				 | op_ringi | op_sll | op_srl | op_sra | op_slli | op_srli | op_srai
				 | gap | ext_prc | ext_printr | ext_halt | ext_n | ext_eval | ext_prd | ext_prx | ext_prs | ext_pr
				 | ext_sleep | ext_xn_init | ext_xn_connect | ext_xn_send | ext_xn_recv

into			-> _ "->" _									{% d => null %}

# R-Type instructions										                   rt    rs    rd
op_add			-> riap["+"]  into reg						{% d => ["add",      ...d[0], d[2]] %}
				 | rv _ "+="  _ rv							{% d => ["add",   d[4], d[0], d[0]] %}
op_sub			-> riap["-"]  into reg						{% d => ["sub",      ...d[0], d[2]] %}
				 | rv _ "-="  _ rv							{% d => ["sub",   d[4], d[0], d[0]] %}
op_mod			-> riap["%"]  into reg						{% d => ["mod",      ...d[0], d[2]] %}
				 | rv _ "%="  _ rv							{% d => ["mod",   d[4], d[0], d[0]] %}
op_and			-> riap["&"]  into reg						{% d => ["and",      ...d[0], d[2]] %}
				 | rv _ "&="  _ rv							{% d => ["and",   d[4], d[0], d[0]] %}
op_or			-> riap["|"]  into reg						{% d => ["or",       ...d[0], d[2]] %}
				 | rv _ "|="  _ rv							{% d => ["or",    d[4], d[0], d[0]] %}
op_xor			-> riap["x"]  into reg						{% d => ["xor",      ...d[0], d[2]] %}
				 | rv _ "x="  _ rv							{% d => ["xor",   d[4], d[0], d[0]] %}
op_nand			-> riap["~&"] into reg						{% d => ["nand",     ...d[0], d[2]] %}
				 | rv _ "~&=" _ rv							{% d => ["nand",  d[4], d[0], d[0]] %}
op_nor			-> riap["~|"] into reg						{% d => ["nor",      ...d[0], d[2]] %}
				 | rv _ "~|=" _ rv							{% d => ["nor",   d[4], d[0], d[0]] %}
op_xnor			-> riap["~x"] into reg						{% d => ["xnor",     ...d[0], d[2]] %}
				 | rv _ "~x=" _ rv							{% d => ["xnor",  d[4], d[0], d[0]] %}

op_land			-> riap["&&"]  into reg						{% d => ["land",     ...d[0], d[2]] %}
				 | rv _ "&&="  _ rv							{% d => ["land",  d[4], d[0], d[0]] %}
op_lor			-> riap["||"]  into reg						{% d => ["lor",      ...d[0], d[2]] %}
				 | rv _ "||="  _ rv							{% d => ["lor",   d[4], d[0], d[0]] %}
op_lxor			-> riap["xx"]  into reg						{% d => ["lxor",     ...d[0], d[2]] %}
				 | rv _ "xx="  _ rv							{% d => ["lxor",  d[4], d[0], d[0]] %}
op_lnand		-> riap["!&&"] into reg						{% d => ["lnand",    ...d[0], d[2]] %}
				 | rv _ "!&&=" _ rv							{% d => ["lnand", d[4], d[0], d[0]] %}
op_lnor			-> riap["!||"] into reg						{% d => ["lnor",     ...d[0], d[2]] %}
				 | rv _ "!||=" _ rv							{% d => ["lnor",  d[4], d[0], d[0]] %}
op_lxnor		-> riap["!xx"] into reg						{% d => ["lxnor",    ...d[0], d[2]] %}
				 | rv _ "!xx=" _ rv							{% d => ["lxnor", d[4], d[0], d[0]] %}
op_lnot			-> "!" _ rv into rv							{% d => ["lnot",    0,  d[2], d[4]] %}
				 | "!" _ rv _ "."							{% d => ["lnot",    0,  d[2], d[2]] %}

op_multu		-> riap["*"] "/u"							{% d => ["multu",    ...d[0],   0 ] %}
op_mult			-> riap["*"]								{% d => ["mult",     ...d[0],   0 ] %}
op_not			-> "~" _ rv into rv							{% d => ["not",     0,  d[2], d[4]] %}
				 | "~" _ rv _ "."							{% d => ["not",     0,  d[2], d[2]] %}
op_slu			-> rv _ "<"  _ rv into rv _ "/u"			{% d => ["slu",   d[0], d[4], d[6]] %}
op_sleu			-> rv _ "<=" _ rv into rv _ "/u"			{% d => ["sleu",  d[0], d[4], d[6]] %}
op_sgeu			-> rv _ ">"  _ rv into rv _ "/u"			{% d => ["sleu",  d[4], d[0], d[6]] %}
op_sgu			-> rv _ ">=" _ rv into rv _ "/u"			{% d => ["slu",   d[4], d[0], d[6]] %}
op_sl			-> rv _ "<"  _ rv into rv					{% d => ["sl",    d[4], d[0], d[6]] %}
op_sle			-> rv _ "<=" _ rv into rv					{% d => ["sle",   d[4], d[0], d[6]] %}
op_seq			-> rv _ "==" _ rv into rv					{% d => ["seq",   d[0], d[4], d[6]] %}
op_sge			-> rv _ ">=" _ rv into rv					{% d => ["sle",   d[0], d[4], d[6]] %}
op_sg			-> rv _ ">"  _ rv into rv					{% d => ["sl",    d[0], d[4], d[6]] %}
op_jr			-> ":"  _ reg								{% d => ["jr",      0,    0,  d[2], null] %}
				 | "+:" _ reg								{% d => ["jr",      0,    0,  d[2],  "p"] %}
				 | "-:" _ reg								{% d => ["jr",      0,    0,  d[2],  "n"] %}
				 | "0:" _ reg								{% d => ["jr",      0,    0,  d[2],  "z"] %}
				 | "*:" _ reg								{% d => ["jr",      0,    0,  d[2], "nz"] %}
op_jrl			-> "::"  _ reg								{% d => ["jrl",     0,    0,  d[2], null] %}
				 | "+::" _ reg								{% d => ["jrl",     0,    0,  d[2],  "p"] %}
				 | "-::" _ reg								{% d => ["jrl",     0,    0,  d[2],  "n"] %}
				 | "0::" _ reg								{% d => ["jrl",     0,    0,  d[2],  "z"] %}
				 | "*::" _ reg								{% d => ["jrl",     0,    0,  d[2], "nz"] %}
op_jrc			-> ":" _ reg __ "if" __ reg					{% d => ["jrc",     0,  d[6], d[2]] %}
op_jrlc			-> "::" _ reg __ "if" __ reg				{% d => ["jrlc",    0,  d[6], d[2]] %}
op_cb			-> "[" _ rv _ "]" into "[" _ rv _ "]" _ "/b"{% d => ["cb",      0,  d[2], d[8]] %}
op_lb			-> "[" _ reg _ "]" into rv _ "/b"			{% d => ["lb",      0,  d[2], d[6]] %}
op_sb			-> rv into "[" _ rv _ "]" _ "/b"			{% d => ["sb",      0,  d[0], d[4]] %}
op_c			-> "[" _ rv _ "]" into "[" _ rv _ "]"		{% d => ["c",       0,  d[2], d[8]] %}
op_l			-> "[" _ reg _ "]" into rv					{% d => ["l",       0,  d[2], d[6]] %}
op_s			-> rv into "[" _ rv _ "]"					{% d => ["s",       0,  d[0], d[4]] %}
op_sll			-> riap["<<"]  into rv						{% d => ["sll",      ...d[0], d[2]] %}
				 | rv _ "<<="  _ rv							{% d => ["sll",   d[4], d[0], d[0]] %}
op_srl			-> riap[">>>"] into rv						{% d => ["srl",      ...d[0], d[2]] %}
				 | rv _ ">>>=" _ rv							{% d => ["srl",   d[4], d[0], d[0]] %}
op_sra			-> riap[">>"]  into rv						{% d => ["sra",      ...d[0], d[2]] %}
				 | rv _ ">>="  _ rv							{% d => ["sra",   d[4], d[0], d[0]] %}
op_time			-> "time" __ rv								{% d => ["time",    0,  d[2],   0 ] %}
op_ring			-> "ring" __ rv								{% d => ["ring",    0,  d[2],   0 ] %}

# I-Type instructions										                   rs    rd    imm
op_multui		-> reg _ "*"   _ int _ "/u"					{% d => ["multui", d[0],   0,  d[4]] %}
op_addi			-> reg _ "+"   _ int into reg				{% d => ["addi",   d[0], d[6], d[4]] %}
				 | reg _ "++"								{% d => ["addi",   d[0], d[0],   1 ] %}
				 |     _ "++"  _ reg						{% d => ["addi",   d[3], d[3],   1 ] %}
				 | reg _ "+="  _ int						{% d => ["addi",   d[0], d[0], d[4]] %}
op_subi			-> reg _ "-"   _ int into reg				{% d => ["subi",   d[0], d[6], d[4]] %}
				 | reg _ "--"								{% d => ["subi",   d[0], d[0],   1 ] %}
				 |     _ "--"  _ reg						{% d => ["subi",   d[3], d[3],   1 ] %}
				 | reg _ "-="  _ int						{% d => ["subi",   d[0], d[0], d[4]] %}
op_modi			-> reg _ "%"   _ int into reg				{% d => ["modi",   d[0], d[6], d[4]] %}
				 | reg _ "%="  _ int						{% d => ["modi",   d[0], d[0], d[4]] %}
op_multi		-> reg _ "*"   _ int						{% d => ["multi",  d[0],   0,  d[4]] %}
op_andi			-> reg _ "&"   _ int into reg				{% d => ["andi",   d[0], d[6], d[4]] %}
				 | reg _ "&="  _ int						{% d => ["andi",   d[0], d[0], d[4]] %}
op_ori			-> reg _ "|"   _ int into reg				{% d => ["ori",    d[0], d[6], d[4]] %}
				 | reg _ "|="  _ int						{% d => ["ori",    d[0], d[0], d[4]] %}
op_xori			-> reg _ "x"   _ int into reg				{% d => ["xori",   d[0], d[6], d[4]] %}
				 | reg _ "x="  _ int						{% d => ["xori",   d[0], d[0], d[4]] %}
op_nandi		-> reg _ "~&"  _ int into reg				{% d => ["nandi",  d[0], d[6], d[4]] %}
				 | reg _ "~&=" _ int						{% d => ["nandi",  d[0], d[0], d[4]] %}
op_nori			-> reg _ "~|"  _ int into reg				{% d => ["nori",   d[0], d[6], d[4]] %}
				 | reg _ "~|=" _ int						{% d => ["nori",   d[0], d[0], d[4]] %}
op_xnori		-> reg _ "~x"  _ int into reg				{% d => ["xnori",  d[0], d[6], d[4]] %}
				 | reg _ "~x=" _ int						{% d => ["xnori",  d[0], d[0], d[4]] %}
op_slui			-> rv _ "<"    _ int into rv _ "/u"			{% d => ["slui",   d[0], d[6], d[4]] %}
op_sleui		-> rv _ "<="   _ int into rv _ "/u"			{% d => ["sleui",  d[0], d[6], d[4]] %}
op_sgeui		-> rv _ ">"    _ int into rv _ "/u"			{% d => ["sgeui",  d[6], d[0], d[4]] %}
op_sgui			-> rv _ ">="   _ int into rv _ "/u"			{% d => ["sgui",   d[6], d[0], d[4]] %}
op_sli			-> rv _ "<"    _ int into rv				{% d => ["sli",    d[0], d[6], d[4]] %}
op_slei			-> rv _ "<="   _ int into rv				{% d => ["slei",   d[0], d[6], d[4]] %}
op_seqi			-> rv _ "=="   _ int into rv				{% d => ["seqi",   d[0], d[6], d[4]] %}
op_sgei			-> rv _ ">="   _ int into rv				{% d => ["sgei",   d[0], d[6], d[4]] %}
op_sgi			-> rv _ ">"    _ int into rv				{% d => ["sgi",    d[0], d[6], d[4]] %}
op_slli			-> rv _ "<<"   _ int into rv				{% d => ["slli",   d[0], d[6], d[4]] %}
				 | rv _ "<<="  _ int						{% d => ["slli",   d[0], d[0], d[4]] %}
op_srli			-> rv _ ">>>"  _ int into rv				{% d => ["srli",   d[0], d[6], d[4]] %}
				 | rv _ ">>>=" _ int						{% d => ["srli",   d[0], d[0], d[4]] %}
op_srai			-> rv _ ">>"   _ int into rv				{% d => ["srai",   d[0], d[6], d[4]] %}
				 | rv _ ">>="  _ int						{% d => ["srai",   d[0], d[0], d[4]] %}
op_lui			-> "lui" _ ":" _ int into reg				{% d => ["lui",      0,  d[6], d[4]] %}
op_lbi			-> "[" _ int  _ "]" into rv _ "/b"			{% d => ["lbi",      0,  d[6], d[2]] %}
				 | "[" _ xvar _ "]" into rv _ "/b"			{% d => ["lbi",      0,  d[6], ["label", d[2]]] %}
				 | "*" var into rv _ "/b"					{% d => ["lbi",      0,  d[3], ["label", d[1]]] %}
op_sbi			-> rv into "[" _ int _ "]" _ "/b"			{% d => ["sbi",    d[0],   0,  d[4]] %}
				 | rv into "[" _ xvar _ "]" _ "/b"			{% d => ["sbi",    d[0],   0,  ["label", d[4]]] %}
op_lbni			-> "[" _ int  _ "]" into rv _ "/b"			{% d => ["lbni",     0,  d[6], d[2]] %}
				 | "[" _ xvar _ "]" into "[" _ rv _ "]"
				 	_ "/b"									{% d => ["lbni",     0,  d[8], ["label", d[2]]] %}
op_li			-> "[" _ int  _ "]" into rv					{% d => ["li",       0,  d[6], d[2]] %}
				 | "[" _ xvar _ "]" into rv					{% d => ["li",       0,  d[6], ["label", d[2]]] %}
				 | "*" _ xvar into rv						{% d => ["li",       0,  d[4], ["label", d[2]]] %}
op_si			-> rv into "[" _ int  _ "]"					{% d => ["si",     d[0],   0,  d[4]] %}
				 | rv into "[" _ xvar _ "]"					{% d => ["si",     d[0],   0,  ["label", d[4]]] %}
				 | rv into "*" _ xvar						{% d => ["si",     d[0],   0,  ["label", d[4]]] %}
op_lni			-> "[" _ int  _ "]" into "[" _ rv _ "]"		{% d => ["lni",      0,  d[8], d[2]] %}
				 | "[" _ xvar _ "]" into "[" _ rv _ "]"		{% d => ["lni",      0,  d[8], ["label", d[2]]] %}
op_set			-> int into rv								{% d => ["set",      0,  d[2], d[0]] %}
				 | ptr_ref into rv							{% d => ["set",      0,  d[2], ["label", d[0]]] %}
op_int			-> "int" __ int								{% d => ["int",      0,    0,  d[2]] %}
op_rit			-> "rit" __ int								{% d => ["rit",      0,    0,  d[2]] %}
				 | "rit" __ var								{% d => ["rit",      0,    0,  ["label", d[2]]] %}
op_timei		-> "time" __ int							{% d => ["timei",    0,    0,  d[2]] %}
op_ringi		-> "ring" __ int							{% d => ["ringi",    0,    0,  d[2]] %}

# J-Type instructions										               rs     addr             link   condition
op_jc			-> ":" _ int   __ "if" __ reg				{% d => ["jc", d[6],  d[2],            false, null] %}
				 | ":" _ xvar  __ "if" __ reg				{% d => ["jc", d[6],  ["label", d[2]], false, null] %}
				 | "::" _ int  __ "if" __ reg				{% d => ["jc", d[6],  d[2],             true, null] %}
				 | "::" _ xvar __ "if" __ reg				{% d => ["jc", d[6],  ["label", d[2]],  true, null] %}

op_j			-> ":"   _ int								{% d => ["j",    0,   d[2],            false, null] %}
				 | ":"   _ xvar								{% d => ["j",    0,   ["label", d[2]], false, null] %}
				 | "::"  _ int								{% d => ["j",    0,   d[2],             true, null] %}
				 | "::"  _ xvar								{% d => ["j",    0,   ["label", d[2]],  true, null] %}
				 | "+:"  _ int								{% d => ["j",    0,   d[2],            false,  "p"] %}
				 | "+:"  _ xvar								{% d => ["j",    0,   ["label", d[2]], false,  "p"] %}
				 | "+::" _ int								{% d => ["j",    0,   d[2],             true,  "p"] %}
				 | "+::" _ xvar								{% d => ["j",    0,   ["label", d[2]],  true,  "p"] %}
				 | "-:"  _ int								{% d => ["j",    0,   d[2],            false,  "n"] %}
				 | "-:"  _ xvar								{% d => ["j",    0,   ["label", d[2]], false,  "n"] %}
				 | "-::" _ int								{% d => ["j",    0,   d[2],             true,  "n"] %}
				 | "-::" _ xvar								{% d => ["j",    0,   ["label", d[2]],  true,  "n"] %}
				 | "0:"  _ int								{% d => ["j",    0,   d[2],            false,  "z"] %}
				 | "0:"  _ xvar								{% d => ["j",    0,   ["label", d[2]], false,  "z"] %}
				 | "0::" _ int								{% d => ["j",    0,   d[2],             true,  "z"] %}
				 | "0::" _ xvar								{% d => ["j",    0,   ["label", d[2]],  true,  "z"] %}
				 | "*:"  _ int								{% d => ["j",    0,   d[2],            false, "nz"] %}
				 | "*:"  _ xvar								{% d => ["j",    0,   ["label", d[2]], false, "nz"] %}
				 | "*::" _ int								{% d => ["j",    0,   d[2],             true, "nz"] %}
				 | "*::" _ xvar								{% d => ["j",    0,   ["label", d[2]],  true, "nz"] %}

op_mv			-> reg into reg								{% d => ["mv", d[0], d[2]] %}
op_ret			-> "!ret"									{% d => ["jr", 0, 0, ["register", "return", 0]] %}
op_push			-> "[" (_ (reg)):+							{% d => ["push", ...d[1].map(x => x[1][0])] %}
op_pop			-> "]" (_ (reg)):+							{% d => ["pop",  ...d[1].map(x => x[1][0])] %}
op_jeq			-> ":" _ reg  __ "if" __ rv _ "==" _ rv		{% d => ["jeq", d[10], d[6], d[2]] %}
				 | ":" _ xvar __ "if" __ rv _ "==" _ rv		{% d => ["jeq", d[10], d[6], ["label", d[2]]] %}
				 | ":" _ reg  __ "if" __ rv _ "==" _ int	{% d => ["jeq", d[10], d[6], d[2]] %}
				 | ":" _ xvar __ "if" __ rv _ "==" _ int	{% d => ["jeq", d[10], d[6], ["label", d[2]]] %}
op_nop			-> "<>"										{% d => ["nop"] %}

# Externals																	  rt    rs             rd    funct
ext_printr		-> "<" _ "print" _ reg _ ">"				{% d => ["ext",    0,  d[4],            0,     1] %}
ext_halt		-> "<" _ "halt" _ ">"						{% d => ["ext",    0,    0,             0,     2] %}
ext_eval		-> "<" _ "eval" _ reg _ ">"					{% d => ["ext",    0,  d[4],            0,     3] %}
ext_prc			-> "<" _ "prc" _ reg _ ">"					{% d => ["ext",    0,  d[4],            0,     4] %}
				 | "<" _ "prc" _ char _ ">"					{% d => ["ext",    0, ["char", d[4]],   0,     4] %}
ext_prd			-> "<" _ "prd" _ reg _ ">"					{% d => ["ext",    0,  d[4],            0,     5] %}
ext_prx			-> "<" _ "prx" _ reg _ ">"					{% d => ["ext",    0,  d[4],            0,     6] %}
ext_prs			-> "<" _ "prs" _ dqstring _ ">"				{% d => ["ext",    0, ["string", d[4]], 0,     4] %}
ext_pr			-> "<" _ "p" _ reg _ ">"					{% d => ["ext",    0,  d[4],            0,     5] %}
				 | "<" _ "p" _ char _ ">"					{% d => ["ext",    0, ["char", d[4]],   0,     4] %}
				 | "<" _ "p" _ dqstring _ ">"				{% d => ["ext",    0, ["string", d[4]], 0,     4] %}
ext_sleep		-> "<" _ ("sleep" | "wait") _ reg _ ">"		{% d => ["ext",    0,  d[4],            0, EXTS.sleep] %}
ext_n			-> "<" _ int _ ">"							{% d => ["ext",    0,    0,             0, parseInt(d[2])] %}

ext_xn_init		-> "<" _ "xn" __ "init" _ ">"				{% d => ["ext",   0,    0,    0,  EXTS.xn_init   ] %}
ext_xn_connect	-> "<" _ "xn" __ "connect" _ reg _ reg _ ">"{% d => ["ext", d[8], d[6],   0,  EXTS.xn_connect] %}
ext_xn_send		-> "<" _ "xn" __ "send" _ reg _ ">"			{% d => ["ext",   0,  d[6],   0,  EXTS.xn_send   ] %}
ext_xn_recv		-> "<" _ "xn" __ "recv" _ reg _ reg _ ">"	{% d => ["ext",   0,  d[8], d[6], EXTS.xn_recv   ] %}

gap				-> brc[int]									{% d => ["gap",  d[0]] %}

call			-> var _ par[args]							{% d => ["call", d[0], ...d[2].map((x) => x[0])] %}
				 | var _ epar								{% d => ["call", d[0]] %}
arg				-> (rv | int | var_addr)					{% nth(0) %}
args			-> delimited[arg, ("," _)]					{% d => d[0][0] %}

reg_temp		-> "$t" ([0-9a-f] | "1" [0-6])				{% d => ["t", parseInt(d[1].join(""), 16)] %}
reg_saved		-> "$s" ([0-9a-f] | "1" [0-6])				{% d => ["s", parseInt(d[1].join(""), 16)] %}
reg_kern		-> "$k" [0-9a-f]							{% d => ["k", parseInt(d[1], 16)] %}
reg_arg			-> "$a" [0-9a-f]							{% d => ["a", parseInt(d[1], 16)] %}
reg_return		-> "$r" [0-9a-f]							{% d => ["r", parseInt(d[1], 16)] %}
reg_exc			-> "$e" [0-5]								{% d => ["e", parseInt(d[1], 16)] %}
reg_zero		-> "$" [z0]									{% d => ["zero",   0] %}
reg_retaddr		-> "$" ([r<] | "rt")						{% d => ["return", 0] %}
reg_stack		-> "$" ("s" "p":? | "*")					{% d => ["stack",  0] %}
reg_lo			-> "$lo"									{% d => ["lo",     0] %}
reg_hi			-> "$hi"									{% d => ["hi",     0] %}
reg_gp			-> "$g"										{% d => ["g",      0] %}
reg_st			-> "$st"									{% d => ["st",     0] %}
reg_asm			-> "$m" [0-9a-f]							{% d => ["m", parseInt(d[1], 16)] %}
reg				-> (reg_temp | reg_saved | reg_arg | reg_return | reg_zero | reg_retaddr | reg_stack | reg_exc | reg_lo | reg_hi | reg_asm | reg_gp | reg_kern | reg_st)
															{% d => ["register", ...d[0][0]] %}

var -> varchar:+ {%
	(d, location, reject) => {
		const identifier = d[0].join("");
		return /[0-9]/.test(identifier.charAt(0)) || special.words.indexOf(identifier) !== -1? reject : identifier;
	}
%}

___		-> (lineend:+ | __)									{% d => null %}
single	-> "//" [^\n]:*										{% d => null %}
multi	-> "/*" [^*]:* ("*":+ [^*/] [^*]:*):* "*":* "*/"	{% d => null %}
varchar -> . {% (d, location, reject) => d[0] && special.chars.indexOf(d[0]) === -1 ? d[0] : reject %}
newline -> "\r" "\n" | "\r" | "\n"
_		-> [\t ]:* {% d => null %}
__		-> [\t ]:+ {% d => null %}

dqstring	-> "\"" dstrchar:* "\""									{% d => d[1].join("") %}
sqstring	-> "'"  sstrchar:* "'"									{% d => d[1].join("") %}
btstring	-> "`"  [^`]:*     "`"									{% d => d[1].join("") %}

dstrchar	-> [^\\"\n]												{% id %}
			 | "\\0" 												{% d => "\0" %}
			 | "\\A" 												{% d => "\u001b[" %}
			 | "\\" strescape										{% d => JSON.parse(`"${d.join("")}"`) %}

sstrchar	-> [^\\'\n]												{% id %}
			 | "\\" strescape										{% d => JSON.parse(`"${d.join("")}"`) %}
			 | "\\'"												{% d => "'" %}

strescape	-> ["\\/bfnrt]											{% id %}
			 | "u" [a-fA-F0-9] [a-fA-F0-9] [a-fA-F0-9] [a-fA-F0-9]	{% d => d.join("") %}
