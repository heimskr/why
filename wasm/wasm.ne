@{%
"use strict";

const special = {
	chars: "&*\t \":",
	words: "+ - / * ^ -> < > <= >= = == [ ] :".split(" ")
};

const colors = "1 3 2 6 4 5".split(" ");
const color = (x, n) => `${1 < n? "\n    " : ""}\u001b[3${colors[n % colors.length]}m${x instanceof Array || typeof x == "object"? JSON.stringify(x) : x}\u001b[0m`;

// const list = "float_1 float_2 int".split(/\s+/);
// const list = "op op_add op_or op_not op_nand statement".split(/\s+/);
const list = "meta_section meta metakey".split(/\s+/);
const whitelist = true, quiet = true;
const intercept = (out, ...dbg) => { quiet || (!!whitelist ^ !!list.includes(dbg[0])) || console.log(...dbg.concat(JSON.stringify(out)).map(color), "\n"); return out };
const _ = intercept;
const __ = (out, ...dbg) => { console.log(...[out].concat(dbg).map(color), "\n"); return out };
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

main			-> program									{% d => _(compileObject(d[0]), "main", "compileObject(d[0])", d) %}
program			-> newline:* section:*						{% d => _(filter(d[1], 0, null, "\n"), "program", `filter(d[1], 0, null, "\\n")`, d) %}

lineend			-> (single newline | multi | newline) 		{% d => null %}
sep				-> ";"										{% d => null %}
				 | lineend									{% d => null %}

string			-> dqstring									{% d => _(d[0], "dqstring", "d[0]", d) %}
int_hex			-> "-":? "0x" [0-9a-fA-F]:+					{% d => parseInt((d[0] || "") + d[2].join(""), 16) %}
int_bin			-> "-":? "0b" [01]:+						{% d => parseInt((d[0] || "") + d[2].join(""), 2 ) %}
int_dec			-> "-":? [0-9]:+							{% d => parseInt((d[0] || "") + d[1].join("")    ) %}
int				-> (int_hex | int_bin | int_dec)			{% d => _(d[0][0], "int", "", d) %}
float			-> "-":? [0-9]:+ "." [0-9]:*				{% d => _(parseFloat((d[0] || "") + d[1].join("")/**/ + d[2] + d[3].join("")), "float_1", "parseFloat(...)", d) %}
				 | ("-":? ".") [0-9]:+						{% d => _(parseFloat(filter(d[0]).join("") + d[1].join("")), "float_1", "parseFloat(...)", d) %}

section			-> (data_section | code_section | meta_section | handlers_section)
															{% d => _(d[0][0], "section", "d[0][0]", d) %}

oper[X]			-> _ $X _

meta_section	-> _ meta_header _ sep meta:*				{% d => _(["meta", compileObject(d[4])], "meta_section", `["meta", filter(d[4])]`, d) %}
meta_header		-> "#meta" | "#m"							{% d => null %}
meta			-> _ metakey (oper[":"] | oper["="]) string	{% d => _([d[1][0], d[3]], "meta", `[d[0], d[3]]`, d) %}
				 | _ sep									{% d => null %}
metakey			-> ("orcid" | "version" | "author" | "name")
															{% d => d[0] %}

data_section	-> _ data_header _ sep datadef:*			{% d => _(["data", compileData(d[4])], "data_section", `["data", filter(d[4])]`, d) %}
data_header		-> "#data" | "#d"							{% d => null %}
datadef			-> _ var (oper[":"]|__) float  _ sep		{% d => _(["float",  d[1], d[3]], "datadef", `["float",  d[1], d[3]]`, d) %}
				 | _ var (oper[":"]|__) int    _ sep		{% d => _(["int",    d[1], d[3]], "datadef", `["int",    d[1], d[3]]`, d) %}
				 | _ var (oper[":"]|__) string _ sep		{% d => _(["string", d[1], d[3]], "datadef", `["string", d[1], d[3]]`, d) %}
				 | _ sep 									{% d => null %}

code_section	-> _ code_header _ sep statement:*			{% d => _(["code", filter(d[4])], "code_section", `["code", filter(d[4])]`, d) %}
code_header		-> "#code" | "#c"							{% d => null %}
statement		-> _ op _ sep								{% d => _([null, ...d[1][0]], "statement", `[null, d[1][0]]`, d) %}
				 | _ label (_ lineend):* _ op _ sep			{% d => _([d[1], ...d[4][0]], "statement", `[d[1], d[3][0]]`, d) %}
				 | _ sep									{% d => null %}

handlers_section-> _ handlers_header _ sep handler:*		{% d => _(["handlers", compileObject(d[4])], "handlers_section", `["handlers", filter(d[4])]`, d) %}
handlers_header	-> "#handlers" | "#h"						{% d => null %}
handler			-> _ var (oper[":"]|__) "&" var _ sep		{% d => _([d[1], d[4]], "handler", `[d[1], d[4]]`, d) %}
				 | _ sep									{% d => null %}

label			-> "@" var									{% d => d[1] %}

op				-> op_add | op_sub | op_mult | op_and | op_nand | op_nor | op_not | op_or | op_xnor | op_xor
				 | op_addi | op_subi | op_multi | op_andi | op_nandi | op_nori | op_ori | op_xnori | op_xori
				 | op_lui | op_mfhi | op_mflo
				 | op_sl | op_sle | op_seq | op_sge | op_sg
				 | op_j | op_jc | op_jr
				 | op_la | op_li | op_mv | op_ret | op_push | op_pop | op_jeq
				 | op_c | op_l | op_s
															{% d => d[0] %}

into			-> oper["->"]								{% d => null %}

//																				rs    rt    rd
op_add			-> reg oper["+"]  reg into reg				{% d => _(["add",  d[0], d[2], d[4]], "op_add",  "", d) %}
				 | reg oper["+="] reg						{% d => -(["add",  d[0], d[2], d[0]], "op_add",  "", d) %}
op_sub			-> reg oper["-"]  reg into reg				{% d => _(["sub",  d[0], d[2], d[4]], "op_sub",  "", d) %}
				 | reg oper["-="]  reg 						{% d => _(["sub",  d[0], d[2], d[0]], "op_sub",  "", d) %}
op_mult			-> reg oper["*"]  reg into reg				{% d => _(["mult", d[0], d[2], d[4]], "op_mult", "", d) %}
				 | reg oper["*="]  reg 						{% d => _(["mult", d[0], d[2], d[0]], "op_mult", "", d) %}
op_and			-> reg oper["&"]  reg into reg				{% d => _(["and",  d[0], d[2], d[4]], "op_and",  "", d) %}
				 | reg oper["&="]  reg 						{% d => _(["and",  d[0], d[2], d[0]], "op_and",  "", d) %}
op_or			-> reg oper["|"]  reg into reg				{% d => _(["or",   d[0], d[2], d[4]], "op_or",   "", d) %}
				 | reg oper["|="]  reg 						{% d => _(["or",   d[0], d[2], d[0]], "op_or",   "", d) %}
op_xor			-> reg oper["x"]  reg into reg				{% d => _(["xor",  d[0], d[2], d[4]], "op_xor",  "", d) %}
				 | reg oper["x="]  reg 						{% d => _(["xor",  d[0], d[2], d[0]], "op_xor",  "", d) %}
op_nand			-> reg oper["~&"] reg into reg				{% d => _(["nand", d[0], d[2], d[4]], "op_nand", "", d) %}
				 | reg oper["~&="] reg 						{% d => _(["nand", d[0], d[2], d[0]], "op_nand", "", d) %}
op_nor			-> reg oper["~|"] reg into reg				{% d => _(["nor",  d[0], d[2], d[4]], "op_nor",  "", d) %}
				 | reg oper["~|="] reg 						{% d => _(["nor",  d[0], d[2], d[0]], "op_nor",  "", d) %}
op_xnor			-> reg oper["~x"] reg into reg				{% d => _(["xnor", d[0], d[2], d[4]], "op_xnor", "", d) %}
				 | reg oper["~x="] reg 						{% d => _(["xnor", d[0], d[2], d[0]], "op_xnor", "", d) %}
op_not			-> "~" _ reg into reg						{% d => _(["not",  d[0], d[3]      ], "op_not",  "", d) %}

var_addr		-> "&" var									{% d => "&" + d[1]    %}
var_val			-> "*" var									{% d => "*" + d[1]    %}
imm				-> (int | var_val | var_addr)				{% d => d[0][0] %}

op_addi			-> reg oper["+"]  imm into reg				{% d => _(["addi",  d[0], d[2], d[4]], "op_addi",  "", d) %}
				 | reg oper["++"]							{% d => _(["addi",  d[0], 1,    d[0]], "op_addi",  "", d) %}
				 | oper["++"] reg							{% d => _(["addi",  d[1], 1,    d[1]], "op_addi",  "", d) %}
op_subi			-> reg oper["-"]  imm into reg				{% d => _(["subi",  d[0], d[2], d[4]], "op_subi",  "", d) %}
				 | reg oper["--"]							{% d => _(["subi",  d[0], 1,    d[0]], "op_subi",  "", d) %}
				 | oper["--"] reg							{% d => _(["subi",  d[1], 1,    d[1]], "op_subi",  "", d) %}
op_multi		-> reg oper["*"]  imm into reg				{% d => _(["multi", d[0], d[2], d[4]], "op_multi", "", d) %}
op_andi			-> reg oper["&"]  imm into reg				{% d => _(["andi",  d[0], d[2], d[4]], "op_andi",  "", d) %}
op_ori			-> reg oper["|"]  imm into reg				{% d => _(["ori",   d[0], d[2], d[4]], "op_ori",   "", d) %}
op_xori			-> reg oper["x"]  imm into reg				{% d => _(["xori",  d[0], d[2], d[4]], "op_xori",  "", d) %}
op_nandi		-> reg oper["~&"] imm into reg				{% d => _(["nandi", d[0], d[2], d[4]], "op_nandi", "", d) %}
op_nori			-> reg oper["~|"] imm into reg				{% d => _(["nori",  d[0], d[2], d[4]], "op_nori",  "", d) %}
op_xnori		-> reg oper["~x"] imm into reg				{% d => _(["xnori", d[0], d[2], d[4]], "op_xnori", "", d) %}

op_lui			-> ("lui" _ ":" _) imm into reg				{% d => _(["lui",  d[1], d[3]], "op_lui",  "", d) %}
op_mfhi			-> "%hi" into reg							{% d => _(["mfhi", d[2]      ], "op_mfhi", "", d) %}
op_mflo			-> "%lo" into reg							{% d => _(["mflo", d[2]      ], "op_mflo", "", d) %}

op_sl			-> reg oper["<"]  reg into reg				{% d => _(["sl",  d[0], d[2], d[4]], "op_sl",  "", d) %}
op_sle			-> reg oper["<="] reg into reg				{% d => _(["sle", d[0], d[2], d[4]], "op_sle", "", d) %}
op_seq			-> reg oper["=="] reg into reg				{% d => _(["seq", d[0], d[2], d[4]], "op_seq", "", d) %}
op_sge			-> reg oper[">"]  reg into reg				{% d => _(["sge", d[0], d[2], d[4]], "op_sge", "", d) %}
op_sg			-> reg oper[">="] reg into reg				{% d => _(["sg",  d[0], d[2], d[4]], "op_sg",  "", d) %}

op_j			-> ":" _ (imm | var)						{% d => _(["jump", d[2][0]], "op_j", "", d) %}
op_jc			-> "?" _ (imm | var)						{% d => _(["jc", d[2][0]], "op_jc", "", d) %}
op_jr			-> ":" _ reg								{% d => _(["jr", d[2][0]], "op_jr", "", d) %}
op_la			-> var_addr into reg						{% d => _(["la", d[0], d[2]], "op_la", "", d) %}
op_li			-> int   into reg							{% d => _(["li", d[0], d[2]], "op_li", "", d) %}
op_mv			-> reg   into reg							{% d => _(["mv", d[0], d[2]], "op_mv", "", d) %}
op_ret			-> "ret"									{% d => _(["ret"], "op_ret", "", d) %}
op_push			-> "[" (_ (reg)):+							{% d => _(["push", ...d[1].map(x => x[1][0])], "op_push", "", d) %}
op_pop			-> "]" (_ (reg)):+							{% d => _(["pop", ...d[1].map(x => x[1][0])], "op_pop", "", d) %}
op_jeq			-> reg oper["=="] reg oper["?"] reg			{% d => _(["jeq", d[0], d[2], d[4]], "op_jeq", "", d) %}
				 | reg oper["=="] reg oper["?"] var			{% d => _(["jeq", d[0], d[2], ["label", d[4]]], "op_jeq", "", d) %}

op_c			-> "[" _ reg _ "]" into "[" _ reg _ "]"		{% d => _(["mem", d[2], d[8], 0], "op_c", "", d) %}
op_l			-> "[" _ reg _ "]" into reg					{% d => _(["mem", d[2], d[6], 1], "op_l", "", d) %}
op_s			-> reg into "[" _ reg _ "]"					{% d => _(["mem", d[0], d[4], 2], "op_s", "", d) %}

reg_temp		-> "$t" ([0-9a-f] | "1" [0-9a])				{% d => _(["t", d[1].join("")], "reg_temp",  `["t", d[1].join("")]`, d) %}
reg_saved		-> "$s" ([0-9a-f] | "1" [0-9a])				{% d => _(["s", d[1].join("")], "reg_saved", `["s", d[1].join("")]`, d) %}
reg_arg			-> "$a" [0-9a-f]							{% d => _(["a", d[1]], "reg_arg",     `["a", d[1]]`, d) %}
reg_return		-> "$r" [0-9a-f]							{% d => _(["r", d[1]], "reg_return",  `["r", d[1]]`, d) %}
reg_exc			-> "$e" [0-5]								{% d => _(["e", d[1]], "reg_exc",     `["e", d[1]]`, d) %}
reg_zero		-> "$" [z0]									{% d => _(["0"		], "reg_zero",    `["0"]`,       d) %}
reg_retaddr		-> "$" [r<]									{% d => _(["return" ], "reg_retaddr", `["return"]`,  d) %}
reg_stack		-> "$" [s*]									{% d => _(["stack"], "reg_stack", `["stack"]`, d) %}
reg				-> (reg_temp | reg_saved | reg_arg | reg_return | reg_zero | reg_retaddr | reg_stack | reg_exc)
															{% d => _(["register", ...d[0][0]], "reg", "[,,]", d) %}


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
