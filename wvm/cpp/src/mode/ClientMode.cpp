#include "mode/ClientMode.h"

namespace WVM::Mode {
	void ClientMode::run(const std::string &hostname, int port) {
		socket.emplace(hostname, port);
		socket->connect();
		buffer.emplace(&*socket);
		std::iostream stream(&*buffer);
		std::string line;
		while (std::getline(stream, line)) {
			if (line.back() == '\n')
				line.pop_back();
			handleMessage(line);
		}
	}

	void ClientMode::stop() {
		if (buffer)
			buffer->close();
		if (socket)
			socket->close();
	}
}