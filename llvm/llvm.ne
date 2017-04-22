@{%
"use strict";

const { uniq: unique } = require("lodash");

const special = {
	chars: "=@$&*\t \":()",
	words: "+ - / * ^ -> < > <= >= = == [ ] :".split(" ")
};

const filter = (arr, index=null, ...remove) => arr.filter((item) => !(remove.length? remove : [null]).includes(typeof index == "number" && 0 <= index? item[index] : item));
const select = (multidim, index) => multidim.map((arr) => arr[index]);

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

const compileFastMathFlags = (flags) => flags.includes("fast")? ["nnan", "ninf", "nsz", "arcp", "constract", "fast"] : unique(flags);

const compileFnty = (d) => [d[0], d[2], !!d[3]]; // [ return type: Type, argument types (excluding "..."): [Type], is_varargs: Boolean ]

%}

@include "strings.ne"

main -> item:+																		{% d => filter(d[0]) %}
item -> _ lineend																	{% _() %}
	  | _ (type_any | source_filename | target | struct | global | function_def)	{% __(1, 0) %}

lineend				->	(comment newline | newline) 								{% _( ) %}
spaced[X]			->	" " $X " "													{% _(1) %}
list[X]				->	$X (" " $X):*												{% d => [d[0], ...d[1].map((x) => x[1])] %}
commalist[X]		->	$X (", " $X):*												{% d => [d[0], ...d[1].map((x) => x[1])] %}
pars[X]				->	"(" $X ")"													{% _(1) %}

comma				->	_ "," _														{% _( ) %}
eq					->	_ "=" _														{% _( ) %}
prop				->	eq string													{% _(1) %}

cstring				->	"c" string													{% _(1) %}
float				->	"-":? [0-9]:+ "." [0-9]:*									{% d => parseFloat((d[0] || "") + d[1].join("") + d[2] + d[3].join("")) %}
					 |	("-":? ".") [0-9]:+											{% d => parseFloat(filter(d[0]).join("") + d[1].join("")) %}
decimal				->	"-":? [0-9]:+												{% d => parseInt((d[0] || "") + d[1].join("")    ) %}
natural				->	[1-9] [0-9]:*												{% d => parseInt(d[0] + d[1].join("")) %}														

source_filename		->	"source_filename" prop										{% d => ["source_filename", d[1]] %}

target				->	"target" __ targetname prop									{% d => ["target", d[2], d[3]] %}
targetname			->	("datalayout" | "triple")									{% __(0, 0) %}

variable			->	"%" (var | decimal | string)								{% d => ["variable", d[1][0]] %}
temporary			->	"%" decimal													{% d => d[1] %}

type_struct			->	"%struct." var												{% d => ["struct", d[1]] %}
struct_header		->	type_struct eq "type"										{% __(0, 1) %}
struct				->	struct_header __ "opaque"									{% d => ["struct", d[0], "opaque"] %}
					 |	struct_header _ "{" _ types _ "}"							{% d => ["struct", d[0], d[4].map((x) => x[0])] %}

types				->	commalist[type_any]											{% _ %}

type_int			->	"i" natural													{% d => ["int", d[1]] %}
type_array			->	"[" _ natural _ "x" _ type_any "]"							{% d => ["array", d[2], d[6].slice(1)] %}
type_vector			->	"<" _ natural _ "x" _ vector_type ">"						{% d => ["vector", d[2], d[6]] %}
vector_type			->	(type_int | type_ptr)										{% d => d[0][0] %}
					 |	"float"														{% d => ["float"] %}
type_ptr			->	type_any _ "*"												{% d => ["ptr", d[0].slice(1)] %}
type_multiptr		->	type_int _ "(" _ types _ ")" _ "*"							{% d => ["multiptr", d[0], d[4].map((x) => x.slice(1))] %}
type_void			->	"void"														{% d => ["void"] %}
type_function		->	type_any _ "(" _ type_function_args ")"						{% d => ["function", d[0], d[3]] %}
type_any			->	(type_multiptr | type_ptr | type_array | type_vector | type_int | type_struct | type_void)
																					{% d => ["type", ...d[0][0]] %}
