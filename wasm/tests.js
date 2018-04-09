#!/usr/local/bin/node
let Wasmc = require("./wasmc.js").Wasmc,
	Long = require("long"),
	chalk = require("chalk");

const {OPS, FUNCTS} = require("./constants.js");

let tests = {
	passed: 0,
	failed: 0,
	undef: 0,
	pass(x) { tests.passed++; return chalk.green(`${x} \u2714`); },
	fail(x) { tests.failed++; return chalk.red(`${x} \u2718`); },
	maybe(x) { tests.undef++; return chalk.yellow(`${x} ~`); },
	assert(bool, trueMessage, falseMessage, assertTrue=true, handleUndef=true) {
		return typeof bool == "undefined" && handleUndef? tests.maybe("Undefined") : (tests[(!!assertTrue == !!bool)? "pass" : "fail"])(bool? trueMessage : falseMessage);
	},
	showResults() {
		console.log("");
		let color = tests.passed + tests.failed == 0? chalk.dim : (tests.passed == 0 && 0 < tests.failed? chalk.red : (0 < tests.passed && tests.failed == 0? chalk.green : chalk.yellow));
		// let pre = `${(0 < tests.undef? Array(Math.ceil(len / 2)).fill(`${color}█${yellow}█`).join("") : color + Array(len).fill("█").join(""))+reset} `;
	
		// let pre = `${0 < tests.undef? Array(Math.ceil(len / 2)).fill(`${color("█")}█`).join("") : color(Array(len).fill("█").join(""))} `;
		let pre = `${(0 < tests.undef? chalk.yellow : color)("█".repeat(3))} `;

		let great = "Nice 👌🏻 👍🏻";
		if (tests.passed == 0 && tests.failed == 0) {
			console.log(`${pre}No tests were run.`);
		} else {
			console.log(`${pre+chalk.green(tests.passed)} ${chalk.bold("passed")} / ${chalk.yellow(tests.undef)} ${chalk.bold("undefined")} / ${chalk.red(tests.failed)} ${chalk.bold("failed")}`);
			if (tests.passed == 0 && 0 < tests.failed) {
				console.log(`${pre+chalk.bold("Verdict")}: Yikes. 😬`);
			} else if (0 < tests.passed && tests.failed == 0) {
				console.log(`${pre+chalk.bold("Verdict")}: ${0 < tests.undef? "Fix the undefined values and you're good. 🔧" : great}`);
			} else if (0 < tests.passed && 0 < tests.failed) {
				console.log(`${pre+chalk.bold("Verdict")}: ${chalk.yellow(Math.floor(1000*tests.passed/(tests.passed+tests.failed))/10 + "%")} isn't good enough; try harder.`);
			}
		}
	}
};

const checkEqual = (prefix="", assertTrue, ...input) => input.slice(1).forEach((item) => console.log(`${prefix? prefix + ": " : ""}${chalk.bold(input[0])} == ${chalk.bold(item)}? ${tests.assert(input[0] === item, "Yes", "No", assertTrue)}`));
const checkLong = (prefix="", assertTrue, ...input) => {
	const ensureLong = (x) => {
		if (x instanceof Long) return x;
		if (typeof x == "string") return Long.fromString(x, true, 2);
		if (typeof x == "number") return Long.fromInt(x, true);
		return Long.fromValue(x);
	};

	let longs = input.map(ensureLong);
	longs.slice(1).forEach((long) => {
		console.log(`${prefix? prefix + ": " : ""}${chalk.bold(longs[0].toString(2))} == ${chalk.bold(long.toString(2))}? ${tests.assert(longs[0].equals(long), "Yes", "No", assertTrue)}`);
	});
};

let c = new Wasmc({ binary: true, debug: false }, "");

checkLong("Long.fromInt", true, Long.fromInt(2), Long.fromInt(2), Long.fromString("2"), Long.fromString("10", true, 2), Long.fromString("2", true, 16));
checkLong("Long.fromInt", false, Long.fromInt(3), Long.fromInt(4));


checkEqual("Wasmc.convertRegister", true, Wasmc.convertRegister([, "s", 0x0a]), 71);
checkEqual("Wasmc.convertRegister", true, Wasmc.convertRegister([, "t", 0x02]), 39);

checkLong("l", true,
	c.rType(OPS.l, 0, Wasmc.convertRegister([, "s", 0x0a]), Wasmc.convertRegister([, "t", 0x02]), 0, FUNCTS.l),
	Long.fromString("0000000100100000000100011101001110000000000000000000000000000001", true, 2)
);

checkLong("lui", true,
	c.iType(OPS.lui, Wasmc.convertRegister([, "t", 0]), Wasmc.convertRegister([, "r", 5]), 42),
	Long.fromString("0000000011010000000100101000101000000000000000000000000000101010", true, 2)
);

checkLong("j", true,
	c.jType(OPS.j, 0, 666666),
	Long.fromString("0000000011110000000000000000000000000000000010100010110000101010", true, 2)
);

// 000000010000 sssssss 0000000000000 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
// opcode rs addr
checkLong("jc", true,
	c.jType(OPS.jc, Wasmc.convertRegister([, "s", 0x14]), 666666),
	Long.fromString("0000000100001010001000000000000000000000000010100010110000101010", true, 2)
);

tests.showResults();
