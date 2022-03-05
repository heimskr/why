#include <optional>
#include <stdlib.h>
#include <signal.h>

#include "lib/ansi.h"
#include "mode/ConsoleMode.h"
#include "mode/MemoryMode.h"
#include "mode/OutputMode.h"
#include "mode/RegistersMode.h"
#include "mode/ServerMode.h"
#include "net/NetError.h"
#include "net/Server.h"
#include "Util.h"

std::optional<WVM::Mode::ServerMode> server;

void usage() {
	std::cerr << "Usage:\n"
	          << "- wvm server <executable> [files]...\n"
	          << "- wvm registers <hostname> <port>\n"
	          << "- wvm memory <hostname> <port>\n"
	          << "- wvm console <hostname> <port>\n";
}

int main(int argc, char **argv) {
	if (argc < 2) {
		usage();
		return 1;
	}

	std::string arg = argv[1];

	if (arg == "server") {
		if (argc < 3) {
			usage();
			return 1;
		}

		srand(time(NULL));
		server.emplace(rand() % 65536);
		signal(SIGINT, +[](int) { server->stop(); });
		const std::vector<std::string> files(argv + 3, argv + argc);

		try {
			server->run(argv[2], files);
		} catch (const WVM::Net::NetError &err) {
			if (err.statusCode != 4) // Interrupted system call
				std::cerr << err.what() << "\n";
		}

		return 0;
	}

	std::string hostname;
	WVM::UWord port;

	if (argc != 4) {
		usage();
		return 1;
	}

	hostname = argv[2];
	if (!WVM::Util::parseUL(argv[3], port)) {
		std::cerr << "Invalid port: " << argv[3] << "\n";
		return 1;
	}

	if (arg == "registers") {
		WVM::Mode::RegistersMode registers;
		registers.run(hostname, port);
	} else if (arg == "memory") {
		WVM::Mode::MemoryMode memory;
		memory.run(hostname, port);
	} else if (arg == "console") {
		WVM::Mode::ConsoleMode console;
		console.run(hostname, port);
	} else if (arg == "output") {
		WVM::Mode::OutputMode output;
		output.run(hostname, port);
	} else {
		usage();
		return 1;
	}
}