type_intvec			->	(type_int | type_vector)									{% __ %}

type_function_args	->	type_any (comma type_any):* (comma "...") _					{% d => [d[0], ...d[1].map((x) => x.slice(1)), "..."] %}
					 |	type_any (comma type_any):* _								{% d => [d[0], ...d[1].map((x) => x.slice(1))] %}
					 |	null															{% d => [] %}

var_name			->	"@" var														{% _(1) %}
label				->	var ":"														{% d => ["label", d[0]] %}
global				->	var_name
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
						type_any
						(__ initial_value):?
						(comma "section" _ string):?
						(comma "comdat" _ "(" _ "$" _ var _ ")"):?
						(comma "align" __ decimal):?
						#// not sure what "(, !name !N)*" is supposed to mean, but it doesn't to be used in various things I found online, so whatever ¯\_(ツ)_/¯
						{% d => [
							"global",
							d[0],					// variable name
							d[2]? d[2][1] : null,	// linkage
							d[3]? d[3][1] : null,	// visibility
							d[4]? d[4][1] : null,	// dll storage class
							d[5] || null,			// thread local
							d[6]? d[6][1] : null,	// unnamed_addr
							d[7]? d[7][1] : null,	// addrspace
							!!d[8],					// externally_initialized
							d[9]? d[9][1] : null,	// global_constant
							d[11],					// type
							d[12]? d[12][1] : null,	// initial value
							d[13]? d[13][3] : null, 	// section
							d[14]? d[14][7] : null, 	// comdat (what is that)
							d[15]? d[15][3] : null	// align
				   		] %}

linkage				->	("private" | "internal" | "available_externally" | "linkonce" | "weak" | "common" | "appending" | "extern_weak" | "linkonce_odr" | "weak_odr" | "external")
																					{% __ %}
visibility			->	"default"													{% d => 0 %}
					 |	"hidden"														{% d => 1 %}
					 |	"protected"													{% d => 2 %}
dll_storage_class	->	("dllimport" | "dllexport")									{% __ %}
thread_local		->	"thread_local" _ "(" _ ("localdynamic" | "initialexec" | "localexec") _ ")"
																					{% d => d[4][0] %}
unnamed_addr		->	("local_unnamed_addr" | "unnamed_addr")						{% __ %}
addrspace			->	"addrspace" _ "(" _ decimal _ ")"							{% _(4) %}
global_constant		->	("global" | "constant")										{% __ %}
initial_value		->	cstring														{% d => ["string",  d[0]] %}
					 |	float														{% d => ["float",   d[0]] %}
					 |	decimal														{% d => ["decimal", d[0]] %}
					 |	"zeroinitializer"											{% d => ["zero"] %}

function_header		->	"define"
						(" " linkage):?
						(" " visibility):?
						(" " dll_storage_class):?
						(" " cconv):?
						(" " retattr):*
						__
						type_any
						__
						var_name
						(_ "(")
						function_types:?
						")"
						(_ unnamed_addr):?
						((__ fnattr):+ | __ "#" decimal):?
						("  section" _ string):?
						("  comdat" _ "(" _ "$" _ var _ ")"):?
						("  align" _ decimal):?
						("  gc" _ string):?
						("  prefix" __ constant):? # what about that "@md" thing?
						("  prologue" __ constant):?
						("  personality" __ constant):?
						(" " bang_any):*
						{% d => ["function", {
							linkage:		 d[1]? d[1][1] : null,
							visibility:	 d[2]? d[2][1] : null,
							storageclass: d[3]? d[3][1] : null,
							cconv:		 d[4]? d[4][1] : null,
							retattrs:	 select(d[5], 1),
							type:		 d[7],
							name:		 d[9],
							types:		 d[11]? d[11] : null,
							unnamed_addr: d[13]? d[13][1] : null,
							fnattrs:		 d[14],
							section:		 d[15]? d[15][3] : null,
							comdat:		 d[16]? d[16][7] : null,
							align:		 d[17]? d[17][3] : null,
							gc:			 d[18]? d[18][3] : null,
							prefix:		 d[19]? d[19][3] : null,
							prologue:	 d[20]? d[20][3] : null,
							personality:	 d[21]? d[21][3] : null,
							bangs:		 select(d[22], 1)
						}] %}

