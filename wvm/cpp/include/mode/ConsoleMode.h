#ifndef WVM_MODE_CONSOLEMODE_H_
#define WVM_MODE_CONSOLEMODE_H_

#include <thread>

#include "haunted/core/terminal.h"
#include "haunted/ui/boxes/expandobox.h"
#include "haunted/ui/textbox.h"
#include "haunted/ui/textinput.h"
#include "mode/ClientMode.h"

namespace WVM::Mode {
	class ConsoleMode: public ClientMode {
		private:
			haunted::terminal terminal;
			std::thread networkThread;
			std::optional<haunted::ui::boxes::expandobox> expando;
			haunted::ui::textbox textbox;
			haunted::ui::textinput input;

		public:
			ConsoleMode(): ClientMode() {}
			~ConsoleMode();

			void run(const std::string &hostname, int port) override;
			void stop() override;
			void handleMessage(const std::string &) override;
			void handleInput(const std::string &);
	};
}

#endif
