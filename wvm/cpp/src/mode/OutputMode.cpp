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

		*buffer << ":Subscribe output\n";
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
			return;
		}

		const size_t space = message.find(' ');
		const std::string verb = message.substr(1, space - 1);
		const std::string rest = space == std::string::npos? "" : message.substr(space + 1);
		const std::vector<std::string> split = Util::split(rest, " ", false);
	}
}
