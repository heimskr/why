@{%
"use strict";

const { uniq: unique, some } = require("lodash");

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
const compilePtr = (type) => type[0] == "ptr"? ["ptr", type[1], type[2] + 1] : ["ptr", type, 1];

const parseLabelComment = (d, l, r) => {
	const m = d[1].match(/^<label>:(\d+): *; preds = (%[^,]+(, %[^,]+)*)$/);
	return m? ["label_c", m[1], m[2].split(/, /).map((x) => x.substr(1))] : null;
};

const parseLabel = (d, l, r) => {
	const m = d[3].match(/^preds = (%[^,]+(, %[^,]+)*)$/);
	return m? ["label_c", d[1][1], m[1].split(/, /).map((x) => x.substr(1))] : d[1];
};

%}

@include "strings.ne"

main -> item:+																		{% d => filter(d[0]) %}
item ->	_ lineend																	{% _() %}
	  |	_
	  	(type_any | source_filename | target | struct | function_def | declaration |
		 global_def | attributes | metadata_def)									{% __(1, 0) %}

lineend				->	(comment newline | newline) 								{% _( ) %}
spaced[X]			->	" " $X " "													{% _(1) %}
list[X]				->	$X (" " $X):*												{% d => [d[0][0], ...d[1].map((x) => x[1][0])] %}
commalist[X]		->	$X (", " $X):*												{% d => [d[0][0], ...d[1].map((x) => x[1][0])] %}
pars[X]				->	"(" $X ")"													{% _(1) %}

cstring				->	"c" string													{% _(1) %}
float				->	"-":? [0-9]:+ "." [0-9]:+									{% d => parseFloat((d[0] || "") + d[1].join("") + d[2] + d[3].join("")) %}
decimal				->	"-":? [0-9]:+												{% d => parseInt((d[0] || "") + d[1].join("")    ) %}
					 |	"true"														{% d => 1 %}
					 |	"false"														{% d => 0 %}
natural				->	[1-9] [0-9]:*												{% d => parseInt(d[0] + d[1].join("")) %}
vector				->	"<" commalist[type_any " " value] ">"						{% d => ["vector", d[1][0]] %}

value				->	(float | decimal | vector | variable)				{% __ %}

source_filename		->	"source_filename = " string									{% d => ["source_filename", d[1]] %}

target				->	"target " targetname " = " string							{% d => ["target", d[1], d[3]] %}
targetname			->	("datalayout" | "triple")									{% __(0, 0) %}

attributes			->	"attributes #" decimal " = { " list[attribute] " }"			{% d => ["attributes", d[1], d[3]] %}
attribute			->	string "=" string											{% d => [d[0], d[2]] %}
					 |	string														{% d => [d[0], true] %}
					 |  fnattr														{% d => [d[0]] %}

metadata_def		-> "!"
						(decimal | var)
						" ="
						" distinct":?
						" !{"
						commalist[metadata]
						"}"															{% d => ["metadata", d[1][0], !!d[3], ...d[5]] %}
metadata			->	constant													{% d => d[0].slice(0, 2) %}
					 |	"!" string													{% d => d[1] %}
					 |	"null"														{% d => null %}
					 |	"!" decimal													{% d => d[1] %}


variable			->	"%" (var | decimal | string)								{% d => ["variable", d[1][0]] %}

type_struct			->	"%struct." var												{% d => ["struct", d[1]] %}
struct				->	type_struct " = type opaque"								{% d => [...d[0], "opaque"] %}
					 |	type_struct " = type { " types " }"							{% d => [...d[0], d[2]] %}

types				->	commalist[type_any]											{% _ %}

type_int			->	"i" natural													{% d => ["int", d[1]] %}
type_float			->	("half" | "float" | "double" | "fp128" | "x86_fp80" |
					     "ppc_fp128")												{% d => ["float", d[0][0]] %}
type_array			->	"[" _ natural _ "x" _ type_any "]"							{% d => ["array", d[2], d[6]] %}
type_vector			->	"<" natural " x " vector_type ">"							{% d => ["vector", d[1], d[3]] %}
vector_type			->	(type_int | type_ptr)										{% d => d[0][0] %}
					 |	"float"														{% d => ["float"] %}
