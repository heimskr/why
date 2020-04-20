#include <sstream>

#include "lib/ansi.h"
#include "mode/MemoryMode.h"
#include "Unparser.h"
#include "Util.h"

namespace WVM::Mode {
	MemoryMode::~MemoryMode() {
		terminal.join();
	}

	void MemoryMode::run(const std::string &hostname, int port) {
		networkThread = std::thread([&]() {
			ClientMode::run(hostname, port);
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
				} else if (key == 'm') {
					*buffer << ":Subscribe memory\n";
				} else if (key == 'r') {
					*buffer << ":Subscribe registers\n";
				} else if (key == 'g') {
					*buffer << ":GetMain \n";
				} else if (key == 'b') {
					remakeList();
				} else if (key == 'l') {
					terminal.redraw();
				} else return false;
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
					str.reserve(2 * (40 + padding));
					for (int i = 0; i < 40 + padding; ++i)
						str += "─";
					textbox += str + "\e[22m";
				}
				textbox += stringify(address);
				lines.emplace(i, textbox.get_lines().back());
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
		DBG(textbox.get_lines().size());
	}

	std::string MemoryMode::stringify(Word address) const {
		std::stringstream ss;
		UWord word = vm.getWord(address, Endianness::Big);
		ss << "\e[2m[" << std::setw(padding) << std::setfill(' ') << address << "]\e[22;90m  0x\e[39m";
		ss << std::setw(16) << std::setfill('0') << std::hex << word << "  " << std::dec;

		if (address < 32) {
			ss << "\e[38;5;26m" << word << "\e[39m";
		} else if (address < vm.symbolsOffset) {
			for (int i = 0; i < 8; ++i) {
				char ch = static_cast<char>(vm.getByte(address + i));
				if (ch < 32)
					ss << "\e[2m.\e[22m";
				else
					ss << ch;
			}
		} else if (vm.codeOffset <= address && address < vm.dataOffset) {
			ss << Unparser::stringify(word, &vm);
		}

		return ss.str();
	}

	void MemoryMode::updateLine(Word address) {
		if (lines.count(address) == 0)
			throw std::out_of_range("Line " + std::to_string(address) + " is missing");

		std::shared_ptr<haunted::ui::textline> &line = lines.at(address);

		if (haunted::ui::simpleline *simple = dynamic_cast<haunted::ui::simpleline *>(line.get())) {
			simple->text = stringify(address);
		} else throw std::runtime_error("Unable to cast line to haunted::ui::simpleline");
	}

	void MemoryMode::stop() {
		ClientMode::stop();
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
		}
	}
}