function_type		->	type_any (__ parattr):* (" " variable):?						{% d => [d[0].slice(1), d[2]? d[2][1] : null, d[1].map((x) => x[1][0])] %}
function_types		->	function_types comma function_type							{% d => d[0].concat([d[2]]) %}
					 |	function_type												{% d => [d[0]] %}

cconv				->	("ccc" | "cxx_fast_tlscc" | "fastcc" | "ghccc" | "swiftcc")	{% __ %}
					 |	("preserve_allcc" | "preserve_mostcc" | "x86_vectorcallcc")	{% __ %}
					 |	("cc10" | "cc11" | "arm_apcscc" | "coldcc" | "webkit_jscc")	{% __ %}
					 |	("cc64" | "cc65" | "cc66" | "ptx_device" | "x86_stdcallcc")	{% __ %}
					 |	("cc67" | "cc68" | "cc69" | "cc70" | "cc1023" | "anyregcc")	{% __ %}
					 |	("cc71" | "cc72" | "cc75" | "msp430_intrcc" | "ptx_kernel")	{% __ %}
					 |	("cc76" | "cc77" | "cc78" | "spir_func" | "x86_64_win64cc")	{% __ %}
					 |	("cc79" | "cc80" | "arm_aapcs_vfpcc" | "intel_ocl_bicc")	{% __ %}
					 |	("x86_64_sysvcc" | "x86_fastcallcc" | "x86_thiscallcc")		{% __ %}
					 |	("arm_aapcscc" | "spir_kernel")								{% __ %}

retattr				->	("zeroext" | "signext" | "inreg" | "noalias" | "nonnull")	{% d => [d[0][0]] %}
					 |	("dereferenceable" | "deferenceable_or_null") " (" decimal
						")"															{% d => [d[0][0], d[2]] %}

parattr				->	("byval" | "inalloca" | "sret" | "nocapture" | "readonly")	{% _  %}
					 |	retattr														{% _  %}

fnattr				->	("alwaysinline" | "noredzone" | "convergent" | "norecurse") {% __ %}
					 |	("inlinehint" | "inaccessiblemem_or_argmemonly" | "sspreq")	{% __ %}
					 |	("sanitize_memory" | "jumptable" | "minsize" | "nobuiltin")	{% __ %}
					 |	("noduplicate" | "noimplicitfloat" | "builtin" | "uwtable")	{% __ %}
					 |	("nounwind" | "optnone" | "optsize" | "readnone" | "naked")	{% __ %}
					 |	("readonly" | "writeonly" | "argmemonly" | "returns_twice")	{% __ %}
					 |	("safestack" | "inaccessiblememonly" | "cold" | "noreturn")	{% __ %}
					 |	("nonlazybind" | "sanitize_thread" | "thunk" | "sspstrong")	{% __ %}
					 |	("sanitize_address" | "noinline" | "ssp")					{% __ %}
					 |	"patchable-function" eq "\"prologue-short-redirect\""		{% d => [d[0], d[2].replace(/^"|"$/g, "")] %}

constant_int		->	type_int __ decimal											{% d => [d[0], d[2]] %}
constant			->	constant_int												{% _ %}
					 |	type_array _ "[" _ (constant_int (comma constant_int):*) _ "]"
					 																{% d => [d[0], d[4][0], ...d[4][1].map((x) => x[1])] %}

