#!/usr/local/bin/node
let fs = require("fs"),
	nearley = require("nearley"),
	nearleyg = require("nearley/lib/nearley-language-bootstrapped.js"),
	nearleyc = require("nearley/lib/compile.js"),
	gen = require("nearley/lib/generate.js"),
	chalk = require("chalk"),
	getline = require("get-line-from-pos"),
	minimist = require("minimist"),
	prettyjson = require("prettyjson"),
	path = require("path");

const die = (...a) => { console.error(...a); process.exit(1) };

const opt = minimist(process.argv.slice(2), {
	alias: { t: "tree", d: "dev", s: "simple" },
	boolean: ["tree", "dev", "simple"],
	default: { dev: true }
}), filename = opt._[0];

if (!filename) {
	die("Invalid filename.", { opt });
};


let name = "wasm", grammar, parser;
let lines = fs.readFileSync(filename, { encoding: "utf8" }).split("\n");

if (opt.dev) {
	let file = grammar;
	grammar = fs.readFileSync(path.join(__dirname, `${name}.ne`), "utf8");
	parser = new nearley.Parser(nearleyg.ParserRules, nearleyg.ParserStart);
	grammar = nearleyc(parser.feed(grammar).results[0], { });
	fs.writeFileSync(path.join(__dirname, `${name}.js`), gen(grammar, "grammar"));
	grammar = require(path.join(__dirname, `${name}.js`));
} else {
	try {
		grammar = require(`./${grammar}.js`);
	} catch (e) {
		console.error(`Couldn't read ${grammar}.js.`);
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

let trees;
try {
	trees = parser.feed(source).results
} catch (e) {
	console.error(chalk.red("Syntax error"), "at", chalk.white(`${getline(source, e.offset) - 1}:${e.offset - source.split(/\n/).slice(0, getline(source, e.offset) - 1).join("\n").length - 1}`) + ":");
	if (opt.dev) {
		console.log(e.message.replace(/\(@(\d+):([^)]+)\)/g, ($0, $1, $2) => { const _line = getline(source, e.offset) - 1; return `(@${_line}:${e.offset - source.split(/\n/).slice(0, _line).join("\n").length - 1 + $2})` }));
	};

	process.exit(1);
};

const printTree = (tree) => prettyjson.render(tree || "[null]");

if (trees.length > 1 && opt.dev) {
	trees.forEach((tree) => console.log(opt.simple? JSON.stringify(trees[tree], null, 4) : printTree(trees[tree]), "\n"));
	console.warn(chalk.yellow.italic(`^^^^^^^^^^^^^^^^^^^^^^\nAmbiguous grammar (${trees.length}).\n`));
} else if (trees.length === 0) {
	console.warn(chalk.yellow.italic("Nothing parsed."));
	process.exit(1);
} else if (opt.tree) {
	console.log(opt.simple? JSON.stringify(trees[0], null, 4) : printTree(trees[0]));
};
