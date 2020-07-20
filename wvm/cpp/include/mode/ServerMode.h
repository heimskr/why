#ifndef WVM_MODE_SERVERMODE_H_
#define WVM_MODE_SERVERMODE_H_

#include <set>

#include "mode/Mode.h"
#include "net/Server.h"
#include "VM.h"

namespace WVM::Mode {
	class ServerMode: public Mode {
		private:
			Net::Server server;
			VM vm;
			std::set<int> memorySubscribers, registerSubscribers, pcSubscribers, outputSubscribers, ffSubscribers;
			std::set<int> bpSubscribers;
			std::set<Word> writtenAddresses;
			bool logMemoryWrites = false, logRegisters = false;

			void setFastForward(bool);
			void broadcast(const std::string &);
			void sendMemory(int);
			bool tick();

		public:
			static ServerMode *instance;
			ServerMode(int port): server(port, true), vm(1'000'000) {}

			void run(const std::string &path);
			void initVM();
			void cleanupClient(int);
			void stop();
			void handleMessage(int, const std::string &);
	};
}

#endif
