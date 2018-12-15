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
	chalk = require("chalk"),
	text = require("./text.js");

require("jquery.splitter");

const {REGISTER_OFFSETS, FLAGS} = require("../../wasm/constants.js");
window.Long = Long, window.WVM = WVM, window.Parser = Parser, window.WASMC = WASMC, window._ = _, window.chalk = chalk;

const UNPRINTABLE = [...[[0, 32], [127, 159], [173, 173]].reduce((a, [l, r]) => a.concat(_.range(l, r)), [])];
let App = window.App = class App {

	constructor(vm, config={}) {
		this.vm = vm;
		this.heartbeatActive = false;
		this._active = false;
		this.interval = null;
		this.cursor = [0, 0];
		this.breakpoints = [];
		this.tempHeartrate = null;
		this.skippedSubroutine = null;

		this.config = {
			range: [[0, 100]],
			displayWhitespace: false,
			attemptUTF8: false, // seems to be buggy.
			consoleSize: [120, 40],
			heartrate: 0,
			skipStrprint: true,
			excludeSymtab: false
		};

		_.each(config, (val, key) => this.config[key] = val);
		this.loadBreakpoints();
	}

	get active() {
		return this._active;
	}

	set active(to) {
		this._active = to;

		if (to) {
			$("#run").addClass("active");
			this.vm.active = true;
			this.startHeartbeat();
		} else {
			$("#run").removeClass("active");
		}

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

	renderFormatLink() {
		$("#format").show().find("span").text(localStorage.formatStyle == "mnem"? "Mnemonics" : "WASM");
	}

	displayRegisters() {
		$("#registers").removeClass().addClass(`base-${localStorage.wvm_reg_base || 10}`);
		_.range(0, 128).forEach((i) => $($("#registers tr")[i]).find("td:last-child").html(this.vm.registers[i]? this.vm.registers[i].toSigned().toString(localStorage.wvm_reg_base || 10) : "?"));
	}

	displayMemory() {
		$("#memory tr").remove();
		
		const edges = this.symbolTableEdges;

		const {$symtab, $handlers, $code, $data, $end} = vm.offsets;

		this.config.range.forEach(([left, right]) => {
			_.range(Math.floor(left / 8), Math.ceil((right + 1) / 8)).forEach((i) => {
				const byte = i * 8;
				const long = this.vm.getWord(byte);

				const classes = [`addr-${i}`];
				const dataStart = $data == byte && $data != $code;
				const symtabStart = $symtab == byte;
				const handlersStart = $handlers == byte;
				const codeStart = $code == byte;
				const stackStart = vm.memorySize - 16 == i;
				const globalStart = $end == byte;

				if (stackStart || edges.includes(byte))
					classes.push("dashed");
				if (handlersStart || symtabStart || codeStart || dataStart || globalStart)
					classes.push("solid");

				if ($data <= byte && this.vm.symbols && _.keys(this.vm.symbols).filter((s) => this.vm.symbols[s][1].toInt() == byte).length)
					classes.push("dashed");

				let addrcell;
				const tr = $("<tr></tr>").addClass(_.uniq(classes).join(" ")).appendTo($("#memory"))
					.append(addrcell = $("<td></td>").addClass(`addr-cell addr-cell-${byte}`).text(byte).on("contextmenu", (event) => {
						if (this.breakpoints.includes(byte)) {
							this.breakpoints = _.without(this.breakpoints, byte);
							addrcell.removeClass("breakpoint");
						} else {
							this.breakpoints = _.uniq([...this.breakpoints, byte]);
							addrcell.addClass("breakpoint");
						}

						this.saveBreakpoints();
						event.preventDefault();
					}))
					.append($("<td></td>").html(this.hexCell(long)))
					.append($("<td></td>").html(this.decompiledCell(long, byte)))
					.click((event) => {
						if (!$(event.target).hasClass("handler")) {
							vm.programCounter = byte;
							this.onTickUI();
						}
					});

				if (this.vm.symbols) {
					const labels = this.vm.symbolsAt(byte).sort();
					const cell = $("<td></td>").appendTo(tr);
					if (labels.length != 0) {
						cell.text(labels[0].replace(/(.)_/g, "$1\u00a0"));
						if (labels.length > 1) {
							cell.attr("title", labels.slice(1).join(", "));
						}
					}
				}
			});
		});
	}

	set symbolTableEdges(to) {
		this._symbolTableEdges = to;
	}

	get symbolTableEdges() { // returns byte addresses.
		if (this._symbolTableEdges) {
			return this._symbolTableEdges;
		}

		this._symbolTableEdges = [];

		for (let i = this.vm.offsets.$symtab / 8, j = 0; i < this.vm.offsets.$handlers / 8 && j < 10000; j++) {
			i += 2 + (this.vm.initial[i].toUnsigned().low & 0xffff);
			if (i * 8 < this.vm.offsets.$handlers) {
				this._symbolTableEdges.push(i * 8);
			}
		}

		return this._symbolTableEdges;
	}

	hexCell(long) {
		return _.chunk(long.toUnsigned().toString(16).padStart(16, "0"), 2).map((x) => `<span class="digit-group">${x.join("")}</span>`).join("");
	}

	decompiledCell(long, addr) {
		const {$symtab, $handlers, $code, $data} = this.vm.offsets;

		const inMeta = addr < 40;
		const inSymtab = $symtab <= addr && addr < $handlers;
		const inHandlers = $handlers <= addr && addr < $code;
		const inCode = $code <= addr && addr < $data;

		if (inMeta || inHandlers) {
			return $("<a></a>").attr({href: "#"}).addClass("handler").text(long.toString()).click(() => {
				this.vm.programCounter = long.toInt();
				this.highlightProgramCounter();
			});
		}

		const edges = [$symtab, ...this.symbolTableEdges];
		if (inSymtab) {
			if (edges.includes(addr)) { // first word: hash, length
				const hash = long.high.toString(16);
				const lengthNum = long.low & 0xffff;
				let lengthStr = lengthNum.toString();
				if (lengthNum < 5) {
					const pad = `<span class="glyphicon glyphicon-flash"></span>`;
					lengthStr = "".padEnd(pad.length * lengthNum, pad);
				}

				return `<span class="hash">${hash}</span> ${lengthStr}`;
			} else if (edges.includes(addr - 8)) { // first word: position
				return `<span class="handler">${long.toString()}</span>`;
			} else if (edges.includes(addr - 16)) { // at the first word of the symbol name
				const words = [];
				for (let i = addr; !edges.includes(i) && i < $handlers; i += 8) {
					words.push(this.vm.getWord(i));
				}

				// Include the entire symbol name in the first row.
				return `<span class="symbol-name">${words.map((w) => this.long2str(w)).join("")}</span>`;
			}

			// Don't show anything on subsequent rows.
			return "";
		}

		if (inCode) {
			if (long.equals(0)) {
				return "";
			}

			try {
				return ansiHTML(Parser.formatInstruction(long, this.vm.symbols))
					.replace(/#e8bf03/g, "orange")
					.replace(/#ff00ff/g, "#f08")
					.replace(/#00ffee/g, "#00bfff")
					.replace(/#ff0000/g, "#a00")
					.replace(/<span style="[^"]*">\$(rt|[fs]p|0|g|lo|hi)<\/span>/g, ($0, $1) => `<span class="reg-${$1}">$${$1}</span>`)
					.replace(/<span style="[^"]*">\$(([ratskemf])[0-9a-f]+)<\/span>/g, ($0, $1, $2) => `<span class="reg-${$2}x">$${$1}</span>`)
					.replace(/^(<<span)([^>]*>)(halt|pr[cdxb]|sleep)( |<)/, ($0, $1, $2, $3, $4) => `${$1} class="has-title" title="${text[$3]}"` + $2 + $3 + $4)
					.replace(/^(<span style=")([^"]+")><span style="font-weight:bold;">(\w+)<\/span>/, ($0, $1, $2, $3) => $3 in text? `${$1}font-style:italic;${$2}" class="has-title" title="${text[$3]}">` + $3 : $0)
					.replace(/^(<span style="[^"]+")>(\w+)( |<)/, ($0, $1, $2, $3) => $2 in text? `${$1} class="has-title" title="${text[$2]}">` + $2 + $3 : $0);
			} catch(e) {
				return `<span class="what">?</span>`;
			}
		}

		if (this.config.attemptUTF8) {
			try {
				return decodeURIComponent(escape(atob(String.fromCharCode(..._.chunk(long.toString(16).padStart(16, "0"), 2).map((x) => parseInt(x.join(""), 16) || ".".charCodeAt(0))))));
			} catch(e) {
				// Couldn't decode; just try to asciify it
			}
		}

		return this.long2str(long, false);
	}

	long2str(long, noDim=true) {
		return _.chunk(long.toUnsigned().toString(16).padStart(16, "0"), 2).map((x) => {
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

			return UNPRINTABLE.includes(parsed) && noDim === false? `<span class="dim">.</span>` : String.fromCharCode(parsed);
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
		this._symbolTableEdges = null;

		if (this.config.excludeSymtab) {
			this.range = [
				[0, this.vm.offsets.$symtab - 8],
				[this.vm.offsets.$handlers, this.vm.offsets.$end + (32 - 1) * 8],
				[8 * this.vm.memorySize - 128, 8 * this.vm.memorySize - 8]
			].map((x) => x.join("-")).join(";");
		} else {
			this.range = [
				[0, this.vm.offsets.$end + (32 - 1) * 8],
				[8 * this.vm.memorySize - 128, 8 * this.vm.memorySize - 8]
			].map((x) => x.join("-")).join(";");
		}

		this.displayRegisters();

		(this.vm.onTick = this.onTickUI.bind(this))();
		this.vm.onSetWord = this.onSetWord.bind(this);
		this.vm.onSetByte = this.onSetByte.bind(this);
		this.vm.log = this.log.bind(this);
		this.vm.stop = this.stop.bind(this);
	}

	onTickUI() {
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
			
			let breakFn;
			if (breakFn = this.onTick()) {
				breakFn();
			}

			if (!this.vm.active) {
				this.active = false;
				if (this.heartrate >= 0) {
					this.vm.onTick = this.onTickUI.bind(this);
				}
			}
		}
	}

	onTick() {
		if (this.breakpoints.includes(this.vm.programCounter)) {
			this.vm.active = false;
			this.onTickUI();
			this.toggleActive();
		}

		if (this.isAtSubroutineEnd()) {
			this.vm.tick();
			this.active = false;
			this.skippedSubroutine = null;
			return this.restoreHeartrate();
		}

		if (!this.skippedSubroutine && this.config.skipStrprint) {
			if (this.currentLabel == "strprint" && -1000 <= this.heartrate) {
				return this.stashHeartrate();
			}
			
			if (this.currentLabel == "strprint$end" && this.tempHeartrate != null) {
				return this.restoreHeartrate();
			}
		}
	}

	stashHeartrate() {
		this.tempHeartrate = this.heartrate;
		this.heartrate = -2000;
		return () => this.startHeartbeat();
	}

	restoreHeartrate() {
		this.heartrate = this.tempHeartrate;
		this.tempHeartrate = null;
		this.resetOnTickUI()();
		return () => this.startHeartbeat();
	}

	startHeartbeat() {
		if (!this.active) {
			return;
		}

		clearInterval(this.interval);
		this.heartbeatActive = true;
		let rate = this.heartrate;

		if (-1000 <= this.heartrate && this.heartrate < 0) {
			// If the heartrate is slightly negative, don't bother with updating the UI.
			this.vm.onTick = () => {};
			rate = 0;
		}

		if (this.heartrate < -1000) {
			// If the heartrate is really negative, don't even bother with the event loop.
			// Hopefully you're not planning on executing an infinite loop.
			this.vm.onTick = () => {};
			let breakFn;
			while (this.vm.active) {
				this.vm.tick();
				
				if (breakFn = this.onTick()) {
					break;
				}
			}

			if (breakFn) {
				breakFn();
			} else {
				this.active = false;
				this.resetOnTickUI()();
			}
		} else {
			this.interval = setInterval(() => this.heartbeat(), rate);
		}
	}

	get currentLabel() {
		return _.findKey(this.vm.symbols, (v) => v[1].toInt() == this.vm.programCounter);
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

	resetOnTickUI(force=false) {
		if (force || 0 <= this.heartrate) {
			return this.vm.onTick = this.onTickUI.bind(this);
		}

		return () => {};
	}

	isAtSubroutineCall() {
		const {op, flags, addr} = Parser.parseInstruction(this.vm.loadInstruction());
		if (op == "jl" && flags == FLAGS.KNOWN_SYMBOL) {
			const label = _.findKey(this.vm.symbols, (v) => v[1].toInt() == addr);
			return !!label && label + "$end" in this.vm.symbols && label;
		}

		return false;
	}

	isAtSubroutineEnd() {
		const label = this.currentLabel;
		const sub = this.skippedSubroutine;
		if (!sub || !label) {
			return false;
		}

		if (sub + "$end" in app.vm.symbols) {
			return app.vm.symbols[sub + "$end"][1].eq(this.vm.programCounter);
		}
		
		return app.vm.symbols[sub + "$done"][1].eq(this.vm.programCounter);
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

	loadBreakpoints() {
		try {
			const bp = JSON.parse(localStorage.getItem("breakpoints"));
			if (bp instanceof Array) {
				return this.breakpoints = bp;
			}
		} catch(e) { }

		localStorage.setItem("breakpoints", "[]");
		return this.breakpoints = [];
	}

	saveBreakpoints() {
		if (this.breakpoints instanceof Array) {
			localStorage.setItem("breakpoints", JSON.stringify(this.breakpoints));
			return true;
		}
		
		return false;
	}

	applyBreakpoints() {
		$(".addr-cell").removeClass("breakpoint");
		if (this.breakpoints) {
			for (const bp of this.breakpoints) {
				$(`.addr-cell-${bp}`).addClass("breakpoint");
			}
		}
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

			const radix = {b: 2, t: 3, q: 4, o: 8, h: 16, x: 16}[input[0]] || 10;
			const unsigned = input[input.length - 1] == "u";
			const long = app.vm.registers[i] = Long.fromString(input.substring(radix != 10, input.length - (unsigned? 1 : 0)), unsigned, radix);
			valcell.text(long.toString(10));

			if (i == REGISTER_OFFSETS.stack) {
				app.highlightStackPointer();
			}
		});

		row.append(namecell).append(valcell).click(() => row.toggleClass("active-register"));
	}

	$("#format span").click(() => {
		let old = localStorage.formatStyle + "";
		const newStyle = localStorage.formatStyle == "mnem"? "wasm" : "mnem";
		localStorage.formatStyle = newStyle;
		Parser.formatStyle = newStyle;
		app.renderFormatLink();
		app.displayMemory();
	});

	app.renderFormatLink();

	$("#top").split({orientation: "vertical", limit: 210, position: "81.5%"});
	$(".hsplitter").height(4);
	$(".vsplitter").width(2);

	$("#step").click(() => app.vm.tick());
	$("#set_range").click(() => {
		let input = prompt("Range:", app.range);
		if (input) {
			app.range = input;
		}
	});

	$("#flush_buffer").click(() => app.vm.flushPrcBuffer(true));

	$("#run").click(() => {
		$("#run")[app.toggleActive()? "addClass" : "removeClass"]("active");
	}).contextmenu((event) => {
		event.preventDefault();
		let old = app.active;
		app.active = false;
		let answer = parseFloat(prompt("Time between heartbeats (s):", app.config.heartrate / 1000));
		if (isNaN(answer)) {
			alert("Invalid number.");
		} else {
			app.config.heartrate = answer * 1000;
		}

		app.active = old;
	});

	$("#step_over").click(() => {
		const sr = app.isAtSubroutineCall();
		if (sr) {
			app.skippedSubroutine = sr;
			app.stashHeartrate();
			app.active = true;
			app.vm.active = true;
			app.startHeartbeat();
		} else {
			app.vm.tick();
		}
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
	app.applyBreakpoints();
}

let parser = new Parser();
parser.read(fs.readFileSync(__dirname + "/../../wasm/compiled/string.why", "utf8"));
let {offsets, handlers, meta, code, symbols} = parser;
let app, vm = window.vm = new WVM({program: {offsets, handlers, meta, code, symbols}, memory: parser.raw});

if (!localStorage.formatStyle) {
	localStorage.formatStyle = Parser.defaultStyle;
}

Parser.formatStyle = localStorage.formatStyle;

$(() => {
	let bp = [];
	try {
		bp = JSON.parse(window.localStorage.breakpoints);
	} catch(e) { }
	app = window.app = new App(vm, {});
	app.config.consoleSize[0] = Math.max(10, Math.floor(($("#console").width()) / 8));
	app.config.consoleSize[1] = Math.max(10, Math.floor(($("#console").height() - 16) / 16 - 3));
	initializeUI(app);
	setTimeout(() => app.applyBreakpoints());
	app.initializeVM();
});
