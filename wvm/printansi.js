const _ = require("lodash");

const normal = ["black", "red", "green", "yellow", "blue", "magenta", "cyan", "white"];
const bright = ["#808080", "#f00", "#0f0", "#ff0", "#00f", "#f0f", "#0ff", "#fff"]

module.exports = function printansi(str) {
	let fmt = "", args = [], buffer = [];
	let i = 0;
	let unflushed = true;
	const add = (...a) => { buffer.push(...a); unflushed = true };
	const remove = (...a) => {
		const l = buffer.length;
		buffer = _.without(buffer, ...a);
		return unflushed = l > buffer.length || unflushed;
	};

	while (str.length && ++i < 1000) {
		const matches = str.match(/^\x1b\[([^a-z]*)([a-z])/i);
		let submatches;
		if (matches) {
			const [, inner, action] = matches;
			str = str.substr(matches[0].length);
			if (action == "m") {
				if (inner == "0") {
					buffer = [];
					unflushed = true;
				} else if (inner == "1") {
					add("font-weight:bold");
				} else if (inner == "2") {
					add("opacity:0.666");
				} else if (inner == "3") {
					add("font-style:italic");
				} else if (inner == "3") {
					if (remove("text-decoration:line-through")) {
						add("text-decoration:underline,line-through");
					} else {
						add("text-decoration:underline");
					}
				} else if (inner == "9") {
					if (remove("text-decoration:underline")) {
						add("text-decoration:underline,line-through");
					} else {
						add("text-decoration:line-through");
					}
				} else if (inner == "22") {
					remove("font-weight:bold");
				} else if (inner == "23") {
					remove("font-style:italic");
				} else if (inner == "24") {
					if (remove("text-decoration:underline,line-through")) {
						add("text-decoration:line-through");
					} else {
						remove("text-decoration:underline");
					}
				} else if (inner == "29") {
					if (remove("text-decoration:underline,line-through")) {
						add("text-decoration:underline");
					} else {
						remove("text-decoration:line-through");
					}
				} else if (inner.match(/^[349][0-7]$/)) {
					add((inner[0] == "4"? "background-" : "") + "color:" + {3: normal, 4: normal, 9: bright}[inner[0]][inner[1]]);
				} else if (submatches = inner.match(/^([34])8;5;(\d+)$/)) {
					const attr = submatches[1] == "3"? "color" : "background-color";
					const n = submatches[2];
					let color;
					if (n < 8) {
						color = normal[n];
					} else if (n < 16) {
						color = bright[n - 8];
					} else if (n < 232) {
						color = cubecolor(n - 16);
					} else if (n < 256) {
						color = "#" + _.repeat(((n - 232) * 11).toString(16).padStart(2, "0"), 3);
					}

					if (color) {
						add(attr + ":" + color);
					}
				}
			}
		} else {
			if (unflushed) {
				args.push(buffer.join(";"));
				unflushed = false;
				fmt += "%c";
			}

			fmt += str[0];
			str = str.substr(1);
		}
	}

	// console.log(fmt);
	console.log(fmt, ...args);
};

function cubecolor(n) {
	const [b, g, r] = [n % 6, Math.floor(n / 6) % 6, Math.floor(n / 36)];
	return "#" + [r, g, b].map((x) => (51 * x).toString(16).padStart(2, "0")).join("");
}

if (process.browser) {
	window.printansi = module.exports;
}
