@{%
"use strict";

const special = {
	chars: "$&*\t \":",
	words: "+ - / * ^ -> < > <= >= = == [ ] :".split(" ")
};

const flatten = (arr) => arr.filter((e) => e != null).reduce((flat, toFlatten) => flat.filter((e) => e != null).concat(Array.isArray(toFlatten) ? flatten(toFlatten) : toFlatten), []);
const filter = (arr, index=null, ...remove) => arr.filter((item) => !(remove.length? remove : [null]).includes(typeof index == "number" && 0 <= index? item[index] : item));
const select = (arr, ...indices) => indices.map((i) => arr[i]);

const compileObject = (entries) => {
	let obj = { };
	filter(entries).forEach((entry) => obj[entry[0]] = entry[1]);
	return obj;
};

const compileData = (entries) => {
	let obj = { };
	filter(entries).forEach((entry) => obj[entry[1]] = select(entry, 0, 2));
	return obj;
};
%}

@builtin "string.ne"

main			-> program									{% d => compileObject(d[0]) %}
program			-> newline:* section:*						{% d => filter(d[1], 0, null, "\n") %}

lineend			-> (single newline | multi | newline) 		{% d => null %}
sep				-> ";"										{% d => null %}
				 | lineend									{% d => null %}

string			-> dqstring									{% d => d[0] %}
int_hex			-> "-":? "0x" [0-9a-fA-F]:+					{% d => parseInt((d[0] || "") + d[2].join(""), 16) %}
int_bin			-> "-":? "0b" [01]:+						{% d => parseInt((d[0] || "") + d[2].join(""), 2 ) %}
int_dec			-> "-":? [0-9]:+							{% d => parseInt((d[0] || "") + d[1].join("")    ) %}
int				-> (int_hex | int_bin | int_dec)			{% d => d[0][0] %}
float			-> "-":? [0-9]:+ "." [0-9]:*				{% d => parseFloat((d[0] || "") + d[1].join("") + d[2] + d[3].join("")) %}
				 | ("-":? ".") [0-9]:+						{% d => parseFloat(filter(d[0]).join("") + d[1].join("")) %}

section			-> (data_section | code_section | meta_section | handlers_section)
															{% d => d[0][0] %}

meta_section	-> _ meta_header _ sep meta:*				{% d => ["meta", compileObject(d[4])] %}
meta_header		-> "#meta" | "#m"							{% d => null %}
meta			-> _ metakey _ [:=] _ string				{% d => [d[1][0], d[5]] %}
				 | _ sep									{% d => null %}
metakey			-> ("orcid" | "version" | "author" | "name"){% d => d[0] %}

handlers_section-> _ handlers_header _ sep handler:*		{% d => ["handlers", compileObject(d[4])] %}
handler			-> _ var (_ ":" _ | __) "&" var _ sep		{% d => [d[1], d[4]] %}
				 | _ sep									{% d => null %}
handlers_header	-> "#handlers" | "#h"						{% d => null %}

data_section	-> _ data_header _ sep datadef:*			{% d => ["data", compileData(d[4])] %}
datadef			-> _ var (_ ":" _ | __) float  _ sep		{% d => ["float",  d[1], d[3]] %}
				 | _ var (_ ":" _ | __) int    _ sep		{% d => ["int",    d[1], d[3]] %}
				 | _ var (_ ":" _ | __) string _ sep		{% d => ["string", d[1], d[3]] %}
				 | _ sep 									{% d => null %}
data_header		-> "#data" | "#d"							{% d => null %}

code_section	-> _ code_header _ sep statement:*			{% d => ["code", filter(d[4])] %}
statement		-> _ op _ sep								{% d => [null, ...d[1][0]] %}
				 | _ label (_ lineend):* _ op _ sep			{% d => [d[1], ...d[4][0]] %}
				 | _ sep									{% d => null %}
code_header		-> "#code" | "#c"							{% d => null %}

label			-> "@" var									{% d => d[1] %}
#	imm			-> int										{% d => d[0] %}
#		 		 | "&" var									{% d => d[1] %}
var_addr		-> "&" var									{% d => "&" + d[1] %}
var_val			-> "*" var									{% d => "*" + d[1] %}

# Matches either a register or a variable dereference (*var) and returns ["register", ...] or ["label", ...] respectively.
rv				-> reg										{% d => d[0] %}
				 | "*" var									{% d => ["label", d[1]] %}

# Represents an unordered pair of registers and/or variable dereferences with an operator between them.
# If the pair contains one register and one variable dereference, the register will be first.
pair[OPER]		-> reg _ $OPER _ reg						{% d => [d[0], d[4]] %}
				 | reg _ $OPER _ "*" var					{% d => [d[0], ["label", d[5]]] %}
				 | "*" var _ $OPER _ reg					{% d => [d[5], ["label", d[1]]] %}
				 | "*" var _ $OPER _ "*" var				{% d => [["label", d[1]], ["label", d[6]]] %}

