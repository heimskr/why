module.exports = (_) => {
	if (typeof Object.values == "undefined") {
		Object.values = (obj) => Object.keys(obj).map((key) => obj[key]);
	};

	_.mixin({
		// Stringifies a long representing 8 characters.
		longString(long) {
			return long instanceof Array? long.map(_.longString) :  _.chunk(long.toString(16).padStart(16, "0"), 2).map((x) => String.fromCharCode(parseInt(x.join(""), 16))).join("");
		},

		// Converts a sequence of longs to strings and splits by \0.
		longStrings(longs) {
			return _.longString(longs).join("").split("\0");
		},

		// Like _.invert, but it doesn't overwrite duplicates; instead, every value in the
		// new object is an array of all keys containing that value in the original object.
		multiInvert(obj) {
			let entries = _.entries(obj);
			return _.fromPairs(_(obj).values().uniq().value().map((val) => [val, entries.filter((item) => item[1] == val).map((item) => item[0])]));
		}
	});
};