@{%
// Thanks to nanalan (https://github.com/nanalan/g) for providing a nearley sample I could use to teach myself how to use nearley
"use strict";

const special = {
	chars: "()<>. +-*/^\\!#='\",!{}:[]",
	words: "+ - / * ^ -> < > <= >= = == [ ]".split(" ")
};

const colors = "1 3 2 6 4 5".split(" ");
const color = (x, n) => {
	return `${[1, 3].includes(n)? "\n\t" : ""}\u001b[3${colors[n % colors.length]}m${x instanceof Array || typeof x == "object"? JSON.stringify(x) : x}\u001b[0m`;
}

const intercept = (out, ...dbg) => { console.log(...dbg.map(color), "\n"); return out };
const _ = intercept;
const __ = (out, ...dbg) => { console.log(...[out].concat(dbg).map(color), "\n"); return out };
const flatten = (arr) => arr.filter((e) => e != null).reduce((flat, toFlatten) => flat.filter((e) => e != null).concat(Array.isArray(toFlatten) ? flatten(toFlatten) : toFlatten), []);
%}

@builtin "string.ne"

main		-> program					{% d => _(d[0], "main", "d[0]", d[0], d) %}
# main		-> _ datadef				{% d => _(d[0], "main", "d[0]", d[0], d) %}

program		-> commands:*				{% d => _(d[0].filter(l => l[0] !== null), "program", "_(d[0].filter(l => l[0] !== null)", d[0], d) %}
commands	-> commandn					{% (d, i) => _([d[0], i], "commands", "[d[0], i]", [d[0], i], d, i) %}

commandn	-> _ newline				{% d => _(null, "commandn[newline]", "null", null, d) %}
			 | _ command _ newline		{% d => _(d[1], "commandn[command newline]", "d[1]", d[1], d) %}
			 | _ section_id _ newline   {% d => _(d[1], "commandn[section_id]", "d[1]", d[1], d) %}
			 | _ call					{% d => _(["call", d[1]], "commandn[call]", `(["call", d[1]])`, ["call", d[1]], d) %}

float		-> int "." [0-9]:+			{% (d) => parseFloat(d[0] + d[1] + d[2]) %}
int			-> [0-9]:+					{% (d) => d[0].join("") %}

# datadef		-> "^" ("foo" | "bar")	    {% (d) => _(d[0], "datadef", "d[0]", d[0], d) %}
datadef		-> "^foo"	   				{% (d) => _(d[0], "datadef", "d[0]", d[0], d) %}

command		-> "COMMAND"
call		-> "CALL"

section_id	-> "data" | "code"			{% (d) => _(null, "section_id", "null", null, d) %}

string -> dqstring						{% d => _(d[0], "dqstring", "d[0]", d[0], d) %}

var -> varchar:+	{%
		(d, _, no) => {
			const identifier = d[0].join("");
			return /[0-9]/.test(identifier.charAt(0)) || special.words.indexOf(identifier) !== -1? no : identifier;
		}
%}

varchar -> . {% (d, _, no) => d[0] && special.chars.indexOf(d[0]) === -1 ? d[0] : no %}

newline -> "\r" "\n" | "\r" | "\n"

_  -> " ":*							{% (d) => null %}
__ -> " ":+							{% (d) => null %}
