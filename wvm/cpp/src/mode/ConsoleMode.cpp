#include "Defs.h"
#include "lib/ansi.h"
#include "mode/ConsoleMode.h"
#include "Util.h"
#include "Why.h"

namespace WVM::Mode {
	void ConsoleMode::run(const std::string &hostname, int port) {
		networkThread = std::thread([&]() {
			ClientMode::run(hostname, port);
		});

		terminal.on_interrupt = [this]() {
			stop();
			terminal.mouse(haunted::mouse_mode::none);
			return true;
		};

		input.listen(haunted::ui::textinput::event::submit, [&](const haunted::ustring &str, int) -> void {
			handleInput(str);
			input.clear();
		});

		expando.emplace(&terminal, haunted::ui::boxes::box_orientation::vertical,
			std::initializer_list<haunted::ui::boxes::expandobox::child_pair> {{&textbox, -1}, {&input, 1}});

		terminal.start_input();
		terminal.set_root(&*expando);
		input.focus();
		input.set_prefix("\e[2m>>\e[22m ");
		textbox.set_background(ansi::color::verydark);
		textbox.set_autoscroll(true);
		terminal.mouse(haunted::mouse_mode::motion);
		textbox.draw();
		terminal.watch_size();
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
			textbox += errorPrefix + rest;
		} else if (verb == "Subscribed") {
			textbox += std::string(infoPrefix) + "Subscribed to " + rest;
		} else if (verb == "Register") {
			Word registerID, newValue;
			if (split.size() != 2 || !Util::parseLong(split[0], registerID) || !Util::parseLong(split[1], newValue)) {
				textbox += std::string(errorPrefix) + "Invalid response from server.";
				DBG("Bad Register response [" << rest << "]");
			} else {
				textbox += infoPrefix + Why::coloredRegister(registerID) + " \e[2m<-\e[22m " + std::to_string(newValue);
			}
		} else if (verb == "MemoryWord") {
			textbox += std::string(infoPrefix) + "\e[2m[" + split[0] + "] = \e[22m" + split[1];
		} else if (verb == "Quit") {
			stop();
			std::terminate();
		} else {
			textbox += message;
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
			} else badInput();
		} else if (first == "r" || first == "res" || first == "reset") {
			*socket << ":Reset\n";
		} else if (first == "b" || first == "ab" || first == "break" || first == "breakpoint" || first == "+breakpoint"
		           || first == "+b" || first == "+bp" || first == "+break") {
			if (size == 1) {
				Word address;
				if (!Util::parseLong(split[0], address)) {
					badInput();
				} else {
					*socket << ":AddBP " << address << "\n";
					textbox += std::string(infoPrefix) + "Added breakpoint at " + split[0] + ".";
				}
			} else badInput();
		} else if (first == "rb" || first == "ub" || first == "unbreak" || first == "-breakpoint" || first == "-bp"
		           || first == "-b" || first == "-break") {
			if (size == 1) {
				Word address;
				if (!Util::parseLong(split[0], address)) {
					badInput();
				} else {
					*socket << ":RemoveBP " << address << "\n";
					textbox += std::string(infoPrefix) + "Removed breakpoint at " + split[0] + ".";
				}
			} else badInput();
		} else if (text.front() == ':') {
			*socket << text << "\n";
		} else {
			badInput("Unrecognized");
		}
	}

	void ConsoleMode::badInput(const std::string &type) {
		textbox += errorPrefix + type + " input.";
	}
}
