if (!process.browser) {
	console.error("This script is intended for browsers only.");
	process.exit(1);
}

let WVM = require("../wvm.js"),
	Parser = require("../parser.js"),
	WASMC = require("../../wasm/wasmc.js"),
	fs = require("fs"),
	Long = require("long"),
	_ = require("lodash"),
	ansiHTML = require("ansi-html"),
	chalk = require("chalk");

require("jquery.splitter");

const {REGISTER_OFFSETS} = require("../../wasm/constants.js");
window.Long = Long, window.WVM = WVM, window.Parser = Parser, window.WASMC = WASMC, window._ = _, window.chalk = chalk;

const UNPRINTABLE = [...[[0, 32]].reduce((a, [l, r]) => a.concat(_.range(l, r)), [])];
let App = window.App = class App {

	constructor(vm, config={ }) {
		this.vm = vm;
		this.heartbeatActive = false;
		this.active = false;
		this.interval = null;
		this.cursor = [0, 0];

		this.config = {
			range: [[0, 100]],
			displayWhitespace: false,
			attemptUTF8: false, // seems to be buggy.
			consoleSize: [120, 40],
			heartrate: 500
		};

		_.each(config, (val, key) => this.config[key] = val);
	}

	get range() {
		return this.config.range.map((arr) => arr.join("-")).join(", ");
	}

	set range(to) {
		if (to instanceof Array) {
			this.config.range = to;
			this.displayMemory();
			return;
		}

		let range = to.trim().split(/\D+/).map((s) => s.match(/^0x/)? parseInt(s.substr(2), 16) : parseInt(s));
		if (range.length % 2 == 1) {
			alert("Invalid range.");
			return;
		}

		// This algorithm to remove redundancy and overlap is overkill, but why not?
		let pairs = [];
		for (let i = 0; i < range.length; i += 2) {
			pairs.push(range.slice(i, i + 2));
		}

		let newPairs = [pairs[0]];
		pairs.slice(1).forEach((pair) => {
			for (let i = 0; i < newPairs.length; i++) {
				if (pair[0] <= newPairs[i][0]) {
					newPairs.splice(i, 0, pair);
					return;
				}
			}

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
			}

		}

		this.config.range = pairs;
		this.displayMemory();
	}

	inRange(addr) {
		for (let i = 0; i < this.config.range.length; i += 2) {
			if (this.config.range[i] <= addr && addr <= this.config.range[i + 1]) {
				return true;
			}
		}

		return false;
	}

	displayRegisters() {
		$("#registers").removeClass().addClass(`base-${localStorage.wvm_reg_base || 10}`);
		_.range(0, 128).forEach((i) => $($("#registers tr")[i]).find("td:last-child").html(this.vm.registers[i]? this.vm.registers[i].toSigned().toString(localStorage.wvm_reg_base || 10) : "?"));
	}

	displayMemory() {
		$("#memory tr").remove();
		this.config.range.forEach(([left, right]) => {
			_.range(Math.floor(left / 8), Math.ceil((right + 1) / 8)).forEach((i) => {
				const long = this.vm.getWord(8*i);

				const classes = [`addr-${i}`];

				if (vm.offsets.$code == i * 8)
					classes.push("code-start");
				if (vm.offsets.$data == i * 8 && vm.offsets.$data != vm.offsets.$code)
					classes.push("data-start");
				if (vm.offsets.$handlers == i * 8)
					classes.push("handlers-start");

				const tr = $("<tr></tr>").addClass(classes.join(" ")).appendTo($("#memory"))
					.append($("<td></td>").text(8*i))
					.append($("<td></td>").html(this.hexCell(long)))
					.append($("<td></td>").html(this.decompiledCell(long, 8*i)))
					.click((event) => {
						if (!$(event.target).hasClass("handler")) {
							vm.programCounter = 8*i;
							this.onTick();
						}
					});

				if (this.vm.data && this.vm.data.labels) {
					tr.append($("<td></td>").text(Object.keys(this.vm.data.labels).filter((label) => this.vm.data.labels[label] == 8 * i)[0] || ""));
				}
			});
		});
	}

	hexCell(long) {
		return _.chunk(long.toString(16).padStart(16, "0"), 2).map((x) => `<span class="digit-group">${x.join("")}</span>`).join("");
	}

	decompiledCell(long, addr) {
		if (addr < 32 || this.vm.offsets.$handlers <= addr && addr < this.vm.offsets.$data) {
			return $("<a></a>").attr({ href: "#" }).addClass("handler").text(long.toString()).click(() => {
				this.vm.programCounter = long.toInt();
				this.highlightProgramCounter();
			});
		}

		if (this.vm.offsets.$code <= addr) {
			if (long.equals(0)) {
				return "";
			}

			try {
				let html = ansiHTML(Parser.formatInstruction(long));
				html = html.replace(/#e8bf03/g, "orange")
						   .replace(/#ff00ff/g, "#f08")
						   .replace(/#00ffee/g, "#00bfff")
						   .replace(/#ff0000/g, "#a00")
						   .replace(/<span style="[^"]*">\$(rt|[fs]p|0|g|lo|hi)<\/span>/g, ($0, $1) => `<span class="reg-${$1}">$${$1}</span>`)
						   .replace(/<span style="[^"]*">\$(([ratskemf])[0-9a-f]+)<\/span>/g, ($0, $1, $2) => `<span class="reg-${$2}x">$${$1}</span>`);
						   // .replace(/<span style="[^"]*">\$sp<\/span>/, `<span class="reg-sp">$sp</span>`)
				return html;
			} catch(e) {
				return "❓";
			}
		}

		if (this.config.attemptUTF8) {
			try {
				return decodeURIComponent(escape(atob(String.fromCharCode(..._.chunk(long.toString(16).padStart(16, "0"), 2).map((x) => parseInt(x.join(""), 16) || ".".charCodeAt(0))))));
			} catch(e) {
				// Couldn't decode
			}
		}

		return _.chunk(long.toString(16).padStart(16, "0"), 2).map((x) => {
			const parsed = parseInt(x.join(""), 16);
			if (this.config.displayWhitespace) {
				switch (parsed) {
					case 9:  return "⭾";
					case 10:
					case 13: return "⏎";
				}
			}

			if (parsed == 32) {
				return "&nbsp;";
			}

			return UNPRINTABLE.includes(parsed)? "<span class=\"dim\">.</span>" : String.fromCharCode(parsed);
		}).join("");
	}

	decompileNext() {
		return Parser.formatInstruction(this.vm.loadInstruction());
	}

	initializeVM(vm) {
		if (vm) {
			this.vm = vm;
		}

		this.vm.enabled = false;

		this.range = `0-${this.vm.offsets.$end + (32 - 1) * 8}; ${8*(this.vm.memorySize - 16)}-${8*(this.vm.memorySize - 1)}`;
		this.displayRegisters();		

		(this.vm.onTick = this.onTick.bind(this))();
		this.vm.onSetWord = this.onSetWord.bind(this);
		this.vm.onSetByte = this.onSetByte.bind(this);
		this.vm.log = this.log.bind(this);
		this.vm.stop = this.stop.bind(this);
	}

	onTick() {
		this.displayRegisters();
		this.highlightProgramCounter();
		this.highlightStackPointer();
	}

	highlightProgramCounter() {
		const pc = this.vm.programCounter;

		if (pc % 8) {
			console.warn(`Program counter (${pc}) is misaligned by ${pc % 8} byte${pc % 8 == 1? "" : "s"}.`);
		}
		
		$(".program-counter").removeClass("program-counter");
		$(`#memory tr.addr-${pc / 8}`).addClass("program-counter")[0].scrollIntoViewIfNeeded();
	}

	highlightStackPointer() {
		const sp = this.vm.registers[REGISTER_OFFSETS.stack].toInt();

		if (sp % 8) {
			console.warn(`Stack pointer (${sp}) is misaligned by ${sp % 8} byte${sp % 8 == 1? "" : "s"}.`);
		}

		$(".stack-pointer").removeClass("stack-pointer");
		$(`#memory tr.addr-${sp / 8}`).addClass("stack-pointer");
	}

	onSetWord(addr, to) {
		addr = addr instanceof Long? addr.toInt() : addr;
		let row = $(`#memory tr.addr-${addr / 8}`);
		row.find("td:eq(1)").html(this.hexCell(to));
		row.find("td:eq(2)").html(this.decompiledCell(to, addr));
	}

	onSetByte(addr) {
		addr = addr instanceof Long? addr.toInt() : addr;
		let word = this.vm.getWord(addr - addr % 8);
		let row = $(`#memory tr.addr-${Math.floor(addr / 8)}`);
		row.find("td:eq(1)").html(this.hexCell(word));
		row.find("td:eq(2)").html(this.decompiledCell(word, addr));
	}

	heartbeat() {
		if (!this.heartbeatActive) {
			return clearInterval(this.interval);
		}

		if (this.active) {
			this.vm.tick();
			if (!this.vm.active) {
				this.active = false;
			}
		}
	}

	startHeartbeat() {
		clearInterval(this.interval);
		this.heartbeatActive = true;
		this.interval = setInterval(() => this.heartbeat(), this.heartrate);
	}

	get heartrate() {
		return this.config.heartrate;
	}

	set heartrate(to) {
		if (to != this.config.heartrate) {
			this.config.heartrate = to;
			if (this.heartbeatActive) {
				this.startHeartbeat();
			}
		}
	}

	toggleActive() {
		if ((this.active = !this.active)) {
			this.startHeartbeat();
		} else {
			clearInterval(this.interval);
		}

		return this.active;
	}

	log(str) {
		console.log(str);
	}

	stop() {
		this.active = false;
		this.vm.active = false;
		console.log(`Execution halted after ${this.vm.cycles} cycle${this.vm.cycles == 1? "" : "s"}.`);
	}

	onPrintChar(c) {
		const index = () => this.cursor[0] + this.cursor[1] * (this.config.consoleSize[0] + 1);
		if (this.consoleText.length <= index()) {
			return;
		}

		if (c == "\n" || this.consoleText[index()] == "\n") {
			this.cursor[0] = 0;
			this.cursor[1]++;

			if (c == "\n" || this.config.consoleSize[1] <= this.cursor[1]) {
				return;
			}
		}

		$("#console").text(this.consoleText = _.replaceChar(this.consoleText, index(), c));
		this.cursor[0]++;
	}

	initializeText() {
		$("#console").text(this.consoleText = [...Array(app.config.consoleSize[1])].map(() => " ".repeat(app.config.consoleSize[0])).join("\n"));
	}
};

function initializeUI(app) {
	for (let i = 0; i < 128; i++) {
		const regname = Parser.getRegister(i);
		const row = $("<tr></tr>").appendTo($("#registers")).addClass("reg-" + regname.replace(/^\$/, "").replace(/^([ratskremf])(\d+[a-f]*|\d*[a-f]+)$/, "$1x"));
		const valcell = $("<td></td>").text("0");
		const namecell = $("<td></td>").text(regname).click((event) => {
			event.stopPropagation();
			let input = prompt(`New value for ${regname}:`);
			if (!input) {
				return;
			}

			const radix = { b: 2, t: 3, q: 4, o: 8, h: 16, x: 16 }[input[0]] || 10;
			const unsigned = input[input.length - 1] == "u";
			const long = app.vm.registers[i] = Long.fromString(input.substring(radix != 10, input.length - (unsigned? 1 : 0)), unsigned, radix);
			valcell.text(long.toString(10));

			if (i == REGISTER_OFFSETS.stack) {
				app.highlightStackPointer();
			}
		});

		row.append(namecell).append(valcell).click(() => row.toggleClass("active-register"));
	}

	$("#top").split({ orientation: "vertical", limit: 210, position: "81.5%" });
	$(".hsplitter").height(4);
	$(".vsplitter").width(4);

	$("#step").click(() => app.vm.tick());
	$("#set_range").click(() => {
		let input = prompt("Range:", app.range);
		if (input) {
			app.range = input;
		}
	});

	$("#run").click(() => {
		$("#run")[app.toggleActive()? "addClass" : "removeClass"]("active");
	}).contextmenu((event) => {
		event.preventDefault();
		let old = app.active;
		app.active = false;
		let answer = parseFloat(prompt("Time between heartbeats (s):", app.config.heartrate / 1000));
		if (isNaN(answer) || answer < 0) {
			alert("Invalid number.");
		} else {
			app.config.heartrate = answer * 1000;
		}

		app.active = old;
	});

	$(document.body).keydown((event) => {
		const {key, ctrlKey, shiftKey} = event;

		if (key == ".") {
			app.vm.active && app.vm.tick();
		} else if (ctrlKey && !shiftKey) {
			if (key == "m") {
				$("#console").toggle();
			} else if (key == "b") {
				localStorage.wvm_reg_base = 2;
				app.displayRegisters();
			} else if (key == "o") {
				localStorage.wvm_reg_base = 8;
				app.displayRegisters();
			} else if (key == "d") {
				localStorage.wvm_reg_base = 10;
				app.displayRegisters();
			} else if (key == "h") {
				localStorage.wvm_reg_base = 16;
				app.displayRegisters();
			}
		} else {
			return;
		}

		event.preventDefault();
	});

	app.initializeText();
}

// let opened = Parser.read(fs.readFileSync(__dirname + "/../../wvm/c/ctest.why", "utf8"));
let opened = Parser.read(fs.readFileSync(__dirname + "/../../wasm/examples/memory.why", "utf8"));

let { offsets, handlers, meta, code } = opened.parsed;
let vm = new WVM({ program: { offsets, handlers, meta, code }, memory: opened.raw });
if (opened.data) {
	vm.data = opened.data;
}

window.vm = vm;
let app;

$(() => {
	app = window.app = new App(vm, { });
	app.config.consoleSize[0] = Math.max(10, Math.floor(($("#console").width()) / 8));
	app.config.consoleSize[1] = Math.max(10, Math.floor(($("#console").height() - 16) / 16 - 3));
	initializeUI(app);
	app.initializeVM();
});
