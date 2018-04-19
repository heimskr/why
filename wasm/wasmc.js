#!/usr/bin/env node
let fs = require("fs"),
	nearley = require("nearley"),
	chalk = require("chalk"),
	getline = require("get-line-from-pos"),
	minimist = require("minimist"),
	Long = require("long"),
	_ = require("lodash");

/**
 * `wasm` is the assembly language for Why.js. The `wasmc` utility parses it and compiles it to `wvm` bytecode.
 * 
 * @module wasm
 */

require("string.prototype.padstart").shim();
require("string.prototype.padend").shim();

const { EXCEPTIONS, R_TYPES, I_TYPES, J_TYPES, OPCODES, FUNCTS, REGISTER_OFFSETS, MAX_ARGS, FLAGS, TRAPS } = require("./constants.js");
const isLabelRef = (x) => x instanceof Array && x.length == 2 && x[0] == "label";

/**
 * Represents a `wasmc` instance.
 */
class WASMC {
	
	/**
	 * Creates a new wasmc instance.
	 * @param {Object} options - An object containing options for the compiler (from the command line, for example).
	 * @param {string} filename - The filename to read the wasm source from.
	 */
	constructor(options, filename) {
		/**
		 * An object containing options for the compiler (from the command line, for example).
		 * @type {Object.<string, *>}
		 * @name module:wasm~WASMC#options
		 */
		this.options = options;

		/**
		 * The filename to read the wasm source from.
		 * @type {string}
		 * @name module:wasm~WASMC#filename
		 */
		this.filename = filename;

		/**
		 * Whether to prevent adding linker flags to the bytecode.
		 * @type {boolean}
		 * @name module:wasm~WASMC#ignoreFlags
		 * @default
		 */
		this.ignoreFlags = true;

		/**
		 * Contains the abstract syntax tree once {@link module:wasm~WASMC#parse WASMC.parse()} is called.
		 * @type {Object}
		 * @name module:wasm~WASMC#parsed
	 	 */
		this.parsed = { };

		/**
		 * Contains a list of offsets/labels.
		 * @type {Object.<string, number>}
		 * @name module:wasm~WASMC#offsets
		 */
		this.offsets = { };

		/**
		 * Contains the program's metadata as an array of Longs. See the ISA documentation for the layout.
		 * @type {Long[]}
		 * @name module:wasm~WASMC#meta
		 */
		this.meta = [];

		/**
		 * Contains the program's data as an array of Longs.
		 * @type {Long[]}
		 * @name module:wasm~WASMC#data
		 */
		this.data = [];

		/**
		 * Contains the program's bytecode as an array of Longs.
		 * @type {Long[]}
		 * @name module:wasm~WASMC#code
		 */
		this.code = [];
	}

	/**
	 * Loads the Nearley grammar, parses the source file and stores the AST in {@link module:wasm~WASMC#parsed parsed}.
	 */
	parse() {
		let grammar;
		try {
			grammar = require("./wasm.js");
		} catch (e) {
			console.error("Couldn't read wasm.js.");
			if (this.options.debug) {
				console.error(e);
			}

			process.exit(1);
		}

		const parser = new nearley.Parser(grammar.ParserRules, grammar.ParserStart);
		const source = fs.readFileSync(this.filename, "utf8") + "\n";

		let trees;
		try {
			trees = parser.feed(source).results;
		} catch (e) {
			console.error(chalk.red("Syntax error"), "at", chalk.white(`${getline(source, e.offset)}:${e.offset - source.split(/\n/).slice(0, getline(source, e.offset) - 1).join("\n").length}`) + ":");
			if (this.options.dev) {
				console.log(e.message.replace(/\(@(\d+):([^)]+)\)/g, ($0, $1, $2) => { const _line = getline(source, e.offset); return `(@${_line}:${e.offset - source.split(/\n/).slice(0, _line).join("\n").length + $2})`; }));
			}

			process.exit(1);
		}

		if (trees.length > 1) {
			trees.forEach((tree) => console.log(JSON.stringify(trees[tree], null, 4)));
			console.error(chalk.red.italic(`\nAmbiguous grammar (${trees.length}).\n`));
			process.exit(1);
		} else if (trees.length === 0) {
			console.warn(chalk.red.italic("Nothing parsed."));
			process.exit(1);
		}

		this.parsed = trees[0];

		if (typeof this.parsed != "object") {
			WASMC.die("Error: parser output isn't an object.");
		}

