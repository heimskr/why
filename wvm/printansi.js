const _ = require("lodash");

const normal = ["black", "red", "green", "yellow", "blue", "magenta", "cyan", "white"];
const bright = ["#808080", "#f00", "#0f0", "#ff0", "#00f", "#f0f", "#0ff", "#fff"]

module.exports = function printansi(str) {
	let fmt = "", args = [], buffer = [];
	let i = 0;
	let unflushed = true;
	const remove = (...a) => buffer = _.without(buffer, ...a);
	while (str.length && ++i < 1000) {
		const matches = str.match(/^\x1b\[([^a-z]*)([a-z])/i);
		let submatches;
		if (matches) {
			const [, inner, action] = matches;
			str = str.substr(matches[0].length);
			if (action == "m") {
				if (inner == "1") {
					buffer.push("font-weight:bold");
					unflushed = true;
				} else if (inner == "0") {
					buffer = [];
					unflushed = true;
				} else if (inner.match(/^[349][0-7]$/)) {
					buffer.push((inner[0] == "4"? "background-" : "") + "color:" + {3: normal, 4: normal, 9: bright}[inner[0]][inner[1]]);
					unflushed = true;
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
						buffer.push(attr + ":" + color);
						unflushed = true;
					}
				}
			}
		} else {
			if (unflushed) {
				args.push(buffer.join(";"));
				fmt += "%c";
				unflushed = false;
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
	const {exports: printansi} = module;
	window.printansi = printansi;
	window.cubecolor = cubecolor;
	// printansi("\x1b[1mfoo\x1b[0mbar\x1b[32mbaz\x1b[0m...\x1b[34m\x1b[1mhel\x1b[43mlo\x1b[0m");
	// printansi("\x1b[38;5;220mhello.");
	for (let n = 0; n < 256; n++) {
		printansi(`\x1b[48;5;${n}m        \x1b[0m ${n}`);
	}
}
