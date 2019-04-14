#!/usr/bin/env ts-node

import {Node, NodeID} from "../node";
import Graph, {DJGraph, BothMap, DTree} from "../graph";
import {CFG} from "./ll2w";

export interface ASTVector {0: "vector", 1: [ASTTypeAny, null, ASTValue][]}
export interface IRConstant {0: ASTTypeAny, 1: ASTOperand | IRConstExpr, 2: IRParAttr[]}
export interface ASTTypeArray {0: "array", 1: number,     2: ASTTypeAny}
export interface ASTTypePtr   {0: "ptr",   1: ASTTypeAny, 2: number}
export interface InstBase<N extends string, M extends Object> extends Array<any> {
	0: "instruction", 1: N, 2: M & {block?: ASTFunctionBlock}
}

export type StringMap<T>   = {[key: string]: T};
export type VariableName   = string | number;
export type BlockName      = string | number;
export type IRFunctionMeta = {
	meta:  ASTFunctionMeta,
	vars:  VariableName[],
	first: BlockName,
	exit:  BlockName,
};
export type ASTFunctionBlock = [string, BlockConnectionsExtra, Instruction[]];
export type IRFunction  = ASTFunctionBlock[] & IRFunctionMeta;
export type AnyNode     = Node<any> | string | number;
export type BlockConnections = {preds: AnyNode[], in: AnyNode[], out: AnyNode[]};
export type BlockConnectionsExtra = BlockConnections & {
	read: VariableName[],
	written: VariableName[],
	assigners: {[varName: string]: Instruction},
	unreachable: boolean
}
export type BasicBlock = [
	string,
	BlockConnections, // edges
	Instruction[] // instructions
];
export type BasicBlockExtra = [string, BlockConnectionsExtra, Instruction[]];
export type FunctionExtractions = {
	functions: StringMap<IRFunction>,
	allBlocks: StringMap<BasicBlock>,
	blockOrder: [string, string][],
	functionOrder: string[];
};
export type ASTTypeAny = ASTTypeInt | ASTTypePtr | ASTTypeArray;
export type ASTTypeInt = ["int", number];
export type IRRetAttr  = ["zeroext" | "signext" | "inreg" | "noalias" | "nonnull"]
                       | ["dereferenceable" | "deferenceable_or_null", number];
export type IRParAttr  = ["byval" | "inalloca" | "sret" | "nocapture" | "readonly"] | IRRetAttr;
export type ASTFunctionType = [ASTTypeAny, IRParAttr[], ASTVariable | null];
export type ASTFunctionMeta = {
	name: string,
	type: ASTTypeAny,
	types: ASTFunctionType[],
	arity: number,
	unnamedAddr: "local_unnamed_addr" | "unnamed_addr" | null
};
export type ASTDeclaration = ["declaration", ASTFunction];
export type ASTFunction = ["function", ASTFunctionMeta, Instruction[]];
export type ASTMetadata = ["metadata", string | number, boolean, ...any[]];
export type ASTVariable = ["variable", string | number];
export type ASTGlobal   = ["global", string];
export type ASTGetElementPtrExpr = ["expr", "getelement", {
	inbounds: boolean,
	type: ASTTypeAny,
	ptr: ASTTypeAny,
	name: ASTGlobal,
	indices: [ASTTypeInt, number][]
}];
export type ASTOperand  = ASTVariable | number | ASTGlobal | ASTGetElementPtrExpr | ["null"];
export type InstBrUncond = InstBase<"br_unconditional", {dest: ASTVariable}>;
export type InstBrCond = InstBase<"br_conditional", {
	type:    ASTTypeAny,
	cond:    ASTVariable | number,
	iftrue:  ASTVariable,
	iffalse: ASTVariable,
	loop:    number | null
}>;
export type ASTSwitchLine = [ASTTypeInt, number, ASTVariable];
export type InstSwitch = InstBase<"switch", {
	type: ASTTypeAny,
	operand: ASTOperand,
	default: ASTVariable,
	table: ASTSwitchLine[]
}>;
export type IRTailType = "tail" | "notail" | "musttail";
export type IRFastMathFlag = "nnan" | "ninf" | "nsz" | "arcp" | "constract" | "fast";
export type IRCConv = "ccc" | "cxx_fast_tlscc" | "fastcc" | "ghccc" | "swiftcc" | "preserve_allcc" | "preserve_mostcc"
                    | "x86_vectorcallcc" | "cc10" | "cc11" | "arm_apcscc" | "coldcc" | "webkit_jscc" | "cc64" | "cc65"
                    | "anyregcc" | "cc71" | "cc66" | "ptx_device" | "x86_stdcallcc" | "cc67" | "cc68" | "cc69" | "cc70"
                    | "cc1023" | "cc72" | "cc75" | "msp430_intrcc" | "ptx_kernel" | "cc76" | "cc77" | "cc78"
                    | "spir_func" | "x86_64_win64cc" | "cc79" | "cc80" | "arm_aapcs_vfpcc" | "intel_ocl_bicc"
                    | "x86_64_sysvcc" | "x86_fastcallcc" | "x86_thiscallcc" | "arm_aapcscc" | "spir_kernel";
export type IRCallFnty   = [ASTTypeAny, ASTTypeAny[], boolean];
export type IRCstToTypes = "trunc" | "zext" | "sext" | "fptrunc" | "fpext" | "fptoui" | "fptosi" | "uitofp" | "sitofp"
                         | "ptrtoint" | "inttoptr" | "bitcast" | "addrspacecast";
export type IRCstToType  = [IRCstToTypes, IRConstant, ASTTypeAny];
export type IRConstExpr  = ["expr", IRCstToType];
export type InstCall = InstBase<"call", {
	assign: ASTVariable | null,
	tail: IRTailType | null,
	fastmath: IRFastMathFlag[] | null,
	cconv: IRCConv | null,
	retattr: IRRetAttr[],
	returnType: IRCallFnty | ASTTypeAny,
	name: string,
	args: IRConstant[]
}>;
export type InstUnreachable = InstBase<"unreachable", {}>;
export type ASTValue = number | ASTVector | ASTVariable | "null";
export type InstRet = InstBase<"ret", {type: ASTTypeAny, value: ASTValue}>;
export type Instruction = InstBrUncond | InstBrCond | InstSwitch | InstCall | InstUnreachable | InstRet;

export type MetadataType = {recursive: boolean, distinct: boolean, items: any[]};
export type DeclarationType = {type: ASTTypeAny, types: ASTFunctionType[], arity: number, isLocalUnnamed: boolean};

export type ComputedOperands = {read: VariableName[], written: VariableName[], assigner: VariableName | null};
export type MergeSets = BothMap<NodeID[]>;
export type CachingCFG = CFG & {_djGraph?: DJGraph, _dTree?: DTree, _mergeSets?: MergeSets};
export type UsefulLivenessData = {djGraph: DJGraph, dTree: DTree, mergeSets: MergeSets};
