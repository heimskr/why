#include <stdlib.h>
#include <signal.h>

#include "main.h"
#include "lib/ansi.h"
#include "net/Server.h"

WVM::Net::Server serv(44902);

void cleanup() {
	serv.stop();
}

void sigint_handler(int) {
	cleanup();
}

int main() {
	// WVM::Main main;
	// main.start();
	serv.run();
	atexit(cleanup);
	signal(SIGINT, sigint_handler);
}

namespace WVM {
	void Main::start() {
		DBG("Hello, World!");
		terminal.watch_size();
		terminal.start_input();
	}

	void Main::initExpando() {
		expando = new haunted::ui::boxes::expandobox(&terminal, terminal.get_position(),
			haunted::ui::boxes::box_orientation::horizontal);
		expando->set_name("expando");
		terminal.set_root(expando);
	}
}