type_ptr			->	type_any "*"												{% d => compilePtr(d[0]) %}
type_void			->	"void"														{% d => ["void"] %}
type_function		->	type_any _ "(" types ", ...":? ")*"							{% d => ["function", d[0], d[3], !!d[4]] %}
					 |	type_any _ "(...)*"											{% d => ["function", d[0], [], true] %}
type_any			->	(type_void | type_ptr | type_array | type_int | type_float |
						 type_function | type_vector | type_struct)					{% d => d[0][0] %}
type_intvec			->	type_int													{% _ %}
					 |	"<" natural " x " type_int ">"								{% d => ["vector", d[1], d[3]] %}
type_floatvec		->	type_float													{% _ %}
					 |	"<" natural " x " type_float ">"							{% d => ["vector", d[1], d[3]] %}

global				->	"@" var														{% _(1) %}
label				->	var ":"														{% d => ["label", d[0]] %}
global_def			->	global
						" ="
						(" " linkage):?
						(" " visibility):?
						(" " dll_storage_class):?
						(" " thread_local):?
						(" " unnamed_addr):?
						(" " addrspace):?
						(" externally_initialized"):?
						(" " global_constant):?
						" "
						type_any
						(" " initial_value):?
						(", section " string):?
						(", comdat $" var):?
						(", align " decimal):?
						#// not sure what "(, !name !N)*" is supposed to mean, but it doesn't to be used in various things I found online, so whatever ¯\_(ツ)_/¯
						{% d => ["global constant", {
							name:                    d[ 0],
							linkage:                 d[ 2]? d[ 2][1] : null,
							visibility:              d[ 3]? d[ 3][1] : null,
							storageClass:            d[ 4]? d[ 4][1] : null,
							threadlocal:             d[ 5] || null,
							unnamedAddr:             d[ 6]? d[ 6][1] : null,
							addrspace:               d[ 7]? d[ 7][1] : null,
							externallyInitialized: !!d[ 8],
							globalConstant:          d[ 9]? d[ 9][1] : null,
							type:                    d[11],
							initialValue:            d[12]? d[12][1] : null,
							section:                 d[13]? d[13][1] : null,
							comdat:                  d[14]? d[14][1] : null,
							align:                   d[15]? d[15][1] : null
						}] %}

linkage				->	("private" | "appending" | "available_externally" | "weak" |
						 "linkonce" | "extern_weak" | "linkonce_odr" | "weak_odr"  |
						 "external" | "common" | "internal")						{% __ %}
visibility			->	"default"													{% d => 0 %}
					 |	"hidden"													{% d => 1 %}
					 |	"protected"													{% d => 2 %}
dll_storage_class	->	("dllimport" | "dllexport")									{% __ %}
thread_local		->	"thread_local(" ("localdynamic" | "initialexec" | "localexec") ")"
																					{% d => d[1][0] %}
unnamed_addr		->	("local_unnamed_addr" | "unnamed_addr")						{% __ %}
addrspace			->	"addrspace(" decimal ")"									{% _(1) %}
global_constant		->	("global" | "constant")										{% __ %}
initial_value		->	cstring														{% d => ["string",  d[0]] %}
					 |	float														{% d => ["float",   d[0]] %}
					 |	decimal														{% d => ["decimal", d[0]] %}
					 |	"zeroinitializer"											{% d => ["zero"] %}

type_list			-> commalist[function_type] ", ...":?							{% d => [d[0], !!d[1]] %}
					 | "..."														{% d => [[], true] %}
