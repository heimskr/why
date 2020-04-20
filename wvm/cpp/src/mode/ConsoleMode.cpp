#include "lib/ansi.h"
#include "mode/ConsoleMode.h"
#include "Util.h"

namespace WVM::Mode {
	ConsoleMode::~ConsoleMode() {
		terminal.join();
	}

	void ConsoleMode::run(const std::string &hostname, int port) {
		networkThread = std::thread([&]() {
			ClientMode::run(hostname, port);
		});

		terminal.on_interrupt = [this]() {
			stop();
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
		if (verb == "Error") {
			textbox += errorPrefix + rest;
		} else if (verb == "Subscribed") {
			textbox += infoPrefix + std::string("Subscribed to ") + rest;
		} else {
			textbox += message;
		}
	}

	void ConsoleMode::handleInput(const std::string &text) {
		*socket << text << "\n";
	}
}
