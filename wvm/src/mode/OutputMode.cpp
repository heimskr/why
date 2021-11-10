#include "lib/ansi.h"
#include "mode/OutputMode.h"
#include "Defs.h"
#include "Util.h"
#include "Why.h"

namespace WVM::Mode {
	void OutputMode::run(const std::string &hostname, int port) {
		ClientMode::run(hostname, port);

		networkThread = std::thread([&]() {
			ClientMode::loop();
		});

		terminal.onInterrupt = [this]() {
			stop();
			return true;
		};

		*buffer << ":Subscribe output\n";
		terminal.keyPostlistener = [this](const Haunted::Key &key) {
			uint64_t encoded = uint64_t(key.type);
			if (key.hasShift())
				encoded |= 1ul << 32;
			if (key.hasAlt())
				encoded |= 1ul << 33;
			if (key.hasCtrl())
				encoded |= 1ul << 34;
			*buffer << ":Keybrd " << Util::toHex(encoded) << "\n";
		};
		std::cerr << "\e[2J\e[H";
		terminal.startInput();
		terminal.join();
		networkThread.join();
	}

	void OutputMode::stop() {
		ClientMode::stop();
	}

	void OutputMode::handleMessage(const std::string &message) {
		if (message.front() != ':') {
			if (message.size() != 2) {
				DBG("Not sure how to handle [" << message << "]");
				return;
			}

			Word to_print;
			if (!Util::parseLong(message, to_print, 16)) {
				DBG("Invalid raw message [" << message << "]");
				return;
			}

			std::cout << static_cast<char>(to_print);
			std::cout.flush();
			return;
		}

		const size_t space = message.find(' ');
		const std::string verb = message.substr(1, space - 1);

		if (verb == "Quit") {
			stop();
			std::exit(0);
		}
	}
}
