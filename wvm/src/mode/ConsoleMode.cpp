#include "Defs.h"
#include "lib/ansi.h"
#include "mode/ConsoleMode.h"
#include "Util.h"
#include "Why.h"

namespace WVM::Mode {
	void ConsoleMode::run(const std::string &hostname, int port) {
		ClientMode::run(hostname, port);

		networkThread = std::thread([&]() {
			loop();
		});

		terminal.onInterrupt = [this]() {
			stop();
			terminal.mouse(Haunted::MouseMode::None);
			return true;
		};

		input = new Haunted::UI::TextInput(&terminal);
		textbox = new Haunted::UI::VectorBox(&terminal);

		input->listen(Haunted::UI::TextInput::Event::Submit, [&](const Haunted::ustring &str, int) -> void {
			handleInput(str);
			input->clear();
		});

		expando = new Haunted::UI::Boxes::ExpandoBox(&terminal, Haunted::UI::Boxes::BoxOrientation::Vertical,
			std::initializer_list<Haunted::UI::Boxes::ExpandoBox::ChildPair> {{textbox, -1}, {input, 1}});

		terminal.startInput();
		terminal.setRoot(&*expando);
		input->focus();
		input->setPrefix("\e[2m>>\e[22m ");
		textbox->setBackground(ansi::color::verydark);
		textbox->setAutoscroll(true);
		terminal.mouse(Haunted::MouseMode::Motion);
		textbox->draw();
		terminal.watchSize();
		terminal.join();
		networkThread.join();
	}

	void ConsoleMode::stop() {
		ClientMode::stop();
	}

	void ConsoleMode::handleMessage(const std::string &message) {
		if (message.front() != ':') {
			DBG("Not sure how to handle [" << message << "]");
			return;
		}

		const size_t space = message.find(' ');
		const std::string verb = message.substr(1, space - 1);
		const std::string rest = space == std::string::npos? "" : message.substr(space + 1);
		const std::vector<std::string> split = Util::split(rest, " ", false);

		if (verb == "Error") {
			*textbox += errorPrefix + rest;
		} else if (verb == "Subscribed") {
			*textbox += std::string(infoPrefix) + "Subscribed to " + rest + ".";
		} else if (verb == "Register") {
			Word registerID, newValue;
			if (split.size() != 2 || !Util::parseLong(split[0], registerID) || !Util::parseLong(split[1], newValue)) {
				*textbox += std::string(errorPrefix) + "Invalid response from server.";
				DBG("Bad Register response [" << rest << "]");
			} else
				*textbox += infoPrefix + Why::coloredRegister(registerID) + " \e[2m<-\e[22m " +
					std::to_string(newValue);
		} else if (verb == "MemoryWord") {
			UWord pc, address;
			if (!Util::parseUL(split[2], pc) || !Util::parseUL(split[0], address)) {
				*textbox += std::string(errorPrefix) + "Invalid message.";
			} else if (filter.empty() || filter.count(address) != 0 || filter.count(pc) != 0) {
				std::string to_add = infoPrefix;
				if (split.size() == 3)
					to_add += "\e[2m(@ \e[1m" + split[2] + "\e[22;2m) ";
				to_add += "\e[2m[" + split[0] + "] = \e[22m" + split[1];
				*textbox += to_add;
			}
		} else if (verb == "Quit") {
			stop();
			std::terminate();
		} else if (verb == "Strict") {
			*textbox += "Strict mode \e[1m" + split[0] + "\e[22m.";
		} else if (verb == "Log") {
			*textbox += infoPrefix + rest;
		} else if (verb == "Paging") {
			*textbox += "Paging \e[1m" + split[0] + "\e[22m.";
		} else if (verb == "P0") {
			*textbox += "P0 set to \e[1m" + split[0] + "\e[22m.";
		} else if (verb == "PC") {
			*textbox += "Program counter: " + split[0];
		} else if (verb == "Debug") {
			const std::string joined = Util::join(split.begin() + 1, split.end());
			if (joined == "Not found")
				*textbox += "Address \e[1m" + split[0] + "\e[22m: " + joined;
			else
				*textbox += "Address \e[1m" + split[0] + "\e[22m: \e[4m" + joined + "\e[24m";
		} else if (verb == "SetReg") {
			Word reg = -1;
			if (!Util::parseLong(split[0], reg))
				*textbox += std::string(errorPrefix) + "Invalid register: \e[1m" + split[0] + "\e[22m\n";
			else
				*textbox += std::string(infoPrefix) + "Register " + Why::coloredRegister(reg) + " set to \e[1m"
					+ split[1] + "\e[22m.";
		} else if (verb == "AddedBP") {
			if (split.size() == 1)
				*textbox += std::string(infoPrefix) + "Added breakpoint at " + split[0] + ".";
		} else if (verb == "InvalidMessage") {
			*textbox += std::string(errorPrefix) + "Invalid message.";
		}
	}

