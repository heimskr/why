@{%
"use strict";

const special = {
	chars: "@$&*\t \":()",
	words: "+ - / * ^ -> < > <= >= = == [ ] :".split(" ")
};

const filter = (arr, index=null, ...remove) => arr.filter((item) => !(remove.length? remove : [null]).includes(typeof index == "number" && 0 <= index? item[index] : item));
const select = (arr, ...indices) => indices.map((i) => arr[i]);

const _ = (arg) => {
	if (arg instanceof Array) {
		return arg[0];
	};

	if (typeof arg == "number") {
		return d => d[arg];
	};

	if (arg === null || typeof arg == "undefined") {
		return d => null;
	};

	return () => { throw "Unknown type given to _." };
};

const __ = (x, y) => {
	if (x instanceof Array) {
		return x[0][0];
	};

	if (typeof x == "number") {
		return typeof y == "undefined"? d => d[x][0] : d => d[x][y];
	};

	return () => { throw "Unknown type given to __." };
};

%}

@builtin "string.ne"

main -> item:+												{% d => filter(d[0]) %}

item -> _ lineend											{% _() %}
	  | _ (type | source_filename | target | struct | global)
	  														{% __(1, 0) %}

lineend				-> (comment newline | newline) 				{% d => null %}

comma				-> _ "," _														{% _( ) %}
eq					-> _ "=" _														{% _( ) %}
prop				-> eq dqstring													{% _(1) %}

cstring				-> "c" dqstring													{% _(1) %}
float				-> "-":? [0-9]:+ "." [0-9]:*									{% d => parseFloat((d[0] || "") + d[1].join("") + d[2] + d[3].join("")) %}
					 | ("-":? ".") [0-9]:+											{% d => parseFloat(filter(d[0]).join("") + d[1].join("")) %}
decimal				-> "-":? [0-9]:+												{% d => parseInt((d[0] || "") + d[1].join("")    ) %}

source_filename		-> "source_filename" prop										{% d => ["source_filename", d[1]] %}

target				-> "target" __ targetname prop									{% d => ["target", d[2], d[3]] %}
targetname			-> ("datalayout" | "triple")									{% __(0, 0) %}

type_struct			-> "%struct." var												{% d => ["struct", d[1]] %}
struct_header		-> type_struct eq "type"										{% __(0, 1) %}
struct				-> struct_header __ "opaque"									{% d => ["struct", d[0], "opaque"] %}
					 | struct_header _ "{" _ types _ "}"							{% d => ["struct", d[0], d[4].map((x) => x.slice(1))] %}

types				-> type (_ "," _ type):*										{% d => [d[0], ...d[1].map((x) => x[3])] %}

type_int			-> ("i1" | "i8" | "i16" | "i32" | "i64" | "i256")				{% d => [d[0][0]] %}
type_array			-> "[" _ decimal _ "x" _ type "]"								{% d => ["array", [d[2], ...d[6]]] %}
type_ptr			-> type "*"														{% d => ["ptr", d[0].slice(1)] %}
type_multiptr		-> type_int _ "(" _ types _ ")" _ "*"							{% d => ["multiptr", d[0], d[4].map((x) => x.slice(1))] %}
type				-> (type_multiptr | type_ptr | type_array | type_int | type_struct)
																					{% d => ["type", ...d[0][0]] %}

var_name			-> "@" var														{% _(1) %}
global				-> var_name
					   eq
					   (__ linkage):?
					   (__ visibility):?
					   (__ dll_storage_class):?
					   (__ thread_local):?
					   (__ unnamed_addr):?
					   (__ addrspace):?
					   (__ "externally_initialized"):?
					   (__ global_constant):?
					   __
					   type
					   (__ initial_value):?
					   (comma "section" _ dqstring):?
					   (comma "comdat" _ "(" _ "$" _ var _ ")"):?
					   (comma "align" __ decimal):?
					   #// not sure what "(, !name !N)*" is supposed to mean, but it doesn't to be used in various things I found online, so whatever ¯\_(ツ)_/¯
					   {% d => [
					   		"global",
					   		d[0],					// variable name
					   		d[2]? d[2][1] : null,	// linkage
					   		d[3]? d[3][1] : null,	// visibility
					   		d[4]? d[4][1] : null,	// dll storage class
					   		d[5]? d[5][1] : null,	// thread local
					   		d[6]? d[6][1] : null,	// unnamed_addr
					   		d[7]? d[7][1] : null,	// addrspace
					   		!!d[8],					// externally_initialized
					   		d[9]? d[9][1] : null,	// global_constant
					   		d[11],					// type
					   		d[12]? d[12][1] : null,	// initial value
					   		d[13]? d[13][3] : null, // section
					   		d[14]? d[14][7] : null, // comdat (what is that)
					   		d[15]? d[15][3] : null	// align
				   		] %}

linkage				-> ("private" | "internal" | "available_externally" | "linkonce" | "weak" | "common" | "appending" | "extern_weak" | "linkonce_odr" | "weak_odr" | "external") {% __ %}
visibility			-> "default"													{% d => 0 %}
					 | "hidden"														{% d => 1 %}
					 | "protected"													{% d => 2 %}
dll_storage_class	-> ("dllimport" | "dllexport")									{% __ %}
thread_local		-> ("localdynamic" | "initialexec" | "localexec")				{% __ %}
unnamed_addr		-> ("local_unnamed_addr" | "unnamed_addr")						{% __ %}
addrspace			-> "addrspace" _ "(" _ decimal _ ")"							{% _(4) %}
global_constant		-> ("global" | "constant")										{% __ %}
initial_value		-> (cstring | float | decimal)									{% __ %}


var -> varchar:+ {%
	(d, location, reject) => {
		const identifier = d[0].join("");
		return /[0-9]/.test(identifier.charAt(0)) || special.words.indexOf(identifier) !== -1? reject : identifier;
	}
%}

varchar -> . 						{% (d, location, reject) => d[0] && special.chars.indexOf(d[0]) === -1 ? d[0] : reject %}
comment	-> ";" [^\n]:*				{% d => null %}
_		-> [\t ]:*					{% d => null %}
__		-> [\t ]:+					{% d => null %}
newline -> "\r" "\n" | "\r" | "\n"	{% d => null %}

@{%/*

Potentially useful links:
	http://llvm.org/docs/LangRef.html
	http://stackoverflow.com/questions/36087319/llvm-openmp-what-is-the-meaning-of-internal-thread-local-unnamed-addr-global/36094052

*/%}