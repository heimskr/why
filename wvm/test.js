#!/usr/bin/env node
let WVM = require("./wvm.js"),
	Parser = require("./parser.js"),
	_ = require("lodash"),
	chalk = require("chalk");

let opened = Parser.open("../wasm/compiled/linkertest.why", true);
let { offsets, handlers, meta, code } = opened.parsed;
let vm = new WVM({ program: { offsets, handlers, meta, code }, memory: opened.raw });

console.log();
console.log(chalk.dim("┌────────────────┬────────────────┬───────────────────────┐"));
console.log(["", "    Raw Long    ", "   VM.getWord   ", "      VM.getByte       ", ""].join(chalk.dim("│")));
console.log(chalk.dim("├────────────────┼────────────────┼──┬──┬──┬──┬──┬──┬──┬──┤"));

for (let i = 0; i < opened.raw.length; i++) {
	console.log(chalk.dim("│") + [...[opened.raw[i], vm.getWord(8*i)].map((l) => l.toString(16).padStart(16, "0")), ..._.range(0, 8).map((n) => vm.getByte(8*i + n).toString(16).padStart(2, "0"))].join(chalk.dim("│")) + chalk.dim("│"));
}

console.log(chalk.dim("└────────────────┴────────────────┴──┴──┴──┴──┴──┴──┴──┴──┘"));
