#include <algorithm>
#include <sstream>

#include <unistd.h>

#include "lib/ansi.h"
#include "mode/MemoryMode.h"
#include "Unparser.h"
#include "Util.h"

namespace WVM::Mode {
	MemoryMode::~MemoryMode() {
		if (!searching)
			delete textinput;
	}

	void MemoryMode::run(const std::string &hostname, int port) {
		ClientMode::run(hostname, port);

		networkThread = std::thread([&]() {
			loop();
		});

		autotickThread = std::thread([&]() {
			while (true) {
				std::unique_lock<std::mutex> lock(autotickMutex);
				autotickVariable.wait(lock, [this] { return autotickReady; });
				autotickReady = false;

				if (!alive.load())
					break;

				// Strictly speaking I should use a mutex to check autotick, but because I want to avoid the overhead of
				// locking a mutex repeatedly and I don't mind a few accidental extra ticks, I'm deciding not to.
				while (0 <= autotick) {
					*buffer << ":Tick\n";
					usleep(autotick);
					if (vm.hasBreakpoint(vm.programCounter)) {
						autotick = autotick < 0? autotick : -autotick;
						break;
					}
				}
			}
		});

		terminal.onInterrupt = [this]() {
			stop();
			terminal.mouse(Haunted::MouseMode::None);
			return true;
		};

		textbox = new Haunted::UI::Textbox<Container>();
		textinput = new Haunted::UI::TextInput();

		expando = new Haunted::UI::Boxes::ExpandoBox(&terminal, Haunted::UI::Boxes::BoxOrientation::Vertical,
			std::initializer_list<Haunted::UI::Boxes::ExpandoBox::ChildPair> {{textbox, -1}});

		textbox->keyFunction = [&](const Haunted::Key &key) {
			if (key == Haunted::KeyMod::Ctrl) {
				if (key == 't') {
					send(":Tick");
				} else if (key == 'g') {
					send(":GetMain");
				} else if (key == 'b') {
					remakeList();
				} else if (key == 'r') {
					send(":Reset\n" ":GetMain");
				} else if (key == 'f') {
					follow = !follow;
					if (follow)
						jumpToPC();
				} else if (key == 'p') {
					jumpToPC();
				} else if (key == 'l') {
					terminal.redraw();
				} else if (key == 's') {
					showSymbols = !showSymbols;
					remakeList();
				} else return false;
			} else if (key == '.' && autotick < 0) {
				if (vm.paused)
					send(":Unpause");
				send(":Tick");
			} else if (key == ' ') {
				if (autotick < 0)
					startAutotick();
				else
					autotick = -autotick;
			} else if (key == '<') {
				DBG("autotick = " << (autotick *= 1.1));
			} else if (key == '>') {
				DBG("autotick = " << (autotick /= 1.1));
			} else if (key == Haunted::KeyType::PageDown) {
				textbox->vscroll(textbox->getPosition().height);
			} else if (key == Haunted::KeyType::PageUp) {
				textbox->vscroll(-textbox->getPosition().height);
			} else if (key == '[') {
				send(":Undo");
			} else if (key == ']') {
				send(":Redo");
			} else if (key == '/') {
				toggleSearchbox();
			} else if (key == 'd') {
				send(":DebugData " + std::to_string(vm.programCounter));
			} else if (key == 'h') {
				send(":History toggle");
			} else return false;
			return true;
		};

		terminal.startInput();
		terminal.setRoot(expando);
		textbox->setAutoscroll(false);
		terminal.mouse(Haunted::MouseMode::Motion);
		textbox->focus();
		expando->draw();
		terminal.watchSize();
		send(":Subscribe memory\n" ":GetMain\n" ":Subscribe pc\n" ":Subscribe breakpoints\n" ":Subscribe registers");
		send(":Reg " + std::to_string(Why::stackPointerOffset));
		alive = false;
		terminal.join();
		networkThread.join();
		autotickMutex.lock();
		autotickReady = true;
		autotickMutex.unlock();
		autotickVariable.notify_all();
		autotickThread.join();
	}

	void MemoryMode::toggleSearchbox() {
		if (searching) {
			expando->removeChild(textinput);
			expando->resize();
			textbox->focus();
		} else {
			expando->addChild(textinput);
			expando->requestResize(textinput, 1, 1);
			expando->resize();
			textinput->focus();
		}

		searching = !searching;
		DBG("searching is now " << (searching? "on" : "off"));
	}

