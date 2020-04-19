#include <iostream>

#include "mode/ServerMode.h"

namespace WVM::Mode {
	void ServerMode::run() {
		server.messageHandler = [&](int client, const std::string &message) { handleMessage(client, message); };
		std::cerr << "ServerMode running on port " << server.getPort() << "\n";
		server.run();
	}

	void ServerMode::stop() {
		server.stop();
	}

	void ServerMode::handleMessage(int client, const std::string &message) {
		const size_t space = message.find(' ');
		if (message.empty() || message.front() != ':' || space == std::string::npos) {
			server.send(client, ":error Invalid message\n");
			return;
		}
	}
}