	void ConsoleMode::handleInput(const std::string &text) {
		if (text.empty())
			return;

		const size_t space = text.find(' ');
		const std::string first = text.substr(0, space);
		const std::string rest = space == std::string::npos? "" : text.substr(space + 1);
		const std::vector<std::string> split = Util::split(rest, " ", false);
		const size_t size = split.size();

		if (first == "t" || first == "tick") {
			if (size == 0) {
				*socket << ":Tick\n";
			} else if (size == 1) {
				Word ticks;
				if (!Util::parseLong(split[0], ticks))
					badInput();
				else
					*socket << ":Tick " << ticks << "\n";
			} else badInput();
		} else if (first == "s" || first == "sub" || first == "subscribe") {
			if (size != 1) {
				badInput();
			} else if (split[0] == "r" || split[0] == "reg" || split[0] == "registers") {
				*socket << ":Subscribe registers\n";
			} else if (split[0] == "m" || split[0] == "mem" || split[0] == "memory") {
				*socket << ":Subscribe memory\n";
			} else if (split[0] == "p" || split[0] == "page" || split[0] == "paging") {
				*socket << ":Subscribe paging\n";
			} else if (split[0] == "p0") {
				*socket << ":Subscribe p0\n";
			} else badInput();
		} else if (first == "r" || first == "res" || first == "reset") {
			*socket << ":Reset\n";
		} else if (first == "b" || first == "ab" || first == "break" || first == "breakpoint" || first == "+breakpoint"
		           || first == "+b" || first == "+bp" || first == "+break") {
			if (size == 1) {
				*socket << ":AddBP " << split[0] << "\n";
			} else badInput();
		} else if (first == "rb" || first == "ub" || first == "unbreak" || first == "-breakpoint" || first == "-bp"
		           || first == "-b" || first == "-break") {
			if (size == 1) {
				Word address;
				if (!Util::parseLong(split[0], address)) {
					badInput();
				} else {
					*socket << ":RemoveBP " << address << "\n";
					*textbox += std::string(infoPrefix) + "Removed breakpoint at " + split[0] + ".";
				}
			} else badInput();
		} else if (first == "a" || first == "ask") {
			if (size == 1 || size == 2) {
				Word address;
				if (!Util::parseLong(split[0], address)) {
					badInput();
					return;
				}

				if (size == 2)
					*socket << ":AskAbout " << address << " " << split[1] << "\n";
				else
					*socket << ":AskAbout " << address << "\n";
			} else badInput();
		} else if (first == "str" || first == "string") {
			if (size != 0)
				*socket << ":GetString " << split[0] << "\n";
			else
				badInput();
		} else if (first == "u" || first == "un" || first == "undo" || first == "[") {
			*socket << ":Undo\n";
		} else if (first == "re" || first == "redo" || first == "]") {
			*socket << ":Redo\n";
		} else if (first == "st" || first == "strict") {
			if (size == 0)
				*socket << ":Strict\n";
			else
				*socket << ":Strict " << split[0] << "\n";
		} else if (first == "pc") {
			if (size == 0)
				*socket << ":GetPC\n";
			else
				*textbox += errorPrefix + std::string("Setting the program counter is currently unsupported.");
		} else if (first == "d" || first == "dbg" || first == "debug") {
			if (size == 0)
				*socket << ":DebugData\n";
			else
				*socket << ":DebugData " << split[0] << "\n";
		} else if (first == "set") {
			if (size == 2)
				*socket << ":SetReg " << split[0] << " " << split[1] << "\n";
			else
				badInput();
		} else if (first == "p" || first == "pl" || first == "play" || first == "+") {
			if (size == 0)
				*socket << ":Play\n";
			else if (size == 1)
				*socket << ":Play " << split[0] << "\n";
			else
				badInput();
		} else if (first == "pa" || first == "pause" || first == "-") {
			if (size == 0)
				*socket << ":Pause\n";
			else
				badInput();
		} else if (first == "h" || first == "hist" || first == "history") {
			if (1 < size || (size == 1 && split[0] != "on" && split[0] != "off"))
				badInput();
			else if (size == 0)
				*socket << ":History toggle\n";
			else
				*socket << ":History " << split[0] << "\n";
		} else if (first == "f" || first == "filt" || first == "filter") {
			if (size == 0) {
				std::string to_add = std::string(infoPrefix) + "Filtered addresses:";
				for (UWord address: filter)
					to_add += " " + std::to_string(address);
				*textbox += to_add;
			} else if (size != 2) {
				badInput();
			} else if (split[0] == "add" || split[0] == "remove" || split[0] == "+" || split[0] == "-") {
				UWord address;
				if (!Util::parseUL(split[1], address)) {
					badInput();
				} else {
					const bool present = filter.count(address) != 0;
					const std::string base = std::string(infoPrefix) + "Address \e[1m" + std::to_string(address) +
						"\e[22m ";
					if (split[0] == "add" || split[0] == "+") {
						if (present) {
							*textbox += base + "already in filter.";
						} else {
							filter.insert(address);
							*textbox += base + "added to filter.";
						}
					} else {
						if (present) {
							filter.erase(address);
							*textbox += base + "removed from filter.";
						} else
							*textbox += base + "not present in filter.";
					}
				}
			} else
				badInput();
		} else if (first == "dump") {
			if (size == 2)
				*socket << ":Dump " << split[0] << " " << split[1] << "\n";
			else if (size == 3)
				*socket << ":Dump " << split[0] << " " << split[1] << " " << split[2] << "\n";
			else
				badInput();
		} else if (text.front() == ':') {
			*socket << text << "\n";
		} else {
			badInput("Unrecognized");
		}
	}

	void ConsoleMode::badInput(const std::string &type) {
		*textbox += errorPrefix + type + " input.";
	}
}
