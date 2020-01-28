#!/usr/bin/env ts-node
export const BUILTINS = {
	_int: {arity: 1},
	_rit: {arity: 1},
	_time: {arity: 1},
	_gettime: {arity: 0},
	_halt: {arity: 0},
	_prc: {arity: 1},
	_prd: {arity: 1},
	_prx: {arity: 1},
};

export const CONTROL_TRANSFERS = ["br_conditional", "br_unconditional", "ret"];
