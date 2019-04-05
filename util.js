const _ = require("lodash"),
	  jsome = require("jsome"),
	  chalk = require("chalk");

exports.mixins = (__) => {
	if (typeof Object.values == "undefined") {
		Object.values = (obj) => Object.keys(obj).map((key) => obj[key]);
	}

	__.mixin({
		// Stringifies a long representing 8 characters.
		longString(long) {
			return long instanceof Array? long.map(__.longString) : __.chunk(long.toString(16).padStart(16, "0"), 2).map((x) => String.fromCharCode(parseInt(x.join(""), 16))).join("");
		},

		// Converts a sequence of longs to strings and splits by \0.
		longStrings(longs) {
			return __.longString(longs).join("").split("\0");
		},

		// Like _.invert, but it doesn't overwrite duplicates; instead, every value in the
		// new object is an array of all keys containing that value in the original object.
		multiInvert(obj) {
			let entries = __.entries(obj);
			return __.fromPairs(__(obj).values().uniq().value().map((val) => [val, entries.filter((item) => item[1] == val).map((item) => item[0])]));
		},

		pretty(obj) {
			return jsome.getColoredString(obj);
		},

		replaceChar(str, index, replacement) {
			console.log(`Replacing ${str[index] == "\n"? "\\n" : str[index] == " "? "space" : str[index]} with ${replacement == "\n"? "\\n" : replacement}`);
			return str.substr(0, index) + replacement + str.substr(index + replacement.length);
		},

		// Pushes a value to an array, but only if the value isn't already in the array.
		push(dest, ...vals) {
			return __.pushAll(dest, vals);
		},

		pushAll(dest, vals) {
			for (const val of vals) {
				if (!dest.includes(val)) {
					dest.push(val);
				}
			}
		},

		objectify(arr, initial = () => undefined) {
			return __.fromPairs(arr.map(v => [v, initial()]));
		},

		notSuperOrEq(a, b) {
			// If b contains any value not in a, then a ⊉ b.
			return __.some(b, v => !a.includes(v));
		},
	});
};

// Wouldn't seem particularly idiomatic to make this a lodash mixin.
exports.displayIOError = (error, filename="file", mode="r", print=console.error) => {
	const {message} = error;
	const intro = chalk.red(`Couldn't open ${chalk.bold(filename)} for ${mode == "r"? "read" : "write"}ing`) + ":";
	if (message.match(/^ENOENT:/)) {
		print(intro, `no such file.`);
	} else if (message.match(/^EACCES:/)) {
		print(intro, `permission denied.`);
	} else {
		print(intro, e.message);
	}
};

const alpha = "abcdefghijklmnopqrstuvwxyz";
const isNumeric = x => typeof x == "string" && _.every(x, y => "0123456789".includes(y));
exports.alpha = alpha;
exports.isLetter = x => typeof x == "string" && x.length == 1 && -1 < alpha.indexOf(x.toLowerCase());
exports.isNumeric = isNumeric;
exports.numerize = x => isNumeric(x)? parseInt(x) : x;
