#ifndef WVM_MODE_SERVERMODE_H_
#define WVM_MODE_SERVERMODE_H_

#include "mode/Mode.h"
#include "net/Server.h"

namespace WVM::Mode {
	class ServerMode: Mode {
		private:
			Net::Server server;

		public:
			ServerMode(int port): server(port) {}

			void run();
			void stop();
			void handleMessage(int, const std::string &);
	};
}

#endif
