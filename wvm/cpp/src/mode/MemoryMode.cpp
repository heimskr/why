#include <algorithm>
#include <sstream>

#include "lib/ansi.h"
#include "mode/MemoryMode.h"
#include "Unparser.h"
#include "Util.h"

namespace WVM::Mode {
	void MemoryMode::run(const std::string &hostname, int port) {
		ClientMode::run(hostname, port);

		networkThread = std::thread([&]() {
			loop();
		});

		terminal.on_interrupt = [this]() {
			stop();
			terminal.mouse(haunted::mouse_mode::none);
			return true;
		};

		expando.emplace(&terminal, haunted::ui::boxes::box_orientation::vertical,
			std::initializer_list<haunted::ui::boxes::expandobox::child_pair> {{&textbox, -1}});

		textbox.key_fn = [&](const haunted::key &key) {
			if (key == haunted::kmod::ctrl) {
				if (key == 't') {
					*buffer << ":Tick\n";
				} else if (key == 'g') {
					*buffer << ":GetMain\n";
				} else if (key == 'b') {
					remakeList();
				} else if (key == 'r') {
					*buffer << ":Reset\n:GetMain\n";
				} else if (key == 'f') {
					follow = !follow;
					if (follow)
						jumpToPC();
				} else if (key == 'p') {
					jumpToPC();
				} else if (key == 'l') {
					terminal.redraw();
				} else return false;
			} else if (key == '.') {
				*buffer << ":Tick\n";
			} else return false;
			return true;
		};

		terminal.start_input();
		terminal.set_root(&*expando);
		textbox.set_autoscroll(false);
		terminal.mouse(haunted::mouse_mode::motion);
		textbox.focus();
		expando->draw();
		terminal.watch_size();
		*buffer << ":GetMain\n";
		*buffer << ":Subscribe memory\n";
		*buffer << ":Subscribe pc\n";
		terminal.join();
		networkThread.join();
	}

	void MemoryMode::remakeList() {
		textbox.clear_lines();
		lines.clear();
		std::stringstream stream;
		if (min % 8 || max % 8)
			throw std::runtime_error("MemoryMode min/max are misaligned");

		textbox.draw();
		textbox.suppress_draw = true;

		const int height = textbox.get_position().height;

		std::thread loop = std::thread([&]() {
			for (Word address = min, i = 0; address < max; address += 8, ++i) {
				if (address == vm.symbolsOffset || address == vm.codeOffset || address == vm.dataOffset
				    || address == vm.endOffset) {
					std::string str = "\e[2m";
					str.reserve(50 + padding);
					for (int i = 0; i < 50 + padding; ++i)
						str += "-";
					textbox += str + "\e[22m";
				}
				textbox += stringify(address);
				lines.emplace(address, textbox.get_lines().back());
				if (i == height) {
					textbox.suppress_draw = false;
					textbox.draw();
					textbox.suppress_draw = true;
				}
			}
		});

		loop.join();
		textbox.suppress_draw = false;
		terminal.redraw();
	}

	std::string MemoryMode::stringify(Word address) const {
		std::stringstream ss;
		UWord word = vm.getWord(address, Endianness::Big);
		if (vm.programCounter == address)
			ss << "\e[48;5;22m";
		ss << "\e[2m[" << std::setw(padding) << std::setfill(' ') << address << "]\e[22;90m  0x\e[39m";
		ss << std::setw(16) << std::setfill('0') << std::hex << word << "  " << std::dec;

		if (address < 32) {
			ss << "\e[38;5;26m" << word << "\e[39m";
		} else if (address < vm.symbolsOffset || vm.dataOffset <= address) {
			for (int i = 0; i < 8; ++i) {
				char ch = static_cast<char>(vm.getByte(address + i));
				if (ch < 32)
					ss << "\e[2m.\e[22m";
				else
					ss << ch;
			}
		} else if (address < vm.codeOffset) {
			if (symbolTableEdges.count(address) == 1) {
				const UHWord hash = word >> 32;
				ss << std::hex << "\e[2m" << hash << "\e[22m" << std::dec;
			} else if (symbolTableEdges.count(address - 8) == 1) {
				ss << word;
			} else if (symbolTableEdges.count(address - 16) == 1) {
				ss << "\e[35m" << vm.getString(address, 8 * vm.getHalfword(address - 12, Endianness::Big)) << "\e[39m";
			}
		} else if (vm.codeOffset <= address && address < vm.dataOffset) {
			ss << Unparser::stringify(word, &vm);
		}

		ss << "\e[49m";
		return ss.str();
	}