	void MemoryMode::remakeList() {
		textbox->clearLines();
		lines.clear();
		std::stringstream stream;
		if (min % 8 || max % 8)
			throw std::runtime_error("MemoryMode min/max are misaligned");

		textbox->draw();
		textbox->suppressDraw = true;

		const int height = textbox->getPosition().height;

		std::thread loops = std::thread([&]() {
			const std::string hyphens(42 + padding, '-');
			const std::string spaces(24 + padding, ' ');

			// First, make a set of all addresses that have one or more labels associated with them. This is faster than
			// checking each symbol for each address every time.
			std::unordered_set<Word> labeled;
			if (showSymbols)
				for (const std::pair<const std::string, Symbol> &pair: vm.symbolTable)
					labeled.insert(pair.second.location);

			for (Word address = min, i = 0; address < max + 128 * 8; address += 8, ++i) {
				if (address == vm.symbolsOffset || address == vm.codeOffset || address == vm.dataOffset
				    || address == vm.debugOffset || address == vm.endOffset) {
					*textbox += "\e[2m" + hyphens + "\e[22m";
				}

				if (showSymbols && 0 < labeled.count(address))
					for (const std::pair<const std::string, Symbol> &pair: vm.symbolTable)
						if (pair.second.location == address)
							*textbox += spaces + "\e[36m@\e[39;1;4m" + pair.first + "\e[22;24m";

				addLine(address);

				if (i == height) {
					textbox->suppressDraw = false;
					textbox->draw();
					textbox->suppressDraw = true;
				}
			}

			Word size = static_cast<Word>(vm.getMemorySize());
			for (Word address = size - 256 * 8; address < size; address += 8)
				addLine(address);
		});

		loops.join();
		textbox->suppressDraw = false;
		terminal.redraw();
	}

