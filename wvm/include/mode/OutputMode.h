#ifndef WVM_MODE_OUTPUTMODE_H_
#define WVM_MODE_OUTPUTMODE_H_

#include <thread>

#include "haunted/core/Terminal.h"
#include "mode/ClientMode.h"

namespace WVM::Mode {
	class OutputMode: public ClientMode {
		private:
			std::thread networkThread;
			Haunted::Terminal terminal;

		public:
			OutputMode(): ClientMode() {}

			void run(const std::string &hostname, int port) override;
			void stop() override;
			void handleMessage(const std::string &) override;
	};
}

#endif
