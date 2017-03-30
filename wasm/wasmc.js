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
	Long = require("long"),
	_ = require("lodash");

require("string.prototype.padstart").shim();
require("string.prototype.padend").shim();

class Wasmc {
	static die(...a) { console.error(...a); process.exit(1) };

	// Converts an array of 8 characters into a Long.
	static chunk2long(chunk) {
		return Long.fromString(chunk.map((c) => c.charCodeAt(0).toString(16).padStart(2, "0")).join(""), true, 16);
	};

	// Adds nulls to the end of the string to lengthen it to a multiple of 8.
	static nullpad(str) {
		return str.padEnd(Math.ceil(str.length / 8) * 8, "\0");
	};

	// Given any string, str2longs nullpads and chunks it and returns an array of Longs.
	static str2longs(str) {
		return _.chunk(Wasmc.nullpad(str).split(""), 8).map(Wasmc.chunk2long);
	};
	
	constructor(opt, filename) {
		this.opt = opt;
		this.filename = filename;
	};

	get binary() { return this.opt.binary };
	get debug() { return this.opt.debug };

	parse() {
		let grammar;
		try {
			grammar = require("./wasm.js");
		} catch (e) {
			console.error("Couldn't read wasm.js.");
			if (this.opt.debug) {
				console.error(e);
			};

			process.exit(1);
		};

		const parser = new nearley.Parser(grammar.ParserRules, grammar.ParserStart);
		const source = `\n${fs.readFileSync(filename, {encoding: "utf8"})}\n`;

		let trees;
		try {
			trees = parser.feed(source).results;
		} catch (e) {
			console.error(chalk.red("Syntax error"), "at", chalk.white(`${getline(source, e.offset) - 1}:${e.offset - source.split(/\n/).slice(0, getline(source, e.offset) - 1).join("\n").length - 1}`) + ":");
			if (this.opt.dev) {
				console.log(e.message.replace(/\(@(\d+):([^)]+)\)/g, ($0, $1, $2) => { const _line = getline(source, e.offset) - 1; return `(@${_line}:${e.offset - source.split(/\n/).slice(0, _line).join("\n").length - 1 + $2})` }));
			};

			process.exit(1);
		};

		if (trees.length > 1) {
			trees.forEach((tree) => console.log(JSON.stringify(trees[tree], null, 4)));
			console.error(chalk.red.italic(`^^^^^^^^^^^^^^^^^^^^^^\nAmbiguous grammar (${trees.length}).\n`));
			process.exit(1);
		} else if (trees.length === 0) {
			console.warn(chalk.red.italic("Nothing parsed."));
			process.exit(1);
		};

		let parsed = trees[0];

		if (typeof parsed != "object") {
			Wasmc.die("Parser output isn't an object.");
		};

		if (typeof parsed.metadata == "undefined") {
			parsed.metadata = { };
		};

		return parsed;
	};

	compile() {
		let parsed = this.parse();

		let handlers = [], data = [], code = [];
		let meta = this.metaPreliminary(parsed);

		const out = meta.concat(handlers).concat(data).concat(code);

		console.log({out: out.map(x => typeof x != "undefined"? x.toString(16).padStart(16, "0") : "0".repeat(16))});
	};

	metaPreliminary(parsed) {
		let meta = [], [name, version, author] = [parsed.meta.name || "?", parsed.meta.version || "?", parsed.meta.author || "?"];

		const orcid = typeof parsed.meta.orcid == "undefined"? "0000000000000000" : parsed.meta.orcid.replace(/\D/g, "");
		if (orcid.length != 16) {
			Wasmc.die("Error: invalid ORCID.");
		};

		// Convert the ORCID into two Longs and stash them in the correct positions in meta.
		[meta[4], meta[5]] = [orcid.substr(0, 8), orcid.substr(8)].map((half) => Wasmc.chunk2long(half.split("")));

		meta = meta.concat(Wasmc.str2longs(`${name}\0${version}\0${author}\0`));
		
		// The beginning of the handler pointer section comes right after the end of the meta section.
		meta[0] = Long.fromInt(meta.length, true);

		this.log({parsed, meta, version, author});

		return meta;
	};

	log(...args) {
		if (this.debug) {
			console.log(...args);
		};

		return !!this.debug;
	};
};

const opt = minimist(process.argv.slice(2), {
	alias: { b: "binary", d: "debug" },
	boolean: ["binary", "debug"],
	default: { binary: false, debug: true }
}), filename = opt._[0];

if (!filename) {
	return console.log("Usage: node wasmc.js [filename]");
};

new Wasmc(opt, filename).compile();