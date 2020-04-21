#ifndef WVM_MODE_OUTPUTMODE_H_
#define WVM_MODE_OUTPUTMODE_H_

#include <thread>

#include "haunted/core/terminal.h"
#include "haunted/ui/boxes/expandobox.h"
#include "haunted/ui/textbox.h"
#include "haunted/ui/textinput.h"
#include "mode/ClientMode.h"

namespace WVM::Mode {
	class OutputMode: public ClientMode {
		private:
			std::thread networkThread;

		public:
			OutputMode(): ClientMode() {}

			void run(const std::string &hostname, int port) override;
			void stop() override;
			void handleMessage(const std::string &) override;
	};
}

#endif
