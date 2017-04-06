if (!process.browser) {
	console.error("This script is intended for browsers only.");
	process.exit(1);
};

let WVM = require("../wvm.js"),
	Parser = require("../parser.js"),
	WASMC = require("../../wasm/wasmc.js"),
	fs = require("fs"),
	Long = require("long"),
	_ = require("lodash"),
	ansiHTML = require("ansi-html"),
	chalk = require("chalk");

require("jquery.splitter");

const { EXCEPTIONS, R_TYPES, I_TYPES, J_TYPES, OPCODES, FUNCTS, REGISTER_OFFSETS, TRAPS } = require("../../wasm/constants.js");
window.Long = Long, window.WVM = WVM, window.Parser = Parser, window.WASMC = WASMC, window._ = _, window.chalk = chalk;

let App = window.App = {
	config: {
		range: "0-100",
	},

	vm: null,
	range: [[0, 100]],

	setRange(str) {
		let range = str.trim().split(/\D+/).map((s) => s.match(/^0x/)? parseInt(s.substr(2), 16) : parseInt(s));
		if (range.length % 2 == 1) {
			alert("Invalid range.");
			return;
		};

		// This algorithm to remove redundancy and overlap is overkill, but why not?
		let pairs = [];
		for (let i = 0; i < range.length; i += 2) {
			pairs.push(range.slice(i, i + 2));
		};

		let newPairs = [pairs[0]];
		pairs.slice(1).forEach((pair) => {
			for (let i = 0; i < newPairs.length; i++) {
				if (pair[0] <= newPairs[i][0]) {
					newPairs.splice(i, 0, pair);
					return;
				};
			};

			newPairs.push(pair);
		});

		pairs = newPairs;
		for (let i = 0; i < pairs.length - 1; i++) {
			let [left, right] = pairs[i];
			let [left2, right2] = pairs[i + 1];
			if (right < left2) {
				continue;
			} else if (right == left2 || (left2 < right && right <= right2)) {
				pairs.splice(i, 2, [left, right2]);
				i--;
			} else if (right2 <= right) {
				pairs.splice(i, 2, [left, right]);
				i--;
			};

		};

		App.range = pairs;
		App.displayMemory();
		return App.config.range = str;
	},

	inRange(addr) {
		for (let i = 0; i < App.range.length; i += 2) {
			if (App.range[i] <= addr && addr <= App.range[i + 1]) {
				return true;
			};
		};

		return false;
	},

	displayRegisters() {
		_.range(0, 128).forEach((i) => $($("#registers tr")[i]).find("td:last-child").text(vm.registers[i]));
	},

	displayMemory() {
		$("#memory tr").remove();
		App.range.forEach(([left, right]) => {
			_.range(left, right + 1).forEach((i) => {
				let long = App.vm.get(i);
				$("<tr></tr>").addClass(`addr-${i}`).appendTo($("#memory"))
					.append($("<td></td>").text(i))
					.append($("<td></td>").html(App.hexCell(long)))
					.append($("<td></td>").html(App.decompiledCell(long, i)))
					.click(() => {
						vm.programCounter = i;
						App.onTick();
					});
			});
		});
	},

	hexCell(long) {
		return _.chunk(long.toString(16).padStart(16, "0"), 2).map((x) => `<span class="digit-group">${x.join("")}</span>`).join("");
	},

	decompiledCell(long, i) {
		return i < App.vm.offsets.$code || long.equals(0)? "" : ansiHTML(Parser.formatInstruction(long));
	},

	decompileNext() {
		return Parser.formatInstruction(App.vm.loadInstruction());
	},

	initializeVM(vm) {
		App.vm = vm;
		vm.enabled = false;
		vm.ttl = WVM.DEFAULT_TTL;
		vm.programCounter = vm.offsets.$code;

		App.range = [[0, 50], [vm.memorySize - 15, vm.memorySize - 1]];
		App.displayMemory();
		App.displayRegisters();

		(vm.onTick = App.onTick)();
		vm.onSet = App.onSet;
	},

	onTick() {
		App.displayRegisters();
		$("#memory tr").removeClass("program-counter");
		$(`#memory tr.addr-${vm.programCounter}`).addClass("program-counter");
	},

	onSet(addr, to) {
		let row = $("#registers tr").eq(addr);
		row.find("td eq:2").html(App.hexCell(to));
		row.find("td eq:3").html(App.decompiledCell(to, addr));
	}
};

function initializeUI() {
	for (let i = 0; i < 128; i++) {
		const regname = Parser.getRegister(i);
		const row = $("<tr></tr>").appendTo($("#registers")).append($("<td></td>").text(regname)).append($("<td></td>").text("0")).addClass("reg-" + regname.replace(/^\$/, "").replace(/^([ratskremf])(\d+[a-f]*|\d*[a-f]+)$/, "$1x"));
		row.click(() => row.toggleClass("active-register"));
	};

	$(window).resize(() => $("#container").height(window.innerHeight - 50).split({ orientation: "horizontal" }));
	$("#container").height(window.innerHeight - 50).split({ orientation: "horizontal", limit: 5, position: "100%" });
	$("#top").split({ orientation: "vertical", limit: 210, position: "83%" });
	$(".hsplitter").height(4);
	$(".vsplitter").width(4);

	$("#step").click(() => vm.tick());
	$("#set_range").click(() => {
		let input = prompt("Range:", App.config.range);
		if (input) {
			App.config.range = App.setRange(input);
		};
	});
};

let raw = fs.readFileSync(__dirname + "/../../wasm/compiled/fibonacci.why", "utf8").split("\n").map((s) => Long.fromString(s, true, 16))
let parsed = Parser.parse(raw, true);

let { offsets, handlers, meta, code } = parsed;
let vm = new WVM({ program: { offsets, handlers, meta, code }, memory: raw });

$(() => {
	initializeUI();
	App.initializeVM(vm);
});