#!/usr/bin/env ts-node
import * as _ from "lodash";
import jsome from "jsome";
const chalk = require("chalk");
const long = require("long");

type Long = typeof long.UZERO;

// Stringifies a long representing 8 characters.
export function longString(long: Long[] | Long): string {
	return (long instanceof Array? long.map(longString)
		                         : _.chunk(long.toString(16).padStart(16, "0"), 2)
		                            .map(x => String.fromCharCode(parseInt(x.join(""), 16)))).join("");
}

// Converts a sequence of longs to strings and splits by \0.
export function longStrings(longs) {
	return longString(longs).split("\0");
}

// Like _.invert, but it doesn't overwrite duplicates; instead, every value in the
// new object is an array of all keys containing that value in the original object.
export function multiInvert(obj) {
	let entries = _.entries(obj);
	return _.fromPairs(_(obj).values().uniq().value().map((val) => [
		val, entries.filter(([, x]) => x == val).map(([y]) => y)
	]));
}

export function pretty(obj) {
	return jsome.getColoredString(obj);
}

export function replaceChar(str, index, replacement) {
	console.log(`Replacing ${str[index] == "\n"? "\\n" : str[index] == " "? "space" : str[index]} ` +
	            `with ${replacement == "\n"? "\\n" : replacement}`);
	return str.substr(0, index) + replacement + str.substr(index + replacement.length);
}

// Pushes a value to an array, but only if the value isn't already in the array.
export function push<T>(dest: T[], ...vals: T[]) {
	return pushAll(dest, vals);
}

export function pushAll<T>(dest: T[], vals: T[]) {
	for (const val of vals) {
		if (!dest.includes(val)) {
			dest.push(val);
		}
	}
}

export function objectify<T>(arr: T[], initial: () => any = () => undefined): Object {
	return _.fromPairs(arr.map(v => [v, initial()]));
}

export function notSuperOrEq(a: any[], b: any[]): boolean {
	// If b contains any value not in a, then a ⊉ b.
	return _.some(b, v => !a.includes(v));
}

_.mixin({
	longString,
	longStrings,
	multiInvert,
	pretty,
	replaceChar,
	push,
	pushAll,
	objectify,
	notSuperOrEq
});

// Wouldn't seem particularly idiomatic to make this a lodash mixin.
export function displayIOError(error: Error, filename: string = "file", mode: string = "r",
                               print: (...args: any[]) => void = console.error) {
	const {message} = error;
	const intro = chalk.red(`Couldn't open ${chalk.bold(filename)} for ${mode == "r"? "read" : "write"}ing`) + ":";
	if (message.match(/^ENOENT:/)) {
		print(intro, `no such file.`);
	} else if (message.match(/^EACCES:/)) {
		print(intro, `permission denied.`);
	} else {
		print(intro, message);
	}
}

// export function pushAll(dest, vals) {
// 	for (const val of vals) {
// 		if (!dest.includes(val)) {
// 			dest.push(val);
// 		}
// 	}
// }

export const alpha = "abcdefghijklmnopqrstuvwxyz";
export function isNumeric(x: any): boolean {
	return typeof x == "number" || typeof x == "string" && _.every(x, y => "0123456789".includes(y));
}

export function isLetter(x: any): boolean {
	return typeof x == "string" && x.length == 1 && -1 < alpha.indexOf(x.toLowerCase());
}

export function numerize(x: string): string | number {
	return isNumeric(x)? parseInt(x) : x;
}
