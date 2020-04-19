#include <iostream>

#include "mode/ServerMode.h"

namespace WVM::Mode {
	void ServerMode::run() {
		server.messageHandler = [&](int client, const std::string &message) { handleMessage(client, message); };
		std::cerr << "ServerMode running on port " << server.getPort() << "\n";
		server.run();
	}

	void ServerMode::stop() {
		std::cerr << "Stopping.\n";
		server.stop();
	}

	void ServerMode::handleMessage(int client, const std::string &message) {
		const size_t space = message.find(' ');
		if (message.empty() || message.front() != ':') {
			server.send(client, ":Error Invalid message\n");
			return;
		}

		const std::string verb = message.substr(1, space - 1);
		if (verb == "Stop") {
			stop();
		} else {
			server.send(client, ":UnknownVerb " + verb + "\n");
		}
	}
}
