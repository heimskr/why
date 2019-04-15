import {IRVariable, IRTypeAny, IROperand, IRSwitchLine, IRTailType, IRFastmathFlag, IRCConv, IRRetAttr,
        IRCallFnty, IRConstant, IRValue, VariableName, BlockName, IRFunctionBlock, IRConversionType, IRBang, IRVector,
        IRComparisonType, IRGlobal, IRGEPTriple} from "./types";

type IsTypeFn<T extends Instruction> = (x: Instruction) => x is T;
export interface InstBase<N extends string, M extends Object> extends Array<any> {
	0: "instruction", 1: N, 2: M & {block?: IRFunctionBlock}}
function isInstructionType<T extends Instruction>(type: string): IsTypeFn<T> {
	return ((x: Instruction) => x[1] == type) as IsTypeFn<T>; }

export type InstBrUncond = InstBase<"br_unconditional", {dest: IRVariable}>;
export type InstBrCond = InstBase<"br_conditional", {
	type:    IRTypeAny,
	cond:    IRVariable | number,
	iftrue:  IRVariable,
	iffalse: IRVariable,
	loop:    number | null}>;
export type InstSwitch = InstBase<"switch", {
	type: IRTypeAny,
	operand: IROperand,
	default: IRVariable,
	table: IRSwitchLine[]}>;
export type InstCall = InstBase<"call", {
	assign: IRVariable | null,
	tail: IRTailType | null,
	fastmath: IRFastmathFlag[] | null,
	cconv: IRCConv | null,
	retattr: IRRetAttr[],
	returnType: IRCallFnty | IRTypeAny,
	name: string,
	args: IRConstant[]}>;
export type InstUnreachable = InstBase<"unreachable", {}>;
export type InstRet = InstBase<"ret", {type: IRTypeAny, value: IRValue}>;
export type InstPhi = InstBase<"phi", {destination: IRVariable, type: IRTypeAny, pairs: [VariableName, BlockName][]}>;
export type InstAlloca = InstBase<"alloca", {
	destination: IRVariable,
	inalloca: boolean,
	type: IRTypeAny,
	types: [IRTypeAny, number] | null,
	align: number | null,
	addrspace: number | null}>;
export type InstConversion = InstBase<"conversion", {
	destination: IRVariable,
	sourceType: IRTypeAny
	sourceValue: IRVariable,
	destinationType: IRTypeAny,
	flavor: IRConversionType}>;
export type InstLoad = InstBase<"load", {
	destination: IRVariable,
	volatile: boolean,
	type: IRTypeAny,
	pointerType: IRTypeAny,
	pointerValue: IRVariable,
	align: number | null,
	bangs: IRBang[]}>;
export type InstBinaryNormal = InstBinaryBase<"normal", "and" | "or" | "xor" | "urem" | "srem", {}>;
export type InstBinaryExact = InstBinaryBase<"exact", "ashr" | "lshr" | "sdiv" | "udiv", {exact: boolean}>;
export type InstBinaryFastmath = InstBinaryBase<"fastmath", "fadd" | "fcmp" | "fdiv" | "fmul" | "frem" | "fsub", {
	flags: IRFastmathFlag[]}>;
export type InstBinaryDangerous = InstBinaryBase<"dangerous", "add" | "mul" | "shl" | "sub", {
	nuw: boolean,
	nsw: boolean}>;
export type InstICMP = InstBase<"icmp", {
	destination: IRVariable,
	operator: IRComparisonType,
	op1: IRVariable,
	op2: IRVariable}>;
export type InstGetElementPtr = InstBase<"getelementptr", {
	destination: IRVariable,
	inbounds: boolean,
	type: IRTypeAny,
	pointerType: IRTypeAny, // IRTypePtr?
	pointerValue: IRVariable | IRGlobal,
	indices: IRGEPTriple,
	flavor: "single" | "multi"}>;
export type InstSelect = InstBase<"select", {
	destination: IRVariable,
	conditionType: IRTypeAny,
	condition: IRVariable,
	leftType: IRTypeAny,
	leftValue: IROperand,
	rightType: IRTypeAny,
	rightValue: IROperand}>;
export type InstStore = InstBase<"store", {
	volatile: boolean,
	storeType: IRTypeAny,
	storeValue: IROperand,
	destinationType: IRTypeAny,
	destinationValue: IROperand,
	align: number | null,
	bangs: IRBang[]}>;

export type Instruction = InstBrUncond | InstBrCond | InstSwitch | InstCall | InstUnreachable | InstRet | InstPhi
                        | InstAlloca | InstConversion | InstLoad | InstBinary | InstICMP | InstGetElementPtr
                        | InstSelect | InstStore;

export const isPhi = isInstructionType<InstPhi>("phi");

export type InstBinary = InstBinaryNormal | InstBinaryExact | InstBinaryFastmath | InstBinaryDangerous;
export type InstBinaryBase<F, O, M> = InstBase<"binary", M & {
	destination: IRVariable,
	operation: O,
	type: IRVector,
	op1: IROperand,
	op2: IROperand,
	flavor: F}>;
