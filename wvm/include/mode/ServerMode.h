#pragma once

#include <mutex>
#include <set>

#include "mode/Mode.h"
#include "net/Server.h"
#include "VM.h"

namespace WVM::Mode {
	class ServerMode: public Mode {
		private:
			Net::Server server;
			VM vm;
			std::set<int> memorySubscribers, registerSubscribers, pcSubscribers, outputSubscribers, ffSubscribers,
			              bpSubscribers, pagingSubscribers, p0Subscribers;
			std::set<Word> writtenAddresses;
			bool logMemoryWrites = false, logRegisters = false;
			std::recursive_mutex subscriberMutex;

			void setFastForward(bool);
			void broadcast(const std::string &);
			void sendMemory(int);
			bool tick();

		public:
			static ServerMode *instance;

			ServerMode(int port): server(port, true), vm(134'217'728) {
				vm.logJumps = true;
			}

			void run(const std::string &path, const std::vector<std::string> &disks);
			void initVM();
			void cleanupClient(int);
			void stop();
			void handleMessage(int, const std::string &);

			std::unique_lock<std::recursive_mutex> lockSubscribers() { return std::unique_lock(subscriberMutex); }
	};
}