op				-> op_add | op_sub | op_mult | op_and | op_nand | op_nor | op_not | op_or | op_xnor | op_xor
				 | op_addi | op_subi | op_multi | op_andi | op_nandi | op_nori | op_ori | op_xnori | op_xori
				 | op_lui | op_mfhi | op_mflo
				 | op_sl | op_sle | op_seq | op_sge | op_sg
				 | op_c | op_l | op_s | op_li | op_si | op_set
				 | op_j | op_jc | op_jr | op_jrc
				 | op_mv | op_ret | op_push | op_pop | op_jeq | op_nop
															{% d => d %}
into			-> _ "->" _									{% d => null %}

#																			   rt    rs    rd
op_add			-> pair["+"]  into reg						{% d => ["add",   d[2], d[0], d[4]] %}
				 | rv _ "+="  _ rv							{% d => ["add",   d[4], d[0], d[0]] %}
op_sub			-> pair["-"]  into reg						{% d => ["sub",   d[2], d[0], d[4]] %}
				 | rv _ "-="  _ rv							{% d => ["sub",   d[4], d[0], d[0]] %}
op_mult			-> pair["*"]  into reg						{% d => ["mult",  d[2], d[0],   0 ] %}
				 | rv _ "*="  _ rv							{% d => ["mult",  d[4], d[0], d[0]] %}
op_and			-> pair["&"]  into reg						{% d => ["and",   d[2], d[0], d[4]] %}
				 | rv _ "&="  _ rv							{% d => ["and",   d[4], d[0], d[0]] %}
op_or			-> pair["|"]  into reg						{% d => ["or",    d[2], d[0], d[4]] %}
				 | rv _ "|="  _ rv							{% d => ["or",    d[4], d[0], d[0]] %}
op_xor			-> pair["x"]  into reg						{% d => ["xor",   d[2], d[0], d[4]] %}
				 | rv _ "x="  _ rv							{% d => ["xor",   d[4], d[0], d[0]] %}
op_nand			-> pair["~&"] into reg						{% d => ["nand",  d[2], d[0], d[4]] %}
				 | rv _ "~&=" _ rv							{% d => ["nand",  d[4], d[0], d[0]] %}
op_nor			-> pair["~|"] into reg						{% d => ["nor",   d[2], d[0], d[4]] %}
				 | rv _ "~|=" _ rv							{% d => ["nor",   d[4], d[0], d[0]] %}
op_xnor			-> pair["~x"] into reg						{% d => ["xnor",  d[2], d[0], d[4]] %}
				 | rv _ "~x=" _ rv							{% d => ["xnor",  d[4], d[0], d[0]] %}
op_not			-> "~" _ rv into rv							{% d => ["not",     0,  d[2], d[4]] %}
op_mfhi			-> "%hi" into rv							{% d => ["mfhi",    0,    0,  d[2]] %}
op_mflo			-> "%lo" into rv							{% d => ["mflo",    0,    0,  d[2]] %}
op_sl			-> rv _ "<"  _ rv into rv					{% d => ["sl",    d[0], d[2], d[4]] %}
op_sle			-> rv _ "<=" _ rv into rv					{% d => ["sle",   d[0], d[2], d[4]] %}
op_seq			-> rv _ "==" _ rv into rv					{% d => ["seq",   d[0], d[2], d[4]] %}
op_sge			-> rv _ ">"  _ rv into rv					{% d => ["sle",   d[2], d[0], d[4]] %}
op_sg			-> rv _ ">=" _ rv into rv					{% d => ["sl",    d[2], d[0], d[4]] %}
op_jr			-> ":" _ reg								{% d => ["jr",      0,    0,  d[2]] %}
op_jrc			-> ":" _ reg _ "(" _ reg _ ")"				{% d => ["jrc",     0,  d[6], d[2]] %}
op_c			-> "[" _ rv _ "]" into "[" _ rv _ "]"		{% d => ["c",       0,  d[2], d[8]] %}
op_l			-> "[" _ rv _ "]" into rv					{% d => ["l",       0,  d[2], d[6]] %}
op_s			-> rv into "[" _ rv _ "]"					{% d => ["s",       0,  d[0], d[4]] %}

#																			   rs    rd    imm
op_addi			-> reg _ "+"  _ int into reg				{% d => ["addi",  d[0], d[4], d[2]] %}
				 | reg _ "++" _								{% d => ["addi",  d[0], d[0],   1 ] %}
				 | _ "++" _ reg								{% d => ["addi",  d[1], d[1],   1 ] %}
				 | reg _ "+=" _ int							{% d => ["addi",  d[0], d[0], d[4]] %}
op_subi			-> reg _ "-"  _ int into reg				{% d => ["subi",  d[0], d[4], d[2]] %}
				 | reg _ "--" _								{% d => ["subi",  d[0], d[0],   1 ] %}
				 | _ "--" _ reg								{% d => ["subi",  d[1], d[1],   1 ] %}
				 | reg _ "-="  _ int						{% d => ["subi",  d[0], d[0], d[4]] %}
