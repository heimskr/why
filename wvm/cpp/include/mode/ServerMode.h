#ifndef WVM_MODE_SERVERMODE_H_
#define WVM_MODE_SERVERMODE_H_

#include "mode/Mode.h"
#include "net/Server.h"
#include "VM.h"

namespace WVM::Mode {
	class ServerMode: Mode {
		private:
			Net::Server server;
			VM vm;

		public:
			ServerMode(int port): server(port), vm(1'000'000) {}

			void run(const std::string &path);
			void stop();
			void handleMessage(int, const std::string &);
	};
}

#endif