bang_type			->	("dereferenceable_or_null" | "dereferenceable" | "nonnull")	{% __ %}
					 |	("invariant" | "invariant.load" | "nontemporal.group")		{% __ %}
					 |	("align")													{% __ %}

bang[X]				->	"!" $X " !" decimal											{% d => [d[1], d[3]] %}
bang_any			->	bang[bang_type]												{% _ %}

function_def		->	function_header _ "{" function_line:* "}"					{% d => [...d[0], filter(d[3])] %}
function_line		->	_ lineend													{% _( ) %}
					 |	_ instruction												{% _(1) %}
					 |	_ label														{% _ %}

instruction			->	(i_alloca | i_load | i_icmp | i_call | i_switch | i_store)	{% __ %}
					 |	(i_getelementptr | i_unreachable | i_br | i_binary)			{% __ %}

i_alloca			->	variable
						" = alloca "
						"inalloca ":?
						type_any
						(", " type_any " " decimal):?
						(", align " decimal):?
						(", addrspace(" decimal ")"):?
						{% d => ["instruction", "alloca", {
							destination: d[0],
							inalloca: !!d[2],
							type: d[3],
							types: d[4]? [d[4][1], d[4][3]] : null,
							align: d[5]? d[5][1] : null,
							addrspace: d[6]? d[6][1] : null
						}] %}

i_load				->	(i_load_normal | i_load_atomic)								{% __ %}
i_load_normal		->	variable
						" = load "
						"volatile ":?
						(type_any ", " type_any "* " temporary)
						(", align " decimal):?
						(", !nontemporal !" decimal):?
						(", !invariant.load !" decimal):?
						(", !invariant.group !" decimal):?
						(", !nonnull !" decimal):?
						(", !dereferenceable !" decimal):? # decimal might not be correct here.
						(", !dereferenceable_or_null !" decimal):? # or here.
						(", !align !" decimal):? # or here?
						{% d => ["instruction", "load", {
							destination: d[0],
							volatile: !!d[2],
							type: d[3][0],
							ptr: d[3][2],
							register: d[3][4],
							align: d[4]? d[4][1] : null,
							nontemporal: d[5]? d[5][1] : null,
							invariantLoad: d[6]? d[6][1] : null,
							invariantGroup: d[7]? d[7][1] : null,
							nonnull: d[8]? d[8][1] : null,
							dereferenceable: d[9]? d[9][1] : null,
							dereferenceable_or_null: d[10]? d[10][1] : null,
							align2: d[11]? d[11][1] : null
						}] %}

operand				->	(variable | decimal)										{% __  %}
floperand			->	(float | variable | decimal)								{% __  %}
icmp_operand		->	"null"														{% _() %}
					 |	operand														{% _   %}

i_icmp				->	variable
						" = icmp "
						("eq" | "ne" | "ugt" | "uge" | "ult" | "ule" | "sgt" | "sge" | "slt" | "sle")
						spaced[type_any]
						icmp_operand
						", "
						icmp_operand
						{% d => ["instruction", "icmp", {
							destination: d[0],
							operator: d[2][0],
							type: d[3],
							op1: d[4],
							op2: d[6]
						}] %}

i_br				->	(i_br_conditional | i_br_unconditional)						{% __ %}
i_br_unconditional	->	"br label " variable										{% d => ["instruction", "br_unconditional", { dest: d[1] }] %}
i_br_conditional	->	"br"
						spaced[type_any]
						variable
						", label "
						variable
						", label "
						variable
						{% d => ["instruction", "br_conditional", {
							type: d[1][0].slice(1),
							cond: d[2],
							iftrue: d[4],
							iffalse: d[6]
						}] %}