function_header		->	(" " linkage):?
						(" " visibility):?
						(" " dll_storage_class):?
						(" " cconv):?
						(" " retattr):*
						" "
						type_any
						" "
						function_name
						"("
						(type_list | _)
						")"
						(" " unnamed_addr):?
						((" " fnattr):+ | " #" decimal):?
						("  section " string):?
						("  comdat $" var):?
						("  align " decimal):?
						("  gc " string):?
						("  prefix " constant):? # what about that "@md" thing?
						("  prologue " constant):?
						("  personality " constant):?
						(" " bang_any):*
						{% d => ["function", {
							linkage:      d[0]? d[0][1] : null,
							visibility:   d[1]? d[1][1] : null,
							storageClass: d[2]? d[2][1] : null,
							cconv:        d[3]? d[3][1] : null,
							retattrs:     select(d[4], 1),
							type:         d[6],
							name:         d[8],
							types:        d[10][0]? d[10][0][0] : null,
							varargs:      d[10][0]? d[10][0][1] : null,
							unnamedAddr:  d[12]? d[12][1] : null,
							fnattrs:      d[13],
							section:      d[14]? d[14][1] : null,
							comdat:       d[15]? d[15][1] : null,
							align:        d[16]? d[16][1] : null,
							gc:           d[17]? d[17][1] : null,
							prefix:       d[18]? d[18][1] : null,
							prologue:     d[19]? d[19][1] : null,
							personality:  d[20]? d[20][1] : null,
							bangs:        select(d[21], 1)
						}] %}

declaration			->	"declare" function_header									{% d => ["declaration", d[1]] %}
function_type		->	type_any (_ parattr):* (" " variable):?
function_def		->	"define" function_header " {" function_line:* "}"			{% d => [...d[1], filter(d[3])] %}

function_line		->	_ label " ":* comment newline								{% parseLabel %}
					 |	_ comment newline											{% parseLabelComment %}
					 |	_ newline													{% _( ) %}
					 |	_ instruction												{% _(1) %}

cconv				->	("ccc" | "cxx_fast_tlscc" | "fastcc" | "ghccc" | "swiftcc" |
						 "preserve_allcc" | "preserve_mostcc" | "x86_vectorcallcc" |
						 "cc10" | "cc11" | "arm_apcscc" | "coldcc" | "webkit_jscc" |
						 "cc64" | "cc65" | "cc66" | "ptx_device" | "x86_stdcallcc" |
						 "cc67" | "cc68" | "cc69" | "cc70" | "cc1023" | "anyregcc" |
						 "cc71" | "cc72" | "cc75" | "msp430_intrcc" | "ptx_kernel" |
						 "cc76" | "cc77" | "cc78" | "spir_func" | "x86_64_win64cc" |
						 "cc79" | "cc80" | "arm_aapcs_vfpcc" | "intel_ocl_bicc"    |
						 "x86_64_sysvcc" | "x86_fastcallcc" | "x86_thiscallcc"     |
						 "arm_aapcscc" | "spir_kernel")								{% __ %}

retattr				->	("zeroext" | "signext" | "inreg" | "noalias" | "nonnull")	{% d => [d[0][0]] %}
					 |	("dereferenceable" | "deferenceable_or_null") " (" decimal
						")"															{% d => [d[0][0], d[2]] %}

parattr				->	("byval" | "inalloca" | "sret" | "nocapture" | "readonly")	{% _  %}
					 |	retattr														{% _  %}

fnattr				->	("alwaysinline" | "noredzone" | "convergent" | "norecurse" |
						 "inlinehint" | "inaccessiblemem_or_argmemonly" | "sspreq" |
						 "sanitize_memory" | "jumptable" | "minsize" | "nobuiltin" |
						 "noduplicate" | "noimplicitfloat" | "builtin" | "uwtable" |
						 "nounwind" | "optnone" | "optsize" | "readnone" | "naked" |
						 "readonly" | "writeonly" | "argmemonly" | "returns_twice" |
						 "safestack" | "inaccessiblememonly" | "cold" | "noreturn" |
						 "nonlazybind" | "sanitize_thread" | "thunk" | "sspstrong" |
						 "sanitize_address" | "noinline" | "ssp")					{% _ %}
					 |	"alignstack(" decimal ")"									{% d => d["alignstack", d[1]] %}
					 |	"allocsize(" decimal (", " decimal):? ")"					{% d => d["allocsize", d[1], d[2]? d[2][1] : null] %}
					 |	"patchable-function=\"prologue-short-redirect\""			{% d => [d[0], d[2].replace(/^"|"$/g, "")] %}

