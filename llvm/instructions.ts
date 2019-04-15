import {IRVariable, IRTypeAny, IROperand, IRSwitchLine, IRTailType, IRFastmathFlag, IRCConv, IRRetAttr,
        IRCallFnty, IRConstant, IRValue, VariableName, BlockName, IRFunctionBlock, IRConversionType, IRBang, IRVector,
        IRComparisonType, IRGlobal, IRGEPTriple} from "./types";

type IsTypeFn<T extends Instruction> = (x: Instruction) => x is T;
export interface InstBase<N extends string, M extends Object> extends Array<any> {
	0: "instruction", 1: N, 2: M & {block?: IRFunctionBlock}}
function isInstructionType<T extends Instruction>(type: string): IsTypeFn<T> {
	return ((x: Instruction) => x[1] == type) as IsTypeFn<T>; }

export type InstBrUncond = InstBase<"br_unconditional", InstBrUncondMeta>;
export type InstBrCond = InstBase<"br_conditional", InstBrCondMeta>;
export type InstSwitch = InstBase<"switch", InstSwitchMeta>;
export type InstCall = InstBase<"call", InstCallMeta>;
export type InstUnreachable = InstBase<"unreachable", {}>;
export type InstRet = InstBase<"ret", {type: IRTypeAny, value: IRValue}>;
export type InstPhi = InstBase<"phi", InstPhiMeta>;
export type InstAlloca = InstBase<"alloca", InstAllocaMeta>;
export type InstConversion = InstBase<"conversion", InstConversionMeta>;
export type InstLoad = InstBase<"load", InstLoadMeta>;

export type InstBrUncondMeta = {dest: IRVariable};
export type InstBrCondMeta = {
	type:    IRTypeAny,
	cond:    IRVariable | number,
	iftrue:  IRVariable,
	iffalse: IRVariable,
	loop:    null | number};
export type InstSwitchMeta = {
	type:    IRTypeAny,
	operand: IROperand,
	default: IRVariable,
	table:   IRSwitchLine[]};
export type InstCallMeta = {
	assign:     null | IRVariable,
	tail:       null | IRTailType,
	fastmath:   null | IRFastmathFlag[],
	cconv:      null | IRCConv,
	retattr:    IRRetAttr[],
	returnType: IRCallFnty | IRTypeAny,
	name:       string,
	args:       IRConstant[]};
export type InstPhiMeta = {
	destination: IRVariable,
	type:        IRTypeAny,
	pairs:       [VariableName, BlockName][]};
export type InstAllocaMeta = {
	destination: IRVariable,
	inalloca:    boolean,
	type:        IRTypeAny,
	types:       null | [IRTypeAny, number],
	align:       null | number,
	addrspace:   null | number};
export type InstConversionMeta = {
	destination:     IRVariable,
	sourceType:      IRTypeAny
	sourceValue:     IRVariable,
	destinationType: IRTypeAny,
	flavor:          IRConversionType};
export type InstLoadMeta = {
	destination:  IRVariable,
	volatile:     boolean,
	type:         IRTypeAny,
	pointerType:  IRTypeAny,
	pointerValue: IRVariable,
	align:        null | number,
	bangs:        IRBang[]};
export type InstICMPMeta = {
	destination: IRVariable,
	operator:    IRComparisonType,
	op1:         IRVariable,
	op2:         IRVariable};
export type InstGetElementPtrMeta = {
	destination:  IRVariable,
	inbounds:     boolean,
	type:         IRTypeAny,
	pointerType:  IRTypeAny, // IRTypePtr?
	pointerValue: IRVariable | IRGlobal,
	indices:      IRGEPTriple,
	flavor:       "single" | "multi"};
export type InstSelectMeta = {
	destination:   IRVariable,
	conditionType: IRTypeAny,
	condition:     IRVariable,
	leftType:      IRTypeAny,
	leftValue:     IROperand,
	rightType:     IRTypeAny,
	rightValue:    IROperand};
