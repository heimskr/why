
#include "lib/ansi.h"
#include "mode/ConsoleMode.h"

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
		textbox.set_background(ansi::color::gray);
		textbox.draw();
		terminal.watch_size();
		terminal.join();
		networkThread.join();
	}

	void ConsoleMode::stop() {
		ClientMode::stop();
	}

	void ConsoleMode::handleMessage(const std::string &message) {
		textbox += message;
	}

	void ConsoleMode::handleInput(const std::string &text) {
		*socket << text << "\n";
	}
}