bang_type			->	("dereferenceable_or_null" | "dereferenceable" | "nonnull")	{% __ %}
					 |	("invariant" | "invariant.load" | "nontemporal.group")		{% __ %}
					 |	("align" | "tbaa")											{% __ %}
					 |	llvm_bang													{% _ %}

llvm_bang			->	"llvm." ("loop" | "mem.parallel_loop_access")				{% d => d[0] + d[1][0] %}

bang[X]				->	"!" $X " !" decimal											{% d => [d[1], d[3]] %}
bang_any			->	bang[bang_type]												{% _ %}
instruction			->	(i_alloca | i_load | i_icmp | i_call | i_switch | i_store  |
						 i_getelementptr | i_unreachable | i_br | i_binary | i_phi |
						 i_conversion | i_ret)										{% __ %}

i_alloca			->	variable
						" = alloca "
						"inalloca ":?
						type_any
						(", " type_any " " decimal):?
						(", align " decimal):?
						(", addrspace(" decimal ")"):?
						{% d => ["instruction", "alloca", {
							destination: d[0],
							inalloca:  !!d[2],
							type:        d[3],
							types:       d[4]? [d[4][1], d[4][3]] : null,
							align:       d[5]? d[5][1] : null,
							addrspace:   d[6]? d[6][1] : null
						}] %}

i_load				->	variable
						" = load "
						"volatile ":?
						(type_any ", " type_any "* " (variable | global))
						(", align " decimal):?
						(", " commalist[bang_any]):?
						{% d => ["instruction", "load", {
							destination:           d[0],
							volatile:            !!d[2],
							type:                  d[3][0],
							pointerType:           d[3][2],
							pointerValue:          d[3][4][0],
							align:                 d[4]? d[4][1] : null,
							bangs:                 d[5]? d[5][1] : []
						}] %}

operand				->	(variable | decimal)										{% __  %}
					 |	"null"														{% d => ["null"] %}
floperand			->	(float | variable | decimal)								{% __  %}