op_multi		-> reg _ "*"   _ int						{% d => ["multi", d[0],   0,  d[2]] %}
op_andi			-> reg _ "&"   _ int into reg				{% d => ["andi",  d[0], d[4], d[2]] %}
				 | reg _ "&="  _ int						{% d => ["andi",  d[0], d[0], d[2]] %}
op_ori			-> reg _ "|"   _ int into reg				{% d => ["ori",   d[0], d[4], d[2]] %}
				 | reg _ "|="  _ int						{% d => ["ori",   d[0], d[0], d[2]] %}
op_xori			-> reg _ "x"   _ int into reg				{% d => ["xori",  d[0], d[4], d[2]] %}
				 | reg _ "x="  _ int						{% d => ["xori",  d[0], d[0], d[2]] %}
op_nandi		-> reg _ "~&"  _ int into reg				{% d => ["nandi", d[0], d[4], d[2]] %}
				 | reg _ "~&=" _ int						{% d => ["nandi", d[0], d[0], d[2]] %}
op_nori			-> reg _ "~|"  _ int into reg				{% d => ["nori",  d[0], d[4], d[2]] %}
				 | reg _ "~|=" _ int						{% d => ["nori",  d[0], d[0], d[2]] %}
op_xnori		-> reg _ "~x"  _ int into reg				{% d => ["xnori", d[0], d[4], d[2]] %}
				 | reg _ "~x=" _ int						{% d => ["xnori", d[0], d[0], d[2]] %}
op_lui			-> "lui" _ ":" _ int into reg				{% d => ["lui",     0,  d[6], d[4]] %}
op_li			-> "[" _ int _ "]" into rv					{% d => ["li",      0,  d[4], d[1]] %}
op_si			-> rv into "[" _ int _ "]"					{% d => ["si",    d[0],   0,  d[3]] %}
op_set			-> int into rv								{% d => ["set",     0,  d[2], d[0]] %}

#																			   rs      addr
op_j			-> ":" _ int								{% d => ["j",       0,     d[2]] %}
				 | ":" _ "&" var							{% d => ["j",       0,     d[3]] %}
op_jc			-> ":" _ int _ "(" _ reg _ ")" 				{% d => ["jc",    d[6],    d[2]] %}
				 | ":" _ "&" var _ "(" _ reg _ ")" 			{% d => ["jc",    d[7],    d[3]] %}

op_la			-> var_addr into reg						{% d => ["la", d[0], d[2]] %}
op_mv			-> reg   into reg							{% d => ["mv", d[0], d[2]] %}
op_ret			-> "ret"									{% d => ["jr", 0,  ["register", "return", 0], 0] %}
op_push			-> "[" (_ (reg)):+							{% d => ["push", ...d[1].map(x => x[1][0])] %}
op_pop			-> "]" (_ (reg)):+							{% d => ["pop",  ...d[1].map(x => x[1][0])] %}
op_jeq			-> ":" _ reg _ "(" _ rv _ "==" _ rv ")"		{% d => ["jeq", d[10], d[6], d[2]] %}
				 | ":" _ "&" var _ "(" _ rv _ "==" _ rv ")"	{% d => ["jeq", d[11], d[7], ["label", d[3]]] %}


op_nop			-> "<>"										{% d => ["nop"] %}

reg_temp		-> "$t" ([0-9a-f] | "1" [0-9a])				{% d => ["t", parseInt(d[1].join(""), 16)] %}
reg_saved		-> "$s" ([0-9a-f] | "1" [0-9a])				{% d => ["s", parseInt(d[1].join(""), 16)] %}
reg_arg			-> "$a" [0-9a-f]							{% d => ["a", parseInt(d[1], 16)] %}
reg_return		-> "$r" [0-9a-f]							{% d => ["r", parseInt(d[1], 16)] %}
reg_exc			-> "$e" [0-5]								{% d => ["e", parseInt(d[1], 16)] %}
reg_zero		-> "$" [z0]									{% d => ["zero",   0] %}
reg_retaddr		-> "$" [r<]									{% d => ["return", 0] %}
reg_stack		-> "$" [s*]									{% d => ["stack",  0] %}
reg				-> (reg_temp | reg_saved | reg_arg | reg_return | reg_zero | reg_retaddr | reg_stack | reg_exc)
															{% d => ["register", ...d[0][0]] %}


var -> varchar:+ {%
	(d, location, reject) => {
		const identifier = d[0].join("");
		return /[0-9]/.test(identifier.charAt(0)) || special.words.indexOf(identifier) !== -1? reject : identifier;
	}
%}

single	-> "//" [^\n]:*										{% d => null %}
multi	-> "/*" [^*]:* ("*":+ [^*/] [^*]:*):* "*":* "*/"	{% d => null %}
varchar -> . {% (d, location, reject) => d[0] && special.chars.indexOf(d[0]) === -1 ? d[0] : reject %}
newline -> "\r" "\n" | "\r" | "\n"
_		-> [\t ]:* {% d => null %}
__		-> [\t ]:+ {% d => null %}
