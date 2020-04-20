
#include "lib/ansi.h"
#include "mode/MemoryMode.h"

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
			return true;
		};

		textbox.key_fn = [&](const haunted::key &key) {
			if (key == haunted::kmod::ctrl) {
				if (key == 't') {
					*buffer << ":Tick\n";
				} else if (key == 'm') {
					*buffer << ":Subscribe memory\n";
				} else if (key == 'r') {
					*buffer << ":Subscribe registers\n";
				} else if (key == 'l') {
					terminal.redraw();
				} else return false;
			} else return false;
			return true;
		};

		terminal.start_input();
		terminal.set_root(&textbox);
		terminal.watch_size();
		terminal.join();
		networkThread.join();
	}

	void MemoryMode::stop() {
		ClientMode::stop();
	}

	void MemoryMode::handleMessage(const std::string &message) {
		ansi::out << "[" << message << "]\n";
	}
}