i_icmp				->	variable
						" = icmp "
						("eq" | "ne" | "ugt" | "uge" | "ult" | "ule" | "sgt" | "sge" | "slt" | "sle")
						spaced[type_any]
						operand
						", "
						operand
						{% d => ["instruction", "icmp", {
							destination: d[0],
							operator:    d[2][0],
							type:        d[3],
							op1:         d[4],
							op2:         d[6]
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
						(", !llvm.loop !" decimal):?
						{% d => ["instruction", "br_conditional", {
							type:    d[1][0],
							cond:    d[2],
							iftrue:  d[4],
							iffalse: d[6],
							loop:    d[7]? d[7][1] : null
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
						function_name
						"("
						(commalist[constant] | _)
						")"
						# todo: fn attrs
						(_ list["#" decimal]):?
						{% d => ["instruction", "call", {
							assign:     d[ 0]? d[0][0] : null,
							tail:       d[ 1]? d[1][0][0] : null,
							fastmath:   d[ 3] || null,
							cconv:      d[ 4] || null,
							retattr:    d[ 5]? d[5].map((x) => x[1]) : [],
							returnType: d[ 7][0],
							name:       d[ 9][0],
							args:       d[11][0]? d[11][0] : [],
							bundles:    d[13]?    d[13][1].map((x) => x[1]) : []
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
						(variable | global)
						(", " "inrange ":? type_int " " (variable | decimal)):+
						{% d => ["instruction", "getelementptr", {
							destination: d[0],
							inbounds: !!d[2],
							type: d[3],
							pointerType: d[5],
							pointerValue: d[7][0],
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
						(variable | global)
						(", " "inrange ":? type_int " " (variable | decimal)):+
						{% d => ["instruction", "getelementptr", {
							destination: d[0],
							inbounds: !!d[2],
							type: d[4],
							pointerType: d[6],
							pointerValue: d[8][0],
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

i_store				->	"store"
						" volatile":?
						spaced[type_any]
						operand
						", "
						type_any
						"* "
						operand
						(", align " decimal):?
						(", " commalist[bang_any]):?
						{% d => ["instruction", "store", {
							volatile:       !!d[1],
							storeType:        d[2][0],
							storeValue:       d[3],
							destinationType:  d[5],
							destinationValue: d[7],
							align:            d[8]? d[8][1] : null,
							bangs:            d[9]? d[9][1] : []
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
i_binary_dangerous	->	variable
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

i_phi_pair			->	"[ " floperand ", " variable " ]"							{% d => [d[1], d[3]] %}
					 |	"[ undef, " variable " ]"									{% d => [["undefined"], d[1]] %}
					 |	"[ null, " variable " ]"									{% d => [["null"], d[1]] %}
i_phi				->	variable
						" = phi "
						type_any
						" "
						commalist[i_phi_pair]
						{% d => ["instruction", "phi", {
							destination: d[0],
							type: d[2],
							pairs: d[4]
						}] %}

i_conversion		->	(i_conversion_ext)											{% __ %}
i_conversion_ext	->	variable
						" = "
						("bitcast" | "fpext" | "fptosi" | "fptoui" | "fptrunc" | "inttoptr" | "ptrtoint" | "sext" | "sitofp" | "trunc" | "uitofp" | "zext")
						" "
						type_any
						" "
						value
						" to "
						type_any
						{% d => ["instruction", "conversion", {
							destination: d[0],
							sourceType: d[4],
							sourceValue: d[6],
							destinationType: d[8],
							flavor: d[2][0],
						}] %}

i_ret				->	"ret " type_any " " value									{% d => ["instruction", "ret", { type: d[1], value: d[3] }] %}
					 |	"ret void"													{% d => ["instruction", "ret", { type: ["void"], value: null }] %}



call_fnty			->	type_any " (" commalist[type_any] ", ...":? ")"				{% d => [d[0], d[2][0], !!d[3]] %}
					 |	type_any " (...)"											{% d => [d[0], [], true] %}
function_name		->	"@" (var | string)											{% d => d[1][0] %}

fast_math_flags		->	list[fast_math_flag]											{% compileFastMathFlags %}
fast_math_flag		->	("nnan" | "ninf" | "nsz" | "arcp" | "constract" | "fast")	{% __ %}

constant			->	type_any (" " parattr):* " " (operand | const_expr)			{% d => [d[0], d[3][0], d[1].map((x) => x[0])] %}
cst_to_type[X]		->	$X " " constant " to " type_any								{% d => [d[0], ...d[2], d[4]] %}
cst_to_types		->	("trunc" | "zext" | "sext" | "fptrunc" | "fpext" | "fptoui" | "fptosi" | "uitofp" | "sitofp" | "ptrtoint" | "inttoptr" | "bitcast" | "addrspacecast")
																					{% __ %}
const_expr			->	cst_to_type[cst_to_types]									{% d => ["expr", d[0]] /* need to test this. */ %}
					 |	getelementptr_expr											{% _ %}

getelementptr_expr	->	"getelementptr "
						"inbounds ":?
						"("
						type_any
						", "
						type_any
						"* "
						global
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

varchar -> . 							{% (d, location, reject) => d[0] && special.chars.indexOf(d[0]) === -1 ? d[0] : reject %}
comment	-> ";" _ ([^\n\t ] [^\n]:*):?	{% d => d[2]? d[2][0] + d[2][1].join(""): "" %}
_		-> [\t ]:*						{% d => null %}
__		-> [\t ]:+						{% d => null %}
newline -> "\r\n" | [\r\n]				{% d => null %}

@{%/*

Potentially useful links:
	http://llvm.org/docs/LangRef.html
	http://stackoverflow.com/questions/36087319/llvm-openmp-what-is-the-meaning-of-internal-thread-local-unnamed-addr-global/36094052
	http://llvm.org/docs/WritingAnLLVMBackend.html

*/%}