		if (typeof this.parsed.metadata == "undefined") {
			this.parsed.metadata = { };
		}

		if (typeof this.parsed.data == "undefined") {
			this.parsed.data = { };
		}

		if (typeof this.parsed.code == "undefined") {
			this.parsed.code = { };
		}
	}

	/**
	 * {@link module:wasm~WASMC#parse Parses} and processes the source code and writes the output.
	 */
	compile() {
		this.parse();
		this.processMetadata();
		this.processHandlers();
		this.processData();
		this.processCode(this.expandLabels(this.expandCode()));

		this.meta[3] = Long.fromInt(this.getEnd(this.code.length), true);
		const out = this.meta.concat(this.handlers).concat(this.data).concat(this.code);

		if (this.options.debug) {
			console.log({
				meta: WASMC.longs2strs(this.meta),
				handlers: WASMC.longs2strs(this.handlers),
				data: WASMC.longs2strs(this.data),
				code: WASMC.longs2strs(this.code),
				out: WASMC.longs2strs(out),
				offsets: this.offsets
			});
		} else {
			let outname = typeof this.options.out != "string"? this.filename.replace(/\.wasm$/i, "") + ".why" : this.options.out;

			let frozen = this.options.library? JSON.stringify({
				meta: this.parsed.meta,
				labels: _.omitBy(this.offsets, (val, key) => key[0] == "_"),
				program: WASMC.longs2strs(out).join(" ")
			}) : WASMC.longs2strs(out).join("\n");

			fs.writeFileSync(outname, frozen);
			console.log(chalk.green("\u2714"), "Successfully assembled", chalk.bold(this.filename), `${this.options.library? "(library) " : ""}and saved the output to`, chalk.bold(outname) + ".");
		}
	}

	/**
	 * Extracts and processes the program's metadata and stores it in {@link module:wasm~WASMC#meta meta}.
	 */
	processMetadata() {
		let [name, version, author] = [this.parsed.meta.name || "?", this.parsed.meta.version || "?", this.parsed.meta.author || "?"];

		const orcid = typeof this.parsed.meta.orcid == "undefined"? "0000000000000000" : this.parsed.meta.orcid.replace(/\D/g, "");
		if (orcid.length != 16) {
			WASMC.die("Error: invalid ORCID.");
		}

		// Convert the ORCID into two Longs and stash them in the correct positions in meta.
		[this.meta[4], this.meta[5]] = [orcid.substr(0, 8), orcid.substr(8)].map((half) => WASMC.chunk2long(half.split("")));

		// Append the name-version-author string.
		this.meta = this.meta.concat(WASMC.str2longs(`${name}\0${version}\0${author}\0`));
		
		// The beginning of the handler pointer section comes right after the end of the meta section.
		this.meta[0] = Long.fromInt(this.meta.length * 8, true);

		// The handlers section is exactly as large as the set of exceptions; the data section begins
		// at the sum of the lengths of the meta section and the handlers section.
		this.meta[1] = Long.fromInt((this.meta.length + EXCEPTIONS.length) * 8);

		// Library compilation can be triggered either via inclusion of "library" in
		// the program's #data section or by use of the --library option.
		this.options.library = !(this.ignoreFlags = !(this.parsed.meta.library || this.options.library));

		this.log({ meta: this.meta, version, author });
	}

	/**
	 * Will eventually extract and process the program's handlers and store it, but is currently just a placeholder.
	 */
	processHandlers() {
		this.handlers = [...Array(EXCEPTIONS.length)].map(() => Long.UZERO);
	}

	/**
	 * Extracts and processes the program's data and stores it in {@link module:wasm~WASMC#data data}.
	 */
	processData() {
		let offset = this.meta[1].toInt();
		_(this.parsed.data).forEach(([type, value], key) => {
			let pieces;
			if (type.match(/^(in|floa)t$/)) {
				pieces = [Long.fromValue(value)];
			} else if (type == "string") {
				pieces = WASMC.str2longs(value);
			} else {
				WASMC.die(`Error: unknown data type "${type}" for "${key}".`);
			}

			this.offsets[key] = offset;
			this.log(chalk.yellow("Assigning"), offset, "to", key);
			this.data = this.data.concat(pieces);
			offset += pieces.length * 8;
		});

		this.meta[2] = Long.fromInt(offset);
	}

	/**
	 * Copies an array of expanded code into the {@link module:wasm~WASMC#code main code array}.
	 * @param {Array} expanded - The list of expanded instructions to compile and store.
	 */
	processCode(expanded) {
		expanded.forEach((item) => {
			this.addCode(item);
		});
	}

	/**
	 * Returns a copy of {@link module:wasm~WASMC#parsed parsed}.code with all pseudoinstructions expanded.
	 * @return {Object[]} An array of expanded instructions.
	 */
	expandCode() {
		let expanded = [];
		// In the first pass, we expand pseudoinstructions into their constituent parts. Some instructions will need to be
		// gone over once again after labels have been sorted out so we can replace variable references with addresses.
		this.parsed.code.forEach((item) => {
			let [label, op, ...args] = item;
			if (label) {
				this.offsets[label] = this.meta[2].toInt() + expanded.length * 8;
				this.log(chalk.magenta("Assigning"), this.offsets[label], "to", label, "based on an expanded length equal to", chalk.bold(expanded.length));
			}

			const add = (x) => expanded.push(x);

			const addPush = (args, _label=label) => {
				const getLabel = () => [_label, _label = null][0];
				args.forEach((reg) => {
					add([getLabel(), "s", _0, reg, _SP]);
					add([null, "subi", _SP, _SP, 8]);
				});
			};

			const addPop = (args, _label=label) => {
				const getLabel = () => [_label, _label = null][0];
				args.forEach((reg) => {
					add([getLabel(), "addi", _SP, _SP, 8]);
					add([null, "l", _0, _SP, reg]);
				});
			};

			if (op == "call") {
				let [name, ...vals] = args;

				// There can't be more arguments than the set of argument registers can handle.
				if (MAX_ARGS < vals.length) {
					throw new Error(`Too many arguments given to subroutine (given ${vals.length}, maximum is ${MAX_ARGS})`);
				}

				// Push the current return address and the current values in $a_0...$a_{n-1} (in that order) to the stack.
				addPush([_RA, ..._.range(0, vals.length).map((n) => _A[n])], label);

				// For each argument in the subroutine call, set its corresponding
				// argument register based on the type of the argument. (An argument
				// can be an immediate value, a register, a variable dereference or
				// a label/variable reference.)
				vals.forEach((val, i) => {
					if (typeof val == "number") {
						add([null, "set", _0, _A[i], Long.fromInt(val).toUnsigned().toInt()]);
					} else if (!(val instanceof Array)) {
						throw new Error(`Invalid value for argument ${i + 1}: ${JSON.stringify(val)}`);
					} else if (val[0] == "register") {
						add([null, "or", val, _0, _A[i]]);
					} else if (val[0] == "label") {
						add([null, "li", _0, _A[i], val]);
					} else if (val[0] == "address") {
						add([null, "set", _0, _A[i], ["label", val[1]]]);
					} else {
						throw new Error(`Invalid value for argument ${i + 1}: ${JSON.stringify(val)}`);
					}
				});

				// Store the program counter in $rt and jump to the subroutine.
				add([null, "jl", _0, ["label", name]]);

				// Now that we've returned from the subroutine, pop the values we pushed earlier, but in reverse order.
				addPop([..._.range(vals.length - 1, -1, -1).map((n) => _A[n]), _RA], null);
			} else if (op == "trap") {
				const funct = args[3];
				if (funct == TRAPS.prc && args[1][0] == "char") {
					add([label, "set", _0, _M[2], args[1][1]]);
					add([null, "trap", 0, ["register", "m", 2], 0, TRAPS.prc]);
				} else {
					add([label, "trap", ...args]);
				}
			} else if (op == "mv") {
				add([label, "or", args[0], _0, args[1]]);
			} else if (op == "push") {
				addPush(args);
			} else if (op == "pop") {
				addPop(args);
			} else if (op == "jeq") {
				const right = args[0];
				const left = args[1];
				if (isLabelRef(right)) {
					add([label, "li", _0, _M[2], right]);
					add([null, "seq", _M[2], left, _M[0]]);
				} else if (typeof right == "number") {
					add([label, "seqi", left, _M[0], right]);
				} else {
					add([label, "seq", right, left, _M[0]]);
				}

				if (args[2][0] == "value") {
					// Set $m1 to the immediate value and then conditionally jump to $m1.
					add([null, "set", _0, _M[1], args[2][1]]);
					add([null, "jrc", _0, _M[0], _M[1]]);
				} else if (args[2][0] == "register") {
					// We're already given a register, so we don't have to do anything with $m1.
					add([null, "jrc", _0, _M[0], args[2]]);
				} else if (args[2][0] == "label") {
					// Load the value of the given variable into $m1 and then conditionally jump to $m1.
					this.log("jeq with label:", args[2]);
					add([null, "set",  _0, _M[1], args[2]]);
					add([null, "jrc", _0, _M[0],   _M[1]]);
				}
			} else if (R_TYPES.includes(OPCODES[op]) && _.some(args, isLabelRef)) {
				let [rt, rs, rd] = args;
				let [lt, ls, ld] = [rt, rs, rd].map(isLabelRef);
				let _label = label, getLabel = () => [_label, _label = null][0];
				[rt, rs].forEach((reg, i) => {
					if (isLabelRef(reg)) {
						// Whoops, this register isn't actually a register
						add([getLabel(), "li", _0, _M[i], reg]);
					}
				});

				add([getLabel(), op, ...[rt, rs, rd].map((reg, i) => [lt, ls, ld][i]? _M[i] : reg)]);

				if (ld) {
					add([getLabel(), "si", _M[2], _0, rd]);
				}
			} else if (I_TYPES.includes(OPCODES[op]) && _.some(args, isLabelRef)) {
				let [rs, rd, imm] = args;
				let [ls, ld] = [rs, rd].map(isLabelRef);
				let _label = label, getLabel = () => [_label, _label = null][0];
				if (ls) {
					add([getLabel(), "li", _0, _M[0], rs]);
				}

				add([getLabel(), op, ...[rs, rd].map((reg, i) => [ls, ld][i]? _M[i] : reg), imm]);

				if (ld) {
					add([getLabel(), "si", _M[1], _0, rd]);
				}
			} else {
				add(item);
			}
		});

		this.offsets[".end"] = this.getEnd(expanded.length);
		return expanded;
	}

	/**
	 * Returns the address of the beginning of the program memory section.
	 * @param  {number} instructionCount The number of code instructions. 
	 * @return {number} The address of the first byte after the end of the code section.
	 */
	getEnd(instructionCount) {
		if (!this.handlers) {
			this.processHandlers();
		}
		
		return 8 * ([this.meta, this.handlers, this.data].reduce((a, b) => a + b.length, 0) + instructionCount);
	}

	/**
	 * Replaces all label references in a given array of expanded instructions with the corresponding memory addresses.
	 * Mutates the input array.
	 * @param {Object[]} expanded - An array of expanded instructions (see {@link module:wasm~WASMC#expandCode expandCode}).
	 * @return {Object[]} The mutated input array with label references replaced with memory addresses.
	 */
	expandLabels(expanded) {
		// In the second pass, we replace label references with the corresponding
		// addresses now that we know the address of all the labels.
		expanded.forEach((item) => {
			// First off, now that we've recorded all the label positions,
			// we can remove the label tags.
			item.shift();

			// Look at everything past the operation name (the arguments).
			item.slice(1).forEach((arg, i) => {
				// If the argument is a label reference,
				if (isLabelRef(arg)) {
					// replace it with an address from the offsets map. 
					item[i + 1] = this.offsets[arg[1]];
					item.flags = FLAGS.ADJUST_ADDRESS;
				}
			});
		});

		return expanded;
	}

	/**
	 * Compiles an instruction and adds it to the {@link module:wasm~WASMC#code code array}.
	 * @param {Object} item - The instruction to compile and add.
	 */
	addCode(item) {
		this.code.push(this.compileInstruction(item));
	}

	/**
	 * Compiles an array representation of an instruction into a Long containing the bytecode.
	 * @param {Array} instruction - An uncompiled instruction.
	 * @return {Long} The bytecode representation of the instruction.
	 */
	compileInstruction(instruction) {
		const [op, ...args] = instruction;
		const { flags } = instruction;
		if (op == "trap") {
			return this.rType(OPCODES.trap, ...args.slice(0, 3).map(WASMC.convertRegister), args[3], flags);
		} else if (R_TYPES.includes(OPCODES[op])) {
			return this.rType(OPCODES[op], ...args.map(WASMC.convertRegister), FUNCTS[op], flags);
		} else if (I_TYPES.includes(OPCODES[op])) {
			return this.iType(OPCODES[op], ...args.map(this.convertValue, this), flags);
		} else if (J_TYPES.includes(OPCODES[op])) {
			return this.jType(OPCODES[op], ...args.map(this.convertValue, this), flags);
		} else if (op == "nop") {
			return Long.UZERO;
		} else {
			this.warn(`Unhandled instruction ${chalk.bold.red(op)}.`, [op, ...args]);
			return Long.fromString("deadc0de", true, 16);
		}
	}

	/**
	 * Compiles an object representaiton of an instruction into a Long containing the bytecode.
	 * @param {Object} instruction - An uncompiled instruction.
	 * @return {Long} The bytecode representation of the instruction.
	 */
	unparseInstruction(instruction) {
		const { op, type } = instruction;
		if (type == "r") {
			const { opcode, rt, rs, rd, funct, flags } = instruction;
			return this.rType(opcode, rt, rs, rd, funct, flags);
		} else if (type == "i") {
			const { opcode, rs, rd, imm, flags } = instruction;
			return this.iType(opcode, rs, rd, imm, flags);
		} else if (type == "j") {
			const { opcode, rs, addr, flags } = instruction;
			return this.jType(opcode, rs, addr, flags);
		} else if (op == "nop") {
			return Long.UZERO;
		} else {
			this.warn(`Unhandled instruction ${chalk.bold.red(op)}.`, instruction);
			return Long.fromString("deadc0de", true, 16);
		}
	}

	/**
	 * If the input is an array or number accepted by convertRegister, the output is the corresponding register index.
	 * If the input is a string, then the label map is checked and the corresponding address is returned.
	 * @param {(string[]|number|string)} x - The value to convert.
	 * @return {number} The converted value.
	 * @throws Will throw an exception if the input is of an unrecognized type.
	 */
	convertValue(x) {
		if (x instanceof Array || typeof x == "number") {
			return WASMC.convertRegister(x);
		}

		if (typeof x == "string") {
			if (typeof this.offsets[x] == "undefined") {
				throw new Error(`Undefined label: ${x}`);
			}

			return this.offsets[x];
		}

		throw new Error(`Unrecognized value: ${x}`);
	}

	/**
	 * Compiles an R-type instruction into bytecode.
	 * @param {number} opcode - The instruction's opcode.
	 * @param {number} rt - The instruction's secondary source register.
	 * @param {number} rs - The instruction's primary source register.
	 * @param {number} rd - The instruction's destination register.
	 * @param {number} func - The instruction's function field.
	 * @param {number} [flags=0] - The linker flags to embed in the instruction.
	 * @return {Long} The compiled instruction.
	 */
	rType(opcode, rt, rs, rd, func, flags=0) {
		if (!R_TYPES.includes(opcode)) throw new Error(`opcode ${opcode} isn't a valid r-type`);
		if (rt < 0 || 127 < rt) this.warn(`rt (${rt}) not within the valid range (0–127)`);
		if (rs < 0 || 127 < rs) this.warn(`rs (${rs}) not within the valid range (0–127)`);
		if (rd < 0 || 127 < rd) this.warn(`rd (${rd}) not within the valid range (0–127)`);
		if (func < 0 || 4095 < func) this.warn(`func (${func}) not within the valid range (0–4095)`);

		let lower = func | (this.ignoreFlags? 0 : flags << 12) | ((rd & 1) << 31);
		let upper = (rd >> 1) | (rs << 6) | (rt << 13) | (opcode << 20);
		let long = Long.fromBits(lower, upper, true);

		return long;
	}

	/**
	 * Compiles an I-type instruction into bytecode.
	 * @param {number} opcode - The instruction's opcode.
	 * @param {number} rs - The instruction's source register.
	 * @param {number} rd - The instruction's destination register.
	 * @param {number} imm - The instruction's immediate value.
	 * @param {number} [flags=0] - The linker flags to embed in the instruction.
	 * @return {Long} The compiled instruction.
	 */
	iType(opcode, rs, rd, imm, flags=0) {
		if (!I_TYPES.includes(opcode)) throw new Error(`opcode ${opcode} isn't a valid i-type`);
		if (rs < 0 || 127 < rs) this.warn(`rs (${rs}) not within the valid range (0–127)`);
		if (rd < 0 || 127 < rd) this.warn(`rd (${rd}) not within the valid range (0–127)`);
		if (imm < -2147483648 || 2147483647 < imm) this.warn(`imm (${imm}) not within the valid range (-2147483648–2147483647)`);

		let lower = imm;
		let upper = rd | (rs << 7) | (this.ignoreFlags? 0 : flags << 14) | (opcode << 20);
		let long = Long.fromBits(lower, upper);

		return long;
	}

	/**
	 * Compiles a J-type instruction into bytecode.
	 * @param {number} opcode - The instruction's opcode.
	 * @param {number} rs - The instruction's source register.
	 * @param {number} addr - The instruction's address field.
	 * @param {number} [flags=0] - The linker flags to embed in the instruction.
	 * @return {Long} The compiled instruction.
	 */
	jType(opcode, rs, addr, flags=0) {
		if (!J_TYPES.includes(opcode)) throw new Error(`opcode ${opcode} isn't a valid j-type`);
		if (rs < 0 || 127 < rs) this.warn(`rs (${rs}) not within the valid range (0–127)`);
		if (addr < 0 || 4294967295 < addr) this.warn(`addr (${addr}) not within the valid range (0–4294967295)`);

		let lower = addr;
		let upper = (this.ignoreFlags? 0 : flags) | (rs << 13) | (opcode << 20);
		let long = Long.fromBits(lower, upper, true);

		return long;
	}

	/**
	 * Prints a warning.
	 * @param {...*} args - The arguments to pass to console.log.
	 */
	warn(...args) {
		console.log(...args);
	}

	/**
	 * Prints a message if the debug {@link module:wasm~WASMC#options option} is set.
	 * @param {...*} args - The arguments to pass to console.log.
	 */
	log(...args) {
		if (this.options.debug) {
			console.log(...args);
		}
	}

	/**
	 * Prints a message to stderr and exits the process with return code 1.
	 * @param {...*} args - The arguments to pass to `console.error`.
	 */
	static die(...a) { console.error(...a); process.exit(1); }

	/**
	 * Converts an array of 8 characters into a Long.
	 * @param {Array.<string>} chunk - An array of 8 characters.
	 * @return {Long} A Long containing the concatenated ASCII values of the characters.
	 */
	static chunk2long(chunk) {
		return Long.fromString(chunk.map((c) => c.charCodeAt(0).toString(16).padStart(2, "0")).join(""), true, 16);
	}

	/**
	 * Adds nulls to the end of the string to lengthen it to a multiple of 8.
	 * If the string is already a multiple of eight, add one null at the end.
	 * @param {string} str - The string to be nullpadded.
	 * @return {string} The concatenation of the given string and a number of null characters.
	 */
	static nullpad(str) {
		return str.length % 8? str.padEnd(Math.ceil(str.length / 8) * 8, "\0") : `${str}\0`;
	}

	/**
	 * Nullpads and chunks a given string into an array of Longs.
	 * @param {string} str - The string to convert into Longs.
	 * @return {Array.<Long>} An array of Longs representing the input string.
	 */
	static str2longs(str) {
		return str == ""? [Long.UZERO] : _.chunk(WASMC.nullpad(str).split(""), 8).map(WASMC.chunk2long);
	}

	/**
	 * Returns an array containing the 16-length zero-padded hex representations of a given array of Longs.
	 * If any element of the input array isn't a Long value, it will be represented as a string of 16 "x"s.
	 * @param {Array.<Long>} longs - An array of Longs to convert to strings.
	 * @return {Array.<string>} An array of zero-padded hex strings corresponding to the inputs.
	 */
	static longs2strs(longs) {
		return longs.map((l) => l instanceof Long? l.toString(16).padStart(16, "0") : "x".repeat(16));
	}

	/**
	 * If the input is an array (expected format: ["register", ...]), then the output is the number corresponding to
	 * that array. Otherwise, if the input is something other than an array, then the output is same as the input.
	 * @param {Array} x - An array representing a register, such as ["register", "return", 0] for $rt or ["register", "t", 22] for $t16.
	 * @return {number} The ID corresponding to the register.
	 */
	static convertRegister(x) {
		return x instanceof Array? (x.length == 0? 0 : REGISTER_OFFSETS[x[x.length - 2]] + x[x.length - 1]) : x;
	}
}

module.exports = WASMC;
const _A = _.range(0, 16).map((n) => ["register", "a", n]);
const _RA = ["register", "return", 0];
const _M = _.range(0, 16).map((n) => ["register", "m", n]);
const _SP = ["register", "stack", 0];
const _0  = ["register", "zero",  0];

if (require.main === module) {
	const options = minimist(process.argv.slice(2), {
			alias: {
				b: "binary",
				l: "library",
				d: "debug"
			},
			boolean: ["binary", "debug", "library"],
			default: {
				binary: false,
				debug: false,
				library: false
			}
		}), filename = options._[0];

	if (!filename) {
		console.log("Usage: ./wasmc.js <filename> [out]");
		process.exit(0);
	}

	options.out = options._[1];

	new WASMC(options, filename).compile();
}