	std::string MemoryMode::stringify(Word address) const {
		std::stringstream ss;
		UWord word = vm.getWord(address, Endianness::Big);

		bool at_sp = vm.sp() - (vm.sp() % 8) == address;
		if (at_sp)
			ss << "\e[48;5;236m";

		if (vm.hasBreakpoint(address))
			ss << (vm.programCounter == address? "\e[48;5;22;31m" : "\e[38;5;202m");
		else if (vm.programCounter == address)
			ss << "\e[48;5;22;2m";
		else
			ss << "\e[2m";

		ss << "[" << std::setw(padding) << std::setfill(' ') << address << "]\e[22;90m  0x\e[39m";
		
		std::stringstream hex_ss;
		hex_ss << std::setw(16) << std::setfill('0') << std::hex << word;
		std::string hex = hex_ss.str();
		if (at_sp) {
			int offset = vm.sp() % 8;
			hex.replace(offset * 2, 2, "\e[48;5;240;1m" + hex.substr(offset * 2, 2) + "\e[48;5;236;22m");
		}

		ss << hex << "  ";

		if (address < 40) {
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

	void MemoryMode::updateLine(Word address, bool careless) {
		if (careless || 0 < lines.count(address)) {
			auto &simple = getLine(address);
			simple.text = stringify(address);
			textbox->redrawLine(simple);
		}
	}

	void MemoryMode::makeSymbolTableEdges() {
		symbolTableEdges.clear();
		symbolTableEdges.insert(vm.symbolsOffset);
		for (Word i = vm.symbolsOffset; i < vm.codeOffset && static_cast<size_t>(i + 4) < vm.getMemorySize();) {
			i += 16 + 8 * vm.getQuarterword(i + 6, Endianness::Big);
			if (i < vm.codeOffset)
				symbolTableEdges.insert(i);
		}
	}

	void MemoryMode::stop() {
		ClientMode::stop();
	}

	void MemoryMode::setFastForward(bool to) {
		terminal.suppressOutput = fastForward = to;
		if (!to)
			remakeList();
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

			Word end = start + count * 8;
			if (max < end)
				max = end;

			if (vm.getMemorySize() < static_cast<size_t>(end))
				vm.resize(end);

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
		} else if (verb == "MemoryWord") {
			Word address, value;
			if (size < 2 || !Util::parseLong(split[0], address) || !Util::parseLong(split[1], value)) {
				DBG("Bad message (" << verb << "): [" << rest << "]");
				return;
			}

			vm.setWord(address, value);
			try {
				updateLine(address);
			} catch (const std::out_of_range &) {}
		} else if (verb == "MemorySize") {
			Word resize_amount;
			if (size != 1 || !Util::parseLong(split[0], resize_amount)) {
				DBG("Bad message (" << verb << "): [" << rest << "]");
				return;
			}

			vm.resize(resize_amount);
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
		} else if (verb == "Done") {
			if (size == 1 && split[0] == "GetMain") {
				remakeList();
				jumpToPC();
			}
		} else if (verb == "AddBP") {
			Word breakpoint;
			if (size != 1 || !Util::parseLong(split[0], breakpoint)) {
				DBG("Invalid: AddBP[" << rest << "]");
				return;
			}

			addBreakpoint(breakpoint);
		} else if (verb == "RemoveBP") {
			Word breakpoint;
			if (size != 1 || !Util::parseLong(split[0], breakpoint)) {
				DBG("Invalid: RemoveBP[" << rest << "]");
				return;
			}

			removeBreakpoint(breakpoint);
		} else if (verb == "Register") {
			Word reg, value;
			if (size != 2 || !Util::parseLong(split[0], reg) || !Util::parseLong(split[1], value) || reg < 0
			    || 128 <= reg) {
				DBG("Invalid: Registers[" << rest << "]");
				return;
			}

			if (reg == Why::stackPointerOffset) {
				Word old_sp = vm.sp() - (vm.sp() % 8);
				vm.registers[reg] = value;
				updateLine(old_sp);
				updateLine(vm.sp() - (vm.sp() % 8));
			} else {
				vm.registers[reg] = value;
			}
		} else if (verb == "Unpaused") {
			vm.paused = false;
		} else if (verb == "Paused") {
			vm.paused = true;
			autotick = autotick < 0? autotick : -autotick;
		} else if (verb == "Offsets") {
			Word symbols, code, data, end;
			if (size != 4 || !Util::parseLong(split[0], symbols) || !Util::parseLong(split[1], code)
				|| !Util::parseLong(split[2], data) || !Util::parseLong(split[3], end)) {
				DBG("Invalid: Offsets[" << rest << "]");
				return;
			}

			vm.symbolsOffset = symbols;
			vm.codeOffset = code;
			vm.dataOffset = data;
			vm.endOffset = end;
		} else if (verb == "Quit") {
			stop();
			std::terminate();
		}
	}

	void MemoryMode::startAutotick() {
		send(":Unpause");
		autotick = autotick < 0? -autotick : autotick;
		std::unique_lock<std::mutex> lock(autotickMutex);
		autotickReady = true;
		autotickVariable.notify_one();
	}

	void MemoryMode::send(const std::string &to_send) {
		if (networkMutex.try_lock()) {
			*buffer << to_send << "\n";
			networkMutex.unlock();
		} else {
			DBG("Failed to send " << to_send);
		}
	}

	void MemoryMode::jumpToPC() {
		try {
			Haunted::UI::SimpleLine<Container> &simple = getLine(vm.programCounter);
			int rows = 0;
			for (auto &line: textbox->getLines()) {
				if (line.get() == &simple)
					break;
				rows += textbox->lineRows(*line);
			}

			textbox->vscroll(rows - textbox->getPosition().height / 2 - textbox->getVoffset());
		} catch (const std::out_of_range &) {
			return;
		}
	}

	Haunted::UI::SimpleLine<MemoryMode::Container> & MemoryMode::getLine(Word address) {
		if (lines.count(address) == 0)
			throw std::out_of_range("Line " + std::to_string(address) + " is missing");

		std::shared_ptr<Haunted::UI::TextLine<Container>> &line = lines.at(address);

		if (auto *simple = dynamic_cast<Haunted::UI::SimpleLine<Container> *>(line.get())) {
			return *simple;
		} else throw std::runtime_error("Unable to cast line to Haunted::UI::simpleline");
	}

	Haunted::UI::SimpleLine<MemoryMode::Container> & MemoryMode::addLine(Word address) {
		*textbox += stringify(address);
		auto ptr = textbox->getLines().back();
		lines.emplace(address, ptr);
		ptr->mouseFunction = [&, address](const Haunted::MouseReport &report) {
			if (report.action == Haunted::MouseAction::Up) {
				if (report.x <= 1 + padding) {
					// Clicked on [.....]
					if (report.button == Haunted::MouseButton::Left)
						send(":DebugData " + std::to_string(address));
					else
						send((vm.hasBreakpoint(address)? ":RemoveBP " : ":AddBP ") + std::to_string(address));
				} else if (padding + 4 <= report.x && report.x <= padding + 21) {
					// Clicked on 0x................
					if (report.button == Haunted::MouseButton::Left) {
						send(":SetPC " + std::to_string(address));
					} else {
						send(":Reg " + std::to_string(Why::stackPointerOffset) + " " + std::to_string(address));
					}
				} else if (padding + 24 <= report.x && report.x <= padding + 31) {
					send(":AskAbout " + std::to_string(address));
				}
			}
		};
		if (auto *simple = dynamic_cast<Haunted::UI::SimpleLine<Container> *>(ptr.get())) {
			return *simple;
		} else throw std::runtime_error("Couldn't cast new textline to simpleline");
	}

	void MemoryMode::addBreakpoint(Word breakpoint) {
		if (vm.hasBreakpoint(breakpoint))
			return;
		vm.addBreakpoint(breakpoint);
		updateLine(breakpoint);
	}

	void MemoryMode::removeBreakpoint(Word breakpoint) {
		if (!vm.hasBreakpoint(breakpoint))
			return;
		vm.removeBreakpoint(breakpoint);
		updateLine(breakpoint);
	}
}
