#ifndef WVM_MODE_MEMORYMODE_H_
#define WVM_MODE_MEMORYMODE_H_

#include <thread>

#include "haunted/core/terminal.h"
#include "haunted/ui/textbox.h"
#include "mode/ClientMode.h"

namespace WVM::Mode {
	class MemoryMode: public ClientMode {
		private:
			haunted::terminal terminal;
			std::thread networkThread;
			haunted::ui::textbox textbox;

		public:
			MemoryMode(): ClientMode() {}
			~MemoryMode();

			void run(const std::string &hostname, int port) override;
			void stop() override;
			void handleMessage(const std::string &) override;
	};
}

#endif
