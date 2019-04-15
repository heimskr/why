#!/usr/bin/env ts-node

import {Node, NodeID, DJGraph, BothMap, DTree} from "../graph";
import {CFG} from "./ll2w";

import {Instruction} from "./instructions";
export * from "./instructions";

export interface IRConstant  {0: IRTypeAny, 1: IROperand | IRConstExpr, 2: IRParAttr[]}
export interface IRVector    {0: "vector", 1: [IRTypeAny, null /*???*/, IRValue][]}
export interface IRTypeArray {0: "array",  1: number,     2: IRTypeAny}
export interface IRTypePtr   {0: "ptr",    1: IRTypeAny, 2: number}

export type StringMap<T>   = {[key: string]: T};
export type VariableName   = string | number;
export type BlockName      = string | number;
export type FunctionName   = string;
export type IRFunctionMeta = {
	meta:  LLVMFunctionMeta,
	vars:  VariableName[],
	first: BlockName,
	exit:  BlockName,
};
export type IRFunctionBlock = [BlockName, BlockConnectionsExtra, Instruction[]];
export type IRFunction = IRFunctionBlock[] & IRFunctionMeta;
export type AnyNode = Node<any> | NodeID;
export type BlockConnections = {preds: AnyNode[], in: AnyNode[], out: AnyNode[]};
export type BlockConnectionsExtra = BlockConnections & {
	read: VariableName[],
	written: VariableName[],
	assigners: BothMap<Instruction>,
	unreachable: boolean}
export type BasicBlock = [
	BlockName,
	BlockConnections, // edges
	Instruction[] // instructions
];
export type BasicBlockExtra = [BlockName, BlockConnectionsExtra, Instruction[]];
export type FunctionExtractions = {
	functions: StringMap<IRFunction>,
	allBlocks: StringMap<BasicBlock>,
	blockOrder: [FunctionName, BlockName][],
	functionOrder: FunctionName[]};
export type IRTypeAny = IRTypeInt | IRTypePtr | IRTypeArray;
export type IRTypeInt = ["int", number];
export type IRRetAttr  = ["zeroext" | "signext" | "inreg" | "noalias" | "nonnull"]
                       | ["dereferenceable" | "deferenceable_or_null", number];
export type IRParAttr  = ["byval" | "inalloca" | "sret" | "nocapture" | "readonly"] | IRRetAttr;
export type IRFunctionType = [IRTypeAny, IRParAttr[], IRVariable | null];
export type LLVMFunctionMeta = {
	name: FunctionName,
	type: IRTypeAny,
	types: IRFunctionType[],
	arity: number,
	unnamedAddr: "local_unnamed_addr" | "unnamed_addr" | null};
export type ASTDeclaration = ["declaration", ASTFunction];
export type ASTFunction = ["function", LLVMFunctionMeta, Instruction[]];
export type IRMetadata = ["metadata", VariableName, boolean, ...any[]];
export type IRVariable = ["variable", VariableName];
export function isASTVariable(x: any): x is IRVariable { return x instanceof Array && x[0] == "variable"; }
export type IRGlobal   = ["global", string];
export type IRGetElementPtrExpr = ["expr", "getelement", {
	inbounds: boolean,
	type: IRTypeAny,
	ptr: IRTypeAny,
	name: IRGlobal,
	indices: [IRTypeInt, number][]}];
export type IROperand  = IRVariable | number | IRGlobal | IRGetElementPtrExpr | ["null"];
export type IRTailType = "tail" | "notail" | "musttail";
export type IRCConv = "ccc" | "cxx_fast_tlscc" | "fastcc" | "ghccc" | "swiftcc" | "preserve_allcc" | "preserve_mostcc"
                    | "x86_vectorcallcc" | "cc10" | "cc11" | "arm_apcscc" | "coldcc" | "webkit_jscc" | "cc64" | "cc65"
                    | "anyregcc" | "cc71" | "cc66" | "ptx_device" | "x86_stdcallcc" | "cc67" | "cc68" | "cc69" | "cc70"
                    | "cc1023" | "cc72" | "cc75" | "msp430_intrcc" | "ptx_kernel" | "cc76" | "cc77" | "cc78"
                    | "spir_func" | "x86_64_win64cc" | "cc79" | "cc80" | "arm_aapcs_vfpcc" | "intel_ocl_bicc"
                    | "x86_64_sysvcc" | "x86_fastcallcc" | "x86_thiscallcc" | "arm_aapcscc" | "spir_kernel";

export type IRConversionType = "bitcast" | "fpext" | "fptosi" | "fptoui" | "fptrunc" | "inttoptr" | "ptrtoint" | "sext"
                             | "sitofp"  | "trunc" | "uitofp" | "zext";
export type IRCallFnty   = [IRTypeAny, IRTypeAny[], boolean];
export type IRCstToTypes = "trunc" | "zext" | "sext" | "fptrunc" | "fpext" | "fptoui" | "fptosi" | "uitofp" | "sitofp"
                         | "ptrtoint" | "inttoptr" | "bitcast" | "addrspacecast";
export type IRCstToType  = [IRCstToTypes, IRConstant, IRTypeAny];
export type IRConstExpr  = ["expr", IRCstToType];
export type IRValue = number | IRVector | IRVariable | "null";
export type IRSwitchLine = [IRTypeInt, number, IRVariable];
export type IRBangType = "dereferenceable_or_null" | "dereferenceable" | "nonnull" | "invariant" | "invariant.load"
                       | "nontemporal.group" | "align" | "tbaa" | "range" | "llvm.loop"
                       | "llvm.mem.parallel_loop_access";
export type IRBang = [IRBangType, number];
export type IRBinaryType = "and" | "or" | "xor" | "urem" | "srem" | "ashr" | "lshr" | "sdiv" | "udiv" | "fadd" | "fcmp"
                         | "fdiv" | "fmul" | "frem" | "fsub" | "add" | "mul" | "shl" | "sub";
export type IRBinaryFlavor = "normal" | "exact" | "fastmath" | "dangerous";
export type IRFastmathFlag = "nnan" | "ninf" | "nsz" | "arcp" | "constract" | "fast";
export type IRComparisonType = "eq" | "ne" | "ugt" | "uge" | "ult" | "ule" | "sgt" | "sge" | "slt" | "sle";

export type LabelComment = ["label_c", BlockName, BlockName[]]; // [, name of following block, names of preds]
export type Label = ["label", BlockName];
export function isLabelComment(x: any[]): x is LabelComment { return x[0] == "label_c"; }
export function isLabel(x: any[]): x is Label { return x[0] == "label"; }

export type MetadataType = {recursive: boolean, distinct: boolean, items: any[]};
export type DeclarationType = {type: IRTypeAny, types: IRFunctionType[], arity: number, isLocalUnnamed: boolean};

export type ComputedOperands = {read: VariableName[], written: VariableName[], assigner: VariableName | null};
export type MergeSets = BothMap<NodeID[]>;
export type CachingCFG = CFG & {_djGraph?: DJGraph, _dTree?: DTree, _mergeSets?: MergeSets};
export type UsefulLivenessData = {djGraph: DJGraph, dTree: DTree, mergeSets: MergeSets};
