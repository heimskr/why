@{%
// Thanks to nanalan (https://github.com/nanalan/g) for providing a nearley sample I could use to teach myself how to use nearley
"use strict";

const special = {
	chars: "()<>. +-*/^\\!#='\",!{}:[]~$|&;",
	words: "+ - / * ^ -> < > <= >= = == [ ]".split(" ")
};

const colors = "1 3 2 6 4 5".split(" ");
const color = (x, n) => `${1 < n? "\n    " : ""}\u001b[3${colors[n % colors.length]}m${x instanceof Array || typeof x == "object"? JSON.stringify(x) : x}\u001b[0m`;
const intercept = (out, ...dbg) => { console.log(...dbg.concat(JSON.stringify(out)).map(color), "\n"); return out };
const _ = intercept;
const __ = (out, ...dbg) => { console.log(...[out].concat(dbg).map(color), "\n"); return out };
const flatten = (arr) => arr.filter((e) => e != null).reduce((flat, toFlatten) => flat.filter((e) => e != null).concat(Array.isArray(toFlatten) ? flatten(toFlatten) : toFlatten), []);
const filter = (arr, index=null, ...remove) => arr.filter((item) => !(remove.length? remove : [null]).includes(typeof index == "number" && 0 <= index? item[index] : item));
%}

@builtin "string.ne"

main		-> program					{% d => _(d[0], "main", "d[0]", d) %}
#main		-> _ datadef				{% d => _(d[0], "main", "d[0]", d, d[0]) %}

program		-> newline:* section:*		{% d => _(filter(d[1], 0, null, "\n"), "program", `filter(d[1], 0, null, "\\n")`, d) %}

#commands	-> command					{% (d, i) => _([d[0], i], "commands", "[d[0], i]", d, [d[0], i]) %}
commands	-> command:*				{% d => _(filter(d[0]), "commands", "filter(d[0])", d) %}
#commands	-> command:*				{% d => _(null, "commands", "null", d, d[0]) %}

command		-> _ newline				{% d => null %}
#			 | _ command separator		{% d => _(d[1], "command[command]", "d[1]", d) %}

separator	-> _ (";" | newline)		{% d => null %}

float		-> [0-9]:+ "." [0-9]:+		{% d => parseFloat(d[0].join("") + d[1] + d[2].join("")) %}
int			-> [0-9]:+					{% d => parseInt(d[0].join("")) %}
string		-> dqstring					{% d => _(d[0], "dqstring", "d[0]", d) %}

value		-> (float | int | string)	{% d => _(d[0][0], "value", "d[0][0]", d) %}

#datadef	-> "^" ("foo" | "bar")		{% d => _(d[0], "datadef", "d[0]", d, d[0]) %}

section			-> (data_section | code_section)	{% d => _(d[0][0], "section", "d[0][0]", d) %}
data_section	-> _ "<data>" datadef:*				{% d => _(["data", filter(d[2])], "data_section", `["data", filter(d[2])]`, d) %}
datadef			-> _ newline						{% d => null %}
				 | _ var _ ":" _ float  separator	{% d => _(["float",  d[1], d[5]], "datadef", `["float",  d[1], d[5]]`, d) %}
				 | _ var _ ":" _ int    separator	{% d => _(["int",    d[1], d[5]], "datadef", `["int",    d[1], d[5]]`, d) %}
				 | _ var _ ":" _ string separator	{% d => _(["string", d[1], d[5]], "datadef", `["string", d[1], d[5]]`, d) %}

var -> varchar:+
{%
	(d, _, no) => {
		const identifier = d[0].join("");
		return /[0-9]/.test(identifier.charAt(0)) || special.words.indexOf(identifier) !== -1? no : identifier;
	}
%}

varchar -> . {% (d, _, no) => d[0] && special.chars.indexOf(d[0]) === -1 ? d[0] : no %}

newline -> "\r" "\n" | "\r" | "\n"		

_  -> [\t ]:*							{% d => null %}
__ -> [\t ]:+							{% d => null %}
