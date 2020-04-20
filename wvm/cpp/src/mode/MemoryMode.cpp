
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

		terminal.watch_size();
		terminal.set_root(&textbox);
		terminal.start_input();
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