	void MemoryMode::updateLine(Word address) {
		haunted::ui::simpleline &simple = getLine(address);
		simple.text = stringify(address);
		textbox.redraw_line(simple);
	}

	void MemoryMode::makeSymbolTableEdges() {
		symbolTableEdges.clear();
		symbolTableEdges.insert(vm.symbolsOffset);
		for (Word i = vm.symbolsOffset; i < vm.codeOffset;) {
			i += 16 + 8 * vm.getHalfword(i + 4, Endianness::Big);
			if (i < vm.codeOffset)
				symbolTableEdges.insert(i);
		}
	}

	void MemoryMode::stop() {
		ClientMode::stop();
	}

	void MemoryMode::setFastForward(bool to) {
		terminal.suppress_output = fastForward = to;
	}

	void MemoryMode::handleMessage(const std::string &message) {
		if (message.front() != ':') {
			DBG("Not sure how to handle [" << message << "]");
			return;
		}

		const size_t space = message.find(' ');
		const std::string verb = message.substr(1, space - 1);
		const std::string rest = space == std::string::npos? "" : message.substr(space + 1);
		const std::vector<std::string> split = Util::split(rest, " ", false);
		const size_t size = split.size();

		if (verb == "MemoryWords") {
			Word start, count;
			UWord uword;
			if (size < 2 || !Util::parseLong(split[0], start) || !Util::parseLong(split[1], count)
			    || static_cast<Word>(size) != 2 + count) {
				DBG("Invalid MemoryWords content: [" << rest << "]");
				return;
			}

			if (start < min)
				min = start - (start % 8);

			if (max < start + count * 8)
				max = start + count * 8;

			vm.reserve(start + count * 8);
			for (Word address = start, i = 0; i < count; address += 8, ++i) {
				if (!Util::parseUL(split[i + 2], uword, 16))
					DBG("Invalid word at index " << i << ": " << split[i + 2]);
				else
					vm.setWord(address, uword, Endianness::Little);
			}

			padding = std::to_string(max).size();

			vm.init();
			vm.loadSymbols();
			makeSymbolTableEdges();
		} else if (verb == "PC") {
			Word to;
			if (size != 1 || !Util::parseLong(split[0], to)) {
				DBG("Bad message (" << verb << "): [" << rest << "]");
				return;
			}

			Word old_pc = vm.programCounter;
			vm.programCounter = to;
			if (!fastForward) {
				if (lines.count(old_pc) == 1)
					updateLine(old_pc);
				if (lines.count(to) == 1)
					updateLine(to);
				if (follow)
					jumpToPC();
			}
		} else if (verb == "FastForward") {
			if (size != 1) {
				DBG("Invalid: FastForward[" << rest << "]");
				return;
			}

			if (split[0] == "on") {
				setFastForward(true);
			} else if (split[0] == "off") {
				setFastForward(false);
			} else {
				DBG("Invalid: FastForward[" << rest << "]");
			}
		}
	}

	void MemoryMode::jumpToPC() {
		try {
			haunted::ui::simpleline &simple = getLine(vm.programCounter);
			int rows = 0;
			for (haunted::ui::textbox::line_ptr &line: textbox.get_lines()) {
				if (line.get() == &simple)
					break;
				rows += textbox.line_rows(*line);
			}

			textbox.vscroll(rows - textbox.get_position().height / 2 - textbox.get_voffset());
		} catch (const std::out_of_range &) {
			return;
		}
	}

	haunted::ui::simpleline & MemoryMode::getLine(Word address) {
		if (lines.count(address) == 0)
			throw std::out_of_range("Line " + std::to_string(address) + " is missing");

		std::shared_ptr<haunted::ui::textline> &line = lines.at(address);

		if (haunted::ui::simpleline *simple = dynamic_cast<haunted::ui::simpleline *>(line.get())) {
			return *simple;
		} else throw std::runtime_error("Unable to cast line to haunted::ui::simpleline");
	}
}
