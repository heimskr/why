#!/usr/bin/env node
let {exec} = require("child_process"),
	rimraf = require("rimraf"),
	chalk = require("chalk");

class LLVM2Why {
	static produceIR(...filenames) {
		exec("mktemp -d", function(err, stdout, stderr) {
			if (err) throw err;
			const dirName = stdout.trim();
			console.log(`Using ${chalk.bold(dirName)}.`);

			rimraf(dirName, () => {
				console.log("Done.");
			});
		});
	}
}

module.exports = LLVM2Why;

if (require.main === module) {
	LLVM2Why.produceIR("test/io.c");
}
