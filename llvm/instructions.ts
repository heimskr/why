import {InstBase, IRVariable, IRTypeAny, IROperand, IRSwitchLine, IRTailType, IRFastMathFlag, IRCConv, IRRetAttr,
        IRCallFnty, IRConstant, IRValue, VariableName, BlockName, IRFunctionBlock, IRConversionType} from "./types";

export type Instruction = InstBrUncond | InstBrCond | InstSwitch | InstCall | InstUnreachable | InstRet | InstPhi
                        | InstAlloca | InstConversion;

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
	fastmath: IRFastMathFlag[] | null,
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

export const isPhi = isInstructionType<InstPhi>("phi");