i_call				->	(variable " = "):?
						(("tail" | "notail" | "musttail") " "):?
						"call"
						(" " fast_math_flags):?
						(" " cconv):?
						(" " retattr):*
						_
						(call_fnty | type_any)
						_
						call_fnptrval
						"("
						typed_args_list
						")"
						# todo: fn attrs
						(_ list["#" decimal]):?
						{% d => ["instruction", "call", {
							assign: d[0]? d[0][0] : null,
							tail: d[1]? d[1][0][0] : null,
							fastmath: d[3] || null,
							cconv: d[4] || null,
							retattr: d[5]? d[5].map((x) => x[1]) : [],
							type: d[7][0],
							name: d[9],
							args: d[11],
							bundles: d[13]? d[13][1].map((x) => x[1]) : []
						}] %}

i_unreachable		->	"unreachable"												{% d => ["instruction", "unreachable", { }] %}

i_getelementptr		->	(i_getelementptr_1 | i_getelementptr_2)						{% __ %}
i_getelementptr_1	->	variable
						" = getelementptr "
						"inbounds ":?
						type_any
						", "
						type_ptr
						" "
						(variable | var_name)
						(", " "inrange ":? type_int " " (variable | decimal)):+
						{% d => ["instruction", "getelementptr", {
							destination: d[0],
							inbounds: !!d[2],
							type: d[3],
							pointerType: d[5],
							pointerValue: d[7],
							indices: d[8].map((x) => [x[2], x[4][0], !!x[1]]),
							flavor: "single"
						}] %}
i_getelementptr_2	->	variable
						" = getelementptr"
						" inbounds":?
						" {"
						types
						"}, {"
						types
						"}* "
						(variable | var_name)
						(", " "inrange ":? type_int " " (variable | decimal)):+
						{% d => ["instruction", "getelementptr", {
							destination: d[0],
							inbounds: !!d[2],
							type: d[4],
							pointerType: d[6],
							pointerValue: d[8],
							indices: d[9].map((x) => [x[2], x[4][0], !!x[1]]),
							flavor: "multi"
						}] %}

i_switch_line		->	_ type_int " " decimal ", label " variable					{% d => [d[1], d[3], d[5]] %}
					 |	_ lineend													{% _() %}
i_switch			->	"switch"
						spaced[type_int]
						operand
						", label "
						variable
						" [" 
						i_switch_line:*
						(_ "]")
						{% d => ["instruction", "switch", {
							type: d[1],
							operand: d[2],
							default: d[4],
							table: d[6]? filter(d[6]) : []
						}] %}

i_store				->	(i_store_normal | i_store_atomic)							{% __ %}
i_store_normal		->	"store"
						" volatile":?
						spaced[type_any]
						operand
						", "
						type_any
						"* "
						operand
						(", align " decimal):?
						(", " bang["nontemporal"]):?
						(", " bang["invariant.group"]):?
						{% d => ["instruction", "store", {
							volatile: !!d[1],
							storeType: d[2][0],
							storeValue: d[3],
							destinationType: d[5],
							destinationValue: d[7],
							align: d[8]? d[8][1] : null,
							nontemporal: d[9]? d[9][1][1] : null,
							invariantGroup: d[10]? d[10][1][1] : null
						}] %}

i_binary			->	(i_binary_normal | i_binary_dangerous | i_binary_fastmath)	{% __ %}
					 | 	(i_binary_exact)											{% __ %}
i_binary_normal		->	variable
						" = "
						("and" | "or" | "xor" | "urem" | "srem")
						spaced[type_intvec]
						operand
						", "
						operand
						{% d => ["instruction", "binary", {
							destination: d[0],
							operation: d[2][0],
							type: d[3][0],
							op1: d[4],
							op2: d[6],
							flavor: "normal"
						}] %}
i_binary_exact		->	variable
						" = "
						("ashr" | "lshr" | "sdiv" | "udiv")
						" exact":?
						spaced[type_intvec]
						operand
						", "
						operand
						{% d => ["instruction", "binary", {
							destination: d[0],
							operation: d[2][0],
							exact: !!d[3],
							type: d[4][0],
							op1: d[5],
							op2: d[7],
							flavor: "exact"
						}] %}