export type InstStoreMeta = {
	volatile:         boolean,
	storeType:        IRTypeAny,
	storeValue:       IROperand,
	destinationType:  IRTypeAny,
	destinationValue: IROperand,
	align:            null | number,
	bangs:            IRBang[]};

export type InstBinaryNormal = InstBinaryBase<"normal", InstBinaryNormalTypes, {}>;
export type InstBinaryExact = InstBinaryBase<"exact", InstBinaryExactTypes, InstBinaryExactBase>;
export type InstBinaryFastmath = InstBinaryBase<"fastmath", InstBinaryFastmathTypes, InstBinaryFastmathBase>;
export type InstBinaryDangerous = InstBinaryBase<"dangerous", InstBinaryDangerousTypes, InstBinaryDangerousBase>;
export type InstICMP = InstBase<"icmp", InstICMPMeta>;
export type InstGetElementPtr = InstBase<"getelementptr", InstGetElementPtrMeta>;
export type InstSelect = InstBase<"select", InstSelectMeta>;
export type InstStore = InstBase<"store", InstStoreMeta>;

type InstBinaryExactBase     = {exact: boolean};
type InstBinaryFastmathBase  = {flags: IRFastmathFlag[]};
type InstBinaryDangerousBase = {nuw: boolean, nsw: boolean};
export type InstBinaryNormalTypes    = "and"  | "or"   | "xor"  | "urem" | "srem";
export type InstBinaryExactTypes     = "ashr" | "lshr" | "sdiv" | "udiv";
export type InstBinaryFastmathTypes  = "fadd" | "fcmp" | "fdiv" | "fmul" | "frem" | "fsub";
export type InstBinaryDangerousTypes = "add"  | "mul"  | "shl"  | "sub";
export type InstBinaryExactMeta     = InstBinaryMeta<"exact",     InstBinaryExactTypes>     & InstBinaryExactBase;
export type InstBinaryFastmathMeta  = InstBinaryMeta<"fastmath",  InstBinaryFastmathTypes>  & InstBinaryFastmathBase;
export type InstBinaryDangerousMeta = InstBinaryMeta<"dangerous", InstBinaryDangerousTypes> & InstBinaryDangerousBase;

export type Instruction = InstBrUncond | InstBrCond | InstSwitch | InstCall | InstUnreachable | InstRet | InstPhi
                        | InstAlloca | InstConversion | InstLoad | InstBinary | InstICMP | InstGetElementPtr
                        | InstSelect | InstStore;

export const isBrCond        = isInstructionType<InstBrCond>("br_conditional");
export const isBrUncond      = isInstructionType<InstBrUncond>("br_unconditional");
export const isSwitch        = isInstructionType<InstSwitch>("switch");
export const isCall          = isInstructionType<InstCall>("call");
export const isUnreachable   = isInstructionType<InstUnreachable>("unreachable");
export const isRet           = isInstructionType<InstRet>("ret");
export const isPhi           = isInstructionType<InstPhi>("phi");
export const isAlloca        = isInstructionType<InstAlloca>("alloca");
export const isConversion    = isInstructionType<InstConversion>("conversion");
export const isLoad          = isInstructionType<InstLoad>("load");
export const isBinary        = isInstructionType<InstBinary>("binary");
export const isICMP          = isInstructionType<InstICMP>("icmp");
export const isGetElementPtr = isInstructionType<InstGetElementPtr>("getelementptr");
export const isSelect        = isInstructionType<InstSelect>("select");
export const isStore         = isInstructionType<InstStore>("store");

export type InstBinary = InstBinaryNormal | InstBinaryExact | InstBinaryFastmath | InstBinaryDangerous;
export type InstBinaryBase<F, O, M> = InstBase<"binary", M & InstBinaryMeta<F, O>>;
type InstBinaryMeta<F, O> = {
	destination: IRVariable,
	operation: O,
	type: IRVector,
	op1: IROperand,
	op2: IROperand,
	flavor: F};
