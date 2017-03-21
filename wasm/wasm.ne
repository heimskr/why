@{%
// Thanks to nanalan (https://github.com/nanalan/g) for providing a nearley sample I could use to teach myself how to use nearley
"use strict";

const special = {
	chars: "()<>. +-*/^\\!#='\",!{}:[]~$|&;",
	words: "+ - / * ^ -> < > <= >= = == [ ]".split(" ")
};

const colors = "1 3 2 6 4 5".split(" ");
const color = (x, n) => `${1 < n? "\n    " : ""}\u001b[3${colors[n % colors.length]}m${x instanceof Array || typeof x == "object"? JSON.stringify(x) : x}\u001b[0m`;

const skip = "float_1 float_2 int".split(/\s+/);
const intercept = (out, ...dbg) => { skip.includes(dbg[0]) || console.log(...dbg.concat(JSON.stringify(out)).map(color), "\n"); return out };
const _ = intercept;
const __ = (out, ...dbg) => { console.log(...[out].concat(dbg).map(color), "\n"); return out };
const flatten = (arr) => arr.filter((e) => e != null).reduce((flat, toFlatten) => flat.filter((e) => e != null).concat(Array.isArray(toFlatten) ? flatten(toFlatten) : toFlatten), []);
const filter = (arr, index=null, ...remove) => arr.filter((item) => !(remove.length? remove : [null]).includes(typeof index == "number" && 0 <= index? item[index] : item));
%}

@builtin "string.ne"

main		-> program										{% d => _(d[0], "main", "d[0]", d) %}
program		-> newline:* section:*							{% d => _(filter(d[1], 0, null, "\n"), "program", `filter(d[1], 0, null, "\\n")`, d) %}
commands	-> command:*									{% d => _(filter(d[0]), "commands", "filter(d[0])", d) %}

command		-> _ newline									{% d => null %}

lineend		-> _ (single newline:+ | multi:+ | newline) 	{% d => null %}
separator	-> _ ";"										{% d => null %}
			 | lineend										{% d => null %}

float		-> "-":? [0-9]:+ "." [0-9]:*					{% d => _(parseFloat((d[0] || "") + d[1].join("")/**/ + d[2] + d[3].join("")), "float_1", "parseFloat(...)", d) %}
			 | ("-":? ".") [0-9]:+							{% d => _(parseFloat(filter(d[0]).join("") + d[1].join("")), "float_1", "parseFloat(...)", d) %}
int			-> "-":? [0-9]:+								{% d => _(parseInt((d[0] || "") + d[1].join("")), "int", "parseInt(...)", d) %}
string		-> dqstring										{% d => _(d[0], "dqstring", "d[0]", d) %}

section			-> (data_section | code_section)			{% d => _(d[0][0], "section", "d[0][0]", d) %}
data_section	-> _ "#data" _ newline datadef:*			{% d => _(["data", filter(d[4])], "data_section", `["data", filter(d[4])]`, d) %}
datadef			-> _ var (_ ":" _ | __) float  separator	{% d => _(["float",  d[1], d[3]], "datadef", `["float",  d[1], d[3]]`, d) %}
				 | _ var (_ ":" _ | __) int    separator	{% d => _(["int",    d[1], d[3]], "datadef", `["int",    d[1], d[3]]`, d) %}
				 | _ var (_ ":" _ | __) string separator	{% d => _(["string", d[1], d[3]], "datadef", `["string", d[1], d[3]]`, d) %}
				 | separator {% d => null %}

single	-> "//" [^\n]:*										{% d => null %}
multi	-> "/*" [^*]:* ("*":+ [^*/] [^*]:*):* "*":* "*/"	{% d => null %}

var -> varchar:+ {%
	(d, _, no) => {
		const identifier = d[0].join("");
		return /[0-9]/.test(identifier.charAt(0)) || special.words.indexOf(identifier) !== -1? no : identifier;
	}
%}

varchar -> . {% (d, _, no) => d[0] && special.chars.indexOf(d[0]) === -1 ? d[0] : no %}
newline -> "\r" "\n" | "\r" | "\n"
_		-> [\t ]:* {% d => null %}
__		-> [\t ]:+ {% d => null %}
