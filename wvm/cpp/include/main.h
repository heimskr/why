#pragma once

#include "lib/formicine/ansi.h"
#include "haunted/core/terminal.h"
#include "haunted/ui/boxes/expandobox.h"

namespace WVM {
	class Main {
		private:
			haunted::terminal terminal;
			haunted::ui::boxes::expandobox *expando;
			haunted::ui::textbox *memory, *registers

			void initExpando();

		public:
			Main() = default;
			Main(Main &&) = delete;
			Main(const Main &) = delete;

			void start();
	};
}
