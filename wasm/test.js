#!/usr/local/bin/node
let fs = require("fs"),
	nearley = require("nearley"),
	nearleyg = require("nearley/lib/nearley-language-bootstrapped.js"),
	nearleyc = require("nearley/lib/compile.js"),
	gen = require("nearley/lib/generate.js"),
	chalk = require("chalk"),
	getline = require("get-line-from-pos"),
	minimist = require("minimist");

const die = (...a) => { console.error(...a); process.exit(1) };

const opt = minimist(process.argv.slice(2), {
	alias: { t: "tree", d: "dev" },
	boolean: ["tree", "dev"],
	default: { dev: true }
}), filename = opt._[0];

if (!filename) {
	die("Invalid filename.", { opt });
};

let grammar = "wasm", parser;
let lines = fs.readFileSync(filename, { encoding: "utf8" }).split("\n");

if (opt.dev) {
	let file = grammar;
	grammar = fs.readFileSync(`${__dirname}/${grammar}.ne`, "utf8");
	parser = new nearley.Parser(nearleyg.ParserRules, nearleyg.ParserStart);
	grammar = nearleyc(parser.feed(grammar).results[0], { });
	fs.writeFileSync(`${__dirname}/${file}.js`, gen(grammar, "grammar"));
	grammar = require(`${__dirname}/${file}.js`);
} else {
	try {
		grammar = require(`./${grammar}.js`);
	} catch (e) {
		console.error(`Couldn't read ${grammar}.ne.`);
		if (opt.dev) {
			console.log(e);
		};

		process.exit(1);
	};
};

// generate parser
parser = new nearley.Parser(grammar.ParserRules, grammar.ParserStart)

// join up the lines again
let source = `\n${lines.join("\n")}\n`;

try {
	var trees = parser.feed(source).results
} catch (e) {
	console.error(chalk.red("Syntax error"), "on", chalk.white(`line ${getline(source, e.offset) - 1}`));
	if (opt.dev) {
		console.log(e.message);
	};

	process.exit(1);
};

if (trees.length > 1 && opt.dev) {
	trees.forEach((tree) => console.log(JSON.stringify(trees[tree], null, 0), "\n"));
	console.warn(chalk.yellow.italic(`^^^^^^^^^^^^^^^^^^^^^^\nAmbiguous grammar (${trees.length}).\n`));
} else if (trees.length === 0) {
	console.warn(chalk.yellow.italic("Nothing parsed."));
	process.exit(1);
} else if (opt.tree) {
	console.log(JSON.stringify(trees[0], null, 2));
};
