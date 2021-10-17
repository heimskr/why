#ifndef WVM_MODE_CONSOLEMODE_H_
#define WVM_MODE_CONSOLEMODE_H_

#include <thread>

#include "haunted/core/Terminal.h"
#include "haunted/ui/boxes/ExpandoBox.h"
#include "haunted/ui/Textbox.h"
#include "haunted/ui/TextInput.h"
#include "mode/ClientMode.h"

namespace WVM::Mode {
	class ConsoleMode: public ClientMode {
		private:
			Haunted::Terminal terminal;
			std::thread networkThread;
			Haunted::UI::Boxes::ExpandoBox *expando;
			Haunted::UI::VectorBox *textbox;
			Haunted::UI::TextInput *input;

			void badInput(const std::string & = "Invalid");

		public:
			ConsoleMode(): ClientMode() {}

			void run(const std::string &hostname, int port) override;
			void stop() override;
			void handleMessage(const std::string &) override;
			void handleInput(const std::string &);
	};
}

#endif
