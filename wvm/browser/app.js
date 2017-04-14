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
	chalk = require("chalk"),
	moment = require("moment");

require("jquery.splitter");

const { EXCEPTIONS, R_TYPES, I_TYPES, J_TYPES, OPCODES, FUNCTS, REGISTER_OFFSETS, TRAPS } = require("../../wasm/constants.js");
window.Long = Long, window.WVM = WVM, window.Parser = Parser, window.WASMC = WASMC, window._ = _, window.chalk = chalk;

let App = window.App = {
	config: {
		range: "0-100",
		displayWhitespace: false,
		attemptUTF8: false // seems to be buggy.
	},

	vm: null,
	range: [[0, 100]],
	heartbeatActive: false,
	heartrate: 500,
	interval: null,

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
		_.range(0, 128).forEach((i) => $($("#registers tr")[i]).find("td:last-child").text(vm.registers[i].toSigned().toString()));
	},

	displayMemory() {
		$("#memory tr").remove();
		App.range.forEach(([left, right]) => {
			_.range(Math.floor(left / 8), Math.ceil((right + 1) / 8)).forEach((i) => {
				let long = App.vm.getWord(8*i);
				$("<tr></tr>").addClass(`addr-${i}${8*vm.programCounter == i? " program-counter" : ""}`).appendTo($("#memory"))
					.append($("<td></td>").text(8*i))
					.append($("<td></td>").html(App.hexCell(long)))
					.append($("<td></td>").html(App.decompiledCell(long, 8*i)))
					.click((event) => {
						if (!$(event.target).hasClass("handler")) {
							vm.programCounter = 8*i;
							App.onTick();
						};
					});
			});
		});
	},

	hexCell(long) {
		return _.chunk(long.toString(16).padStart(16, "0"), 2).map((x) => `<span class="digit-group">${x.join("")}</span>`).join("");
	},

	_unprintable: [...[[0, 32]].reduce((a, [l, r]) => a.concat(_.range(l, r)), [])],

	decompiledCell(long, addr) {
		if (App.vm.offsets.$handlers <= addr && addr < App.vm.offsets.$data) {
			return $("<a></a>").attr({ href: "#" }).addClass("handler").text(long.toString()).click(() => {
				vm.programCounter = long.toInt();
				App.highlightProgramCounter();
			});
		};

		if (App.vm.offsets.$code <= addr) {
			return long.equals(0)? "" : ansiHTML(Parser.formatInstruction(long)).replace(/\#e8bf03/g, "orange").replace(/\#ff00ff/g, "purple").replace(/\#00ffee/g, "#00bfff").replace(/\#ff0000/g, "#a00");
		};

		if (App.config.attemptUTF8) {
			try {
				return decodeURIComponent(escape(atob(String.fromCharCode(..._.chunk(long.toString(16).padStart(16, "0"), 2).map((x) => parseInt(x.join(""), 16) || ".".charCodeAt(0))))));
			} catch(e) { };
		};

		return _.chunk(long.toString(16).padStart(16, "0"), 2).map((x) => {
			const parsed = parseInt(x.join(""), 16);
			if (App.config.displayWhitespace) {
				switch (parsed) {
					case 9:  return "⭾";
					case 10:
					case 13: return "⏎";
				};
			};

			return App._unprintable.includes(parsed)? `<span class="dim">.</span>` : String.fromCharCode(parsed);
		}).join("");
	},

	decompileNext() {
		return Parser.formatInstruction(App.vm.loadInstruction());
	},

	initializeVM(vm) {
		App.vm = vm;
		vm.enabled = false;
		vm.ttl = WVM.DEFAULT_TTL;

		App.setRange(`0-${vm.offsets.$end*8 - 8}; ${8*(vm.memorySize - 10)}-${8*(vm.memorySize - 1)}`);
		App.displayRegisters();

		(vm.onTick = App.onTick)();
		vm.onSetWord = App.onSetWord;
		vm.log = App.log;
		vm.stop = App.stop;
	},

	onTick() {
		App.displayRegisters();
		App.highlightProgramCounter();
		App.highlightStackPointer();
	},

	highlightProgramCounter() {
		const pc = vm.programCounter;

		if (pc % 8) {
			console.warn(`Program counter (${pc}) is misaligned by ${pc % 8} byte${pc % 8 == 1? "" : "s"}.`);
		};
		
		$(".program-counter").removeClass("program-counter");
		$(`#memory tr.addr-${pc / 8}`).addClass("program-counter");
	},

	highlightStackPointer() {
		const sp = vm.registers[REGISTER_OFFSETS.stack].toInt();

		if (sp % 8) {
			console.warn(`Stack pointer (${sp}) is misaligned by ${sp % 8} byte${sp % 8 == 1? "" : "s"}.`);
		};

		$(".stack-pointer").removeClass("stack-pointer");
		$(`#memory tr.addr-${sp / 8}`).addClass("stack-pointer");
	},

	onSetWord(addr, to) {
		let row = $(`#memory tr.addr-${addr.toInt() / 8}`);
		row.find("td:eq(1)").html(App.hexCell(to));
		row.find("td:eq(2)").html(App.decompiledCell(to, addr));
	},

	heartbeat() {
		if (!App.heartbeatActive) {
			return clearInterval(App.interval);
		};

		if (App.active) {
			vm.tick();
			if (!vm.active) {
				App.active = false;
			};
		};
	},

	startHeartbeat() {
		clearInterval(App.interval);
		App.heartbeatActive = true;
		App.interval = setInterval(App.heartbeat, App.heartrate);
	},

	setHeartrate(newHeartrate) {
		if (newHeartrate == App.heartbeat) {
			return;
		};

		App.heartrate = newHeartrate;
		if (App.heartbeatActive) {
			App.startHeartbeat();
		};
	},

	toggleActive() {
		if (App.active = !App.active) {
			App.startHeartbeat();
		} else {
			clearInterval(App.interval);
		};

		return App.active;
	},

	log(str) {
		console.log(str);
		$("<tr></tr>").append($("<th></th>").text(moment().format("HH:mm:ss"))).append($("<td></td>").html(str)).prependTo($("#console .messages"));
	},

	stop() {
		App.active = false;
		vm.active = false;
		App.log("Execution halted.");
	}
};

