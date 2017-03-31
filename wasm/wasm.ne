@{%
"use strict";

const special = {
	chars: "&*\t \":",
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

oper[X]			-> _ $X _

meta_section	-> _ meta_header _ sep meta:*				{% d => ["meta", compileObject(d[4])] %}
meta_header		-> "#meta" | "#m"							{% d => null %}
meta			-> _ metakey (oper[":"] | oper["="]) string	{% d => [d[1][0], d[3]] %}
				 | _ sep									{% d => null %}
metakey			-> ("orcid" | "version" | "author" | "name"){% d => d[0] %}

handlers_section-> _ handlers_header _ sep handler:*		{% d => ["handlers", compileObject(d[4])] %}
handler			-> _ var (oper[":"]|__) "&" var _ sep		{% d => [d[1], d[4]] %}
				 | _ sep									{% d => null %}
handlers_header	-> "#handlers" | "#h"						{% d => null %}

data_section	-> _ data_header _ sep datadef:*			{% d => ["data", compileData(d[4])] %}
datadef			-> _ var (oper[":"]|__) float  _ sep		{% d => ["float",  d[1], d[3]] %}
				 | _ var (oper[":"]|__) int    _ sep		{% d => ["int",    d[1], d[3]] %}
				 | _ var (oper[":"]|__) string _ sep		{% d => ["string", d[1], d[3]] %}
				 | _ sep 									{% d => null %}
data_header		-> "#data" | "#d"							{% d => null %}

code_section	-> _ code_header _ sep statement:*			{% d => ["code", filter(d[4])] %}
statement		-> _ op _ sep								{% d => [null, ...d[1][0]] %}
				 | _ label (_ lineend):* _ op _ sep			{% d => [d[1], ...d[4][0]] %}
				 | _ sep									{% d => null %}
code_header		-> "#code" | "#c"							{% d => null %}

label			-> "@" var									{% d => d[1]       %}
var_addr		-> "&" var									{% d => "&" + d[1] %}
var_val			-> "*" var									{% d => "*" + d[1] %}
imm				-> (int | var_val | var_addr)				{% d => d[0][0]    %}

op				-> op_add | op_sub | op_mult | op_and | op_nand | op_nor | op_not | op_or | op_xnor | op_xor
				 | op_addi | op_subi | op_multi | op_andi | op_nandi | op_nori | op_ori | op_xnori | op_xori
				 | op_lui | op_mfhi | op_mflo
				 | op_sl | op_sle | op_seq | op_sge | op_sg
				 | op_j | op_jc | op_jr
				 | op_la | op_li | op_mv | op_ret | op_push | op_pop | op_jeq
				 | op_c | op_l | op_s
															{% d => d[0] %}
into			-> oper["->"]								{% d => null %}

#																			   rt    rs    rd
op_add			-> reg oper["+"]   reg into reg				{% d => ["add",   d[2], d[0], d[4]] %}
				 | reg oper["+="]  reg						{% d => ["add",   d[2], d[0], d[0]] %}
op_sub			-> reg oper["-"]   reg into reg				{% d => ["sub",   d[2], d[0], d[4]] %}
				 | reg oper["-="]  reg 						{% d => ["sub",   d[2], d[0], d[0]] %}
op_mult			-> reg oper["*"]   reg into reg				{% d => ["mult",  d[2], d[0], d[4]] %}
				 | reg oper["*="]  reg 						{% d => ["mult",  d[2], d[0], d[0]] %}
op_and			-> reg oper["&"]   reg into reg				{% d => ["and",   d[2], d[0], d[4]] %}
				 | reg oper["&="]  reg 						{% d => ["and",   d[2], d[0], d[0]] %}
op_or			-> reg oper["|"]   reg into reg				{% d => ["or",    d[2], d[0], d[4]] %}
				 | reg oper["|="]  reg 						{% d => ["or",    d[2], d[0], d[0]] %}
op_xor			-> reg oper["x"]   reg into reg				{% d => ["xor",   d[2], d[0], d[4]] %}
				 | reg oper["x="]  reg 						{% d => ["xor",   d[2], d[0], d[0]] %}
op_nand			-> reg oper["~&"]  reg into reg				{% d => ["nand",  d[2], d[0], d[4]] %}
				 | reg oper["~&="] reg 						{% d => ["nand",  d[2], d[0], d[0]] %}
op_nor			-> reg oper["~|"]  reg into reg				{% d => ["nor",   d[2], d[0], d[4]] %}
				 | reg oper["~|="] reg 						{% d => ["nor",   d[2], d[0], d[0]] %}
op_xnor			-> reg oper["~x"]  reg into reg				{% d => ["xnor",  d[2], d[0], d[4]] %}
				 | reg oper["~x="] reg 						{% d => ["xnor",  d[2], d[0], d[0]] %}
op_not			-> "~" _ reg into reg						{% d => ["not",     0,  d[2], d[4]] %}
op_mfhi			-> "%hi" into reg							{% d => ["mfhi",    0,    0,  d[2]] %}
op_mflo			-> "%lo" into reg							{% d => ["mflo",    0,    0,  d[2]] %}
op_sl			-> reg oper["<"]  reg into reg				{% d => ["sl",    d[0], d[2], d[4]] %}
op_sle			-> reg oper["<="] reg into reg				{% d => ["sle",   d[0], d[2], d[4]] %}
op_seq			-> reg oper["=="] reg into reg				{% d => ["seq",   d[0], d[2], d[4]] %}
op_sge			-> reg oper[">"]  reg into reg				{% d => ["sle",   d[2], d[0], d[4]] %}
op_sg			-> reg oper[">="] reg into reg				{% d => ["sl",    d[2], d[0], d[4]] %}
op_jr			-> ":" _ reg								{% d => ["jr",      0,  d[2],   0 ] %}
op_c			-> "[" _ reg _ "]" into "[" _ reg _ "]"		{% d => ["c",       0,  d[2], d[8]] %}
op_l			-> "[" _ reg _ "]" into reg					{% d => ["l",       0,  d[2], d[6]] %}
op_s			-> reg into "[" _ reg _ "]"					{% d => ["s",       0,  d[0], d[4]] %}
#																			   rs    rd    imm
op_addi			-> reg oper["+"]   imm into reg				{% d => ["addi",  d[0], d[4], d[2]] %}
				 | reg oper["++"]							{% d => ["addi",  d[0], d[0], 1,  ] %}
				 | oper["++"] reg							{% d => ["addi",  d[1], d[1], 1,  ] %}
op_subi			-> reg oper["-"]   imm into reg				{% d => ["subi",  d[0], d[4], d[2]] %}
				 | reg oper["--"]							{% d => ["subi",  d[0], d[0], 1,  ] %}
				 | oper["--"] reg							{% d => ["subi",  d[1], d[1], 1,  ] %}
op_multi		-> reg oper["*"]   imm into reg				{% d => ["multi", d[0], d[4], d[2]] %}
				 | reg oper["*="]  imm						{% d => ["multi", d[0], d[0], d[2]] %}
op_andi			-> reg oper["&"]   imm into reg				{% d => ["andi",  d[0], d[4], d[2]] %}
				 | reg oper["&="]  imm						{% d => ["andi",  d[0], d[0], d[2]] %}
op_ori			-> reg oper["|"]   imm into reg				{% d => ["ori",   d[0], d[4], d[2]] %}
				 | reg oper["|="]  imm						{% d => ["ori",   d[0], d[0], d[2]] %}
op_xori			-> reg oper["x"]   imm into reg				{% d => ["xori",  d[0], d[4], d[2]] %}
				 | reg oper["x="]  imm						{% d => ["xori",  d[0], d[0], d[2]] %}
op_nandi		-> reg oper["~&"]  imm into reg				{% d => ["nandi", d[0], d[4], d[2]] %}
				 | reg oper["~&="] imm						{% d => ["nandi", d[0], d[0], d[2]] %}
op_nori			-> reg oper["~|"]  imm into reg				{% d => ["nori",  d[0], d[4], d[2]] %}
				 | reg oper["~|="] imm						{% d => ["nori",  d[0], d[0], d[2]] %}
op_xnori		-> reg oper["~x"]  imm into reg				{% d => ["xnori", d[0], d[4], d[2]] %}
				 | reg oper["~x="] imm						{% d => ["xnori", d[0], d[0], d[2]] %}
op_lui			-> ("lui" _ ":" _) imm into reg				{% d => ["lui",     0,  d[3], d[1]] %}
#																			   rs        addr
op_j			-> ":" _ (imm | var)						{% d => ["j",       0,     d[2][0]] %}
op_jc			-> reg "?" _ (imm | var)					{% d => ["jc",    d[0],    d[2][0]] %}

op_la			-> var_addr into reg						{% d => ["la", d[0], d[2]] %}
op_li			-> int   into reg							{% d => ["li", d[0], d[2]] %}
op_mv			-> reg   into reg							{% d => ["mv", d[0], d[2]] %}
op_ret			-> "ret"									{% d => ["ret"] %}
op_push			-> "[" (_ (reg)):+							{% d => ["push", ...d[1].map(x => x[1][0])] %}
op_pop			-> "]" (_ (reg)):+							{% d => ["pop", ...d[1].map(x => x[1][0])] %}
op_jeq			-> reg oper["=="] reg oper["?"] reg			{% d => ["jeq", d[0], d[2], d[4]] %}
				 | reg oper["=="] reg oper["?"] var			{% d => ["jeq", d[0], d[2], ["label", d[4]]] %}

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
