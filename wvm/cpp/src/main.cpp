#include "main.h"
#include "ansi.h"

int main() {
	WVM::Main main;
	main.start();
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