function initializeUI() {
	for (let i = 0; i < 128; i++) {
		const regname = Parser.getRegister(i);
		const row = $("<tr></tr>").appendTo($("#registers")).addClass("reg-" + regname.replace(/^\$/, "").replace(/^([ratskremf])(\d+[a-f]*|\d*[a-f]+)$/, "$1x"));
		const valcell = $("<td></td>").text("0");
		const namecell = $("<td></td>").text(regname).click((event) => {
			event.stopPropagation();
			let input = prompt(`New value for ${regname}:`);
			const radix = { b: 2, t: 3, q: 4, o: 8, h: 16, x: 16 }[input[0]] || 10;
			const unsigned = input[input.length - 1] == "u";
			const long = App.vm.registers[i] = Long.fromString(input.substring(radix != 10, input.length - (unsigned? 1 : 0)), unsigned, radix);
			valcell.text(long.toString(10));

			if (i == REGISTER_OFFSETS.stack) {
				App.highlightStackPointer();
			};
		});

		row.append(namecell).append(valcell).click(() => row.toggleClass("active-register"));
	};

	// $(window).resize(() => $("#container").height(window.innerHeight - 28).split({ orientation: "horizontal" }));
	// $("#container").height(window.innerHeight - 28).split({ orientation: "horizontal", limit: 5, position: "900" });
	$("#top").split({ orientation: "vertical", limit: 210, position: "83%" });
	$(".hsplitter").height(4);
	$(".vsplitter").width(4);

	$("#step").click(() => App.vm.tick());
	$("#set_range").click(() => {
		let input = prompt("Range:", App.config.range);
		if (input) {
			App.config.range = App.setRange(input);
		};
	});

	$("#run").click(() => {
		$("#run")[App.toggleActive()? "addClass" : "removeClass"]("active");
	}).contextmenu((event) => {
		event.preventDefault();
		let old = App.active;
		App.active = false;
		let answer = parseFloat(prompt("Time between heartbeats (s):", App.heartrate / 1000));
		if (isNaN(answer) || answer < 0) {
			alert("Invalid number.");
		} else {
			App.heartrate = answer * 1000;
		};

		App.active = old;
	});

	$(".navbar-brand").click(() => {
		
	});

	$("#console .clear").click(() => $("#console .messages tr").remove());

	$(document.body).keydown(({ key }) => {
		if (key == ".") {
			App.vm.tick();
		};
	});
};

// let opened = Parser.read(fs.readFileSync(__dirname + "/../../wasm/compiled/linkertest.why", "utf8"));
// let opened = Parser.read(fs.readFileSync(__dirname + "/../../wasm/compiled/fibonacci.why", "utf8"));
// let opened = Parser.read(fs.readFileSync(__dirname + "/../../wasm/compiled/fibonacci.wo", "utf8"));
let opened = Parser.read(fs.readFileSync(__dirname + "/../../wasm/compiled/shifts.why", "utf8"));

let { offsets, handlers, meta, code } = opened.parsed;
let vm = new WVM({ program: { offsets, handlers, meta, code }, memory: opened.raw });
window.vm = vm;

$(() => {
	initializeUI();
	App.initializeVM(vm);
});