i_binary_fastmath	->	variable
						" = "
						("fadd" | "fcmp" | "fdiv" | "fmul" | "frem" | "fsub")
						(" " fast_math_flags):?
						spaced[type_floatvec]
						floperand
						", "
						floperand
						{% d => ["instruction", "binary", {
							destination: d[0],
							operation: d[2][0],
							flags: d[3]? d[3].map((x) => x[1]) : [],
							type: d[4][0],
							op1: d[5],
							op2: d[7],
							flavor: "fastmath"
						}] %}
i_dangerous			->	variable
						" = "
						("add" | "mul" | "shl" | "sub")
						" nuw":?
						" nsw":?
						spaced[type_intvec]
						operand
						", "
						operand
						{% d => ["instruction", "binary", {
							destination: d[0],
							operation: d[2][0],
							nuw: !!d[3],
							nsw: !!d[4],
							type: d[5][0],
							op1: d[6],
							op2: d[8],
							flavor: "dangerous"
						}] %}



call_fnty			->	type_any " (" commalist[type_any] ", ...":? ")"				{% compileFnty %}
call_fnptrval		->	"@" (var | string)											{% _(1) %}
#call_retattrs		->	call_retattr (" ")

fast_math_flags		->	list[fast_math_flag]											{% compileFastMathFlags %}
fast_math_flag		->	("nnan" | "ninf" | "nsz" | "arcp" | "constract" | "fast")	{% __ %}

typed_args_list		->	commalist[constant]											{% d => d[0].map((x) => x[0]) %}

constant			->	type_any (" " parattr):* " " variable						{% d => [d[0], d[3], d[1].map((x) => x[0])] %}
					 |	type_any (" " parattr):* " " const_expr						{% d => [d[0], d[3], d[1].map((x) => x[0])] %}
cst_to_type[X]		->	$X " " constant " to " type_any								{% d => [d[0], ...d[2], d[4]] %}
cst_to_types		->	("trunc" | "zext" | "sext" | "fptrunc" | "fpext" | "fptoui" | "fptosi" | "uitofp" | "sitofp" | "ptrtoint" | "inttoptr" | "bitcast" | "addrspacecast")
																					{% __ %}
const_expr			->	cst_to_type[cst_to_types]									{% d => ["expr", d[0][0], ...d[0].slice(1)] %}
					 |	getelementptr_expr											{% _ %}

getelementptr_expr	->	"getelementptr "
						"inbounds ":?
						"("
						type_any
						", "
						type_any
						"* "
						var_name
						(", " type_int " " decimal):*
						")"
						{% d => ["expr", "getelement", {
							inbounds: !!d[1],
							type: d[3],
							ptr: d[5],
							name: d[7],
							indices: d[8].map((x) => [x[1], x[3]])
						}] %}

var -> varchar:+ {%
	(d, location, reject) => {
		const identifier = d[0].join("");
		return /[0-9]/.test(identifier.charAt(0)) || special.words.indexOf(identifier) !== -1? reject : identifier;
	}
%}

varchar -> . 				{% (d, location, reject) => d[0] && special.chars.indexOf(d[0]) === -1 ? d[0] : reject %}
comment	-> ";" [^\n]:*		{% d => null %}
_		-> [\t ]:*			{% d => null %}
__		-> [\t ]:+			{% d => null %}
newline -> "\r\n" | [\r\n]	{% d => null %}

@{%/*

Potentially useful links:
	http://llvm.org/docs/LangRef.html
	http://stackoverflow.com/questions/36087319/llvm-openmp-what-is-the-meaning-of-internal-thread-local-unnamed-addr-global/36094052
	http://llvm.org/docs/WritingAnLLVMBackend.html

*/%}