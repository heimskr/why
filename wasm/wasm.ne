@{%
"use strict";

const special = {
	chars: " \":",
	words: "+ - / * ^ -> < > <= >= = == [ ]".split(" ")
};

const colors = "1 3 2 6 4 5".split(" ");
const color = (x, n) => `${1 < n? "\n    " : ""}\u001b[3${colors[n % colors.length]}m${x instanceof Array || typeof x == "object"? JSON.stringify(x) : x}\u001b[0m`;

// const list = "float_1 float_2 int".split(/\s+/);
// const list = "op op_add op_or op_not op_nand statement".split(/\s+/);
const list = "meta_section meta metakey".split(/\s+/);
const whitelist = true, quiet = false;
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
separator		-> ";"										{% d => null %}
				 | lineend									{% d => null %}

string			-> dqstring									{% d => _(d[0], "dqstring", "d[0]", d) %}
int_hex			-> "-":? "0x" [0-9a-fA-F]:+					{% d => parseInt((d[0] || "") + d[2].join(""), 16) %}
int_bin			-> "-":? "0b" [01]:+						{% d => parseInt((d[0] || "") + d[2].join(""), 2 ) %}
int_dec			-> "-":? [0-9]:+							{% d => parseInt((d[0] || "") + d[1].join("")    ) %}
int				-> (int_hex | int_bin | int_dec)			{% d => _(d[0][0], "int", "", d) %}
float			-> "-":? [0-9]:+ "." [0-9]:*				{% d => _(parseFloat((d[0] || "") + d[1].join("")/**/ + d[2] + d[3].join("")), "float_1", "parseFloat(...)", d) %}
				 | ("-":? ".") [0-9]:+						{% d => _(parseFloat(filter(d[0]).join("") + d[1].join("")), "float_1", "parseFloat(...)", d) %}

section			-> (data_section | code_section | meta_section)
															{% d => _(d[0][0], "section", "d[0][0]", d) %}

oper[X]			-> _ $X _

meta_section	-> _ "#meta" _ separator meta:*				{% d => _(["meta", compileObject(d[4])], "meta_section", `["meta", filter(d[4])]`, d) %}
meta			-> _ metakey (oper[":"] | oper["="]) string	{% d => _([d[1][0], d[3]], "meta", `[d[0], d[3]]`, d) %}
				 | _ separator								{% d => null %}
metakey			-> ("orcid" | "version" | "author")			{% d => d[0] %}

data_section	-> _ "#data" _ separator datadef:*			{% d => _(["data", compileData(d[4])], "data_section", `["data", filter(d[4])]`, d) %}
datadef			-> _ var (oper[":"]|__) float  _ separator	{% d => _(["float",  d[1], d[3]], "datadef", `["float",  d[1], d[3]]`, d) %}
				 | _ var (oper[":"]|__) int    _ separator	{% d => _(["int",    d[1], d[3]], "datadef", `["int",    d[1], d[3]]`, d) %}
				 | _ var (oper[":"]|__) string _ separator	{% d => _(["string", d[1], d[3]], "datadef", `["string", d[1], d[3]]`, d) %}
				 | _ separator 								{% d => null %}

code_section	-> _ "#code" _ separator statement:*			{% d => _(["code", filter(d[4])], "code_section", `["code", filter(d[4])]`, d) %}
statement		-> _ op _ separator							{% d => _(d[1][0], "statement", `d[1][0]`, d) %}
				 | _ separator								{% d => null %}


op				-> op_add | op_sub | op_mult | op_and | op_nand | op_nor | op_not | op_or | op_xnor | op_xor
				 | op_addi | op_subi | op_multi | op_andi | op_nandi | op_nori | op_ori | op_xnori | op_xori
				 | op_lui | op_mfhi | op_mflo
				 | op_sl | op_sle | op_seq | op_sge | op_sg
				 | op_j | op_la | op_li | op_mv

into			-> oper["->"]								{% d => null %}

op_add			-> reg oper["+"]  reg into reg				{% d => _([ "+", d[0], d[2], d[4]], "op_add",  "", d) %}
op_sub			-> reg oper["-"]  reg into reg				{% d => _([ "-", d[0], d[2], d[4]], "op_sub",  "", d) %}
op_mult			-> reg oper["*"]  reg into reg				{% d => _([ "*", d[0], d[2], d[4]], "op_mult", "", d) %}
op_and			-> reg oper["&"]  reg into reg				{% d => _([ "&", d[0], d[2], d[4]], "op_and",  "", d) %}
op_or			-> reg oper["|"]  reg into reg				{% d => _([ "|", d[0], d[2], d[4]], "op_or",   "", d) %}
op_xor			-> reg oper["x"]  reg into reg				{% d => _([ "x", d[0], d[2], d[4]], "op_xor",  "", d) %}
op_nand			-> reg oper["~&"] reg into reg				{% d => _(["~&", d[0], d[2], d[4]], "op_nand", "", d) %}
op_nor			-> reg oper["~|"] reg into reg				{% d => _(["~|", d[0], d[2], d[4]], "op_nor",  "", d) %}
op_xnor			-> reg oper["~x"] reg into reg				{% d => _(["~x", d[0], d[2], d[4]], "op_xnor", "", d) %}
op_not			-> "~" _ reg into reg						{% d => _([ "~", d[0], d[3]      ], "op_not",  "", d) %}

label			-> "&" var									{% d => d[1]    %}
imm				-> (int | label)							{% d => d[0][0] %}

op_addi			-> reg oper["+"]  imm into reg				{% d => _([ "_+", d[0], d[2], d[4]], "op_addi",  "", d) %}
op_subi			-> reg oper["-"]  imm into reg				{% d => _([ "_-", d[0], d[2], d[4]], "op_subi",  "", d) %}
op_multi		-> reg oper["*"]  imm into reg				{% d => _([ "_*", d[0], d[2], d[4]], "op_multi", "", d) %}
op_andi			-> reg oper["&"]  imm into reg				{% d => _([ "_&", d[0], d[2], d[4]], "op_andi",  "", d) %}
op_ori			-> reg oper["|"]  imm into reg				{% d => _([ "_|", d[0], d[2], d[4]], "op_ori",   "", d) %}
op_xori			-> reg oper["x"]  imm into reg				{% d => _([ "_x", d[0], d[2], d[4]], "op_xori",  "", d) %}
op_nandi		-> reg oper["~&"] imm into reg				{% d => _(["_~&", d[0], d[2], d[4]], "op_nandi", "", d) %}
op_nori			-> reg oper["~|"] imm into reg				{% d => _(["_~|", d[0], d[2], d[4]], "op_nori",  "", d) %}
op_xnori		-> reg oper["~x"] imm into reg				{% d => _(["_~x", d[0], d[2], d[4]], "op_xnori", "", d) %}

op_lui			-> ("lui" _ ":" _) imm into reg				{% d => _(["lui",  d[1], d[3]], "op_lui",  "", d) %}
op_mfhi			-> "%hi" into reg							{% d => _(["mfhi", d[2]      ], "op_mfhi", "", d) %}
op_mflo			-> "%lo" into reg							{% d => _(["mflo", d[2]      ], "op_mflo", "", d) %}

op_sl			-> reg oper["<"]  reg into reg				{% d => _(["<",  d[0], d[2], d[4]], "op_sl",  "", d) %}
op_sle			-> reg oper["<="] reg into reg				{% d => _(["<=", d[0], d[2], d[4]], "op_sle", "", d) %}
op_seq			-> reg oper["=="] reg into reg				{% d => _(["==", d[0], d[2], d[4]], "op_seq", "", d) %}
op_sge			-> reg oper[">"]  reg into reg				{% d => _([">",  d[0], d[2], d[4]], "op_sge", "", d) %}
op_sg			-> reg oper[">="] reg into reg				{% d => _([">=", d[0], d[2], d[4]], "op_sg",  "", d) %}

op_j			-> ":" _ imm								{% d => _(["jump", d[2]], "op_j", "", d) %}
op_la			-> label into reg							{% d => _(["la", d[0], d[2]], "op_la", "", d) %}
op_li			-> int   into reg							{% d => _(["li", d[0], d[2]], "op_li", "", d) %}
op_mv			-> reg   into reg							{% d => _(["mv", d[0], d[2]], "op_mv", "", d) %}

reg_temp		-> "$t" ([0-9a-f] | "1" [0-9a])				{% d => _(["t", d[1].join("")], "reg_temp",  `["t", d[1].join("")]`, d) %}
reg_saved		-> "$s" ([0-9a-f] | "1" [0-9a])				{% d => _(["s", d[1].join("")], "reg_saved", `["s", d[1].join("")]`, d) %}
reg_arg			-> "$a" [0-9a-f]							{% d => _(["a", d[1]], "reg_arg",    `["a", d[1]]`, d) %}
reg_return		-> "$r" [0-9a-f]							{% d => _(["r", d[1]], "reg_return", `["r", d[1]]`, d) %}
reg				-> (reg_temp | reg_saved | reg_arg | reg_return)
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
