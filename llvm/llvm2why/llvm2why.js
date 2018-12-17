#!/usr/bin/env node
let util = require("util"),
	exec = util.promisify(require("child_process").exec),
	rimraf = require("rimraf"),
	chalk = require("chalk"),
	shell_escape = require("shell-escape"),
	minimist = require("minimist"),
	path = require("path");

class LLVM2Why {
	static async produceIR(dirName, ...filenames) {
		let deleteDir = false;
		if (!dirName) {
			let {err, stdout, stderr} = await exec("mktemp -d");
			if (err) throw err;
			dirName = stdout.trim();
			deleteDir = true;
		}

		console.log(`Using ${chalk.bold(dirName)}.`);
		
		const resolved = filenames.map((f) => path.resolve(f));
		const cmd = shell_escape(["clang", "-S", "-emit-llvm", ...resolved]);

		process.chdir(dirName);

		let {err, stdout, stderr} = await exec(cmd);
		if (err) throw err;
		
		console.log({err, stdout, stderr});

		if (deleteDir) {
			rimraf.sync(dirName);
		}
	}
}

module.exports = LLVM2Why;

if (require.main === module) {
	const opt = minimist(process.argv.slice(2), {alias: {t: "temp"}});
	LLVM2Why.produceIR(opt.t, ...opt._);
}
