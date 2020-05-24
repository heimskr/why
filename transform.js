#!/usr/bin/env node

// Transforms standard Why.js bytecode files ("xxxxxxxxxxxxxxxx\nxxxxxxxxxxxxxxxx") into a form usable with $readmemh
// ("xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx").

const fs = require("fs");

if (process.argv.length < 3) {
	console.error("Usage: transform.js <inputfile>");
	process.exit(1);
}

let text = "";
try {
	text = fs.readFileSync(process.argv[2], "utf8");
} catch (e) {
	console.error("An error occurred while reading " + process.argv[2] + ": ", e);
	process.exit(1);
}


text = text.trim().split("\n").join("");

let bytes = [];
for (let i = 0; i < text.length; i += 2) {
	bytes.push(text.substr(i, 2));
}

console.log(bytes.join("\n"));
