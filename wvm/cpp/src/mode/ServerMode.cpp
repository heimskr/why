#include <iostream>

#include "mode/ServerMode.h"
#include "Util.h"

namespace WVM::Mode {
	void ServerMode::run(const std::string &path) {
		server.messageHandler = [&](int client, const std::string &message) { handleMessage(client, message); };
		std::cerr << "ServerMode running on port " << server.getPort() << "\n";
		vm.load(path);
		server.run();
	}

	void ServerMode::stop() {
		std::cerr << "Stopping.\n";
		server.stop();
	}

	void ServerMode::handleMessage(int client, const std::string &message) {
		if (message.empty() || message.front() != ':') {
			server.send(client, ":Error Invalid message");
			return;
		}

		const std::vector<std::string> split = Util::split(message, " ");
		const size_t size = split.size();
		const std::string verb = split[0].substr(1);

		if (verb == "Stop") {
			stop();
		} else if (verb == "GetWord") {
			if (size != 2) {
				server.send(client, ":InvalidMessage " + message);
				return;
			}

			unsigned long int address;
			if (!Util::parseUL(split[1], address)) {
				server.send(client, ":InvalidMessage " + message);
				return;
			}

			server.send(client, ":MemoryWord " + std::to_string(address) + " " + std::to_string(vm.getWord(address, Endianness::Little)));
		} else {
			server.send(client, ":UnknownVerb " + verb);
		}
	}
}
