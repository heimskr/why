#ifndef WVM_MODE_CLIENTMODE_H_
#define WVM_MODE_CLIENTMODE_H_

#include <optional>

#include "mode/Mode.h"
#include "net/Socket.h"
#include "net/SocketBuffer.h"

namespace WVM::Mode {
	class ClientMode: public Mode {
		protected:
			std::optional<Net::Socket> socket;
			std::optional<Net::SocketBuffer> buffer;
			ClientMode() = default;

		public:
			virtual void run(const std::string &hostname, int port);
			virtual void loop();
			virtual void stop();
			virtual void handleMessage(const std::string &) = 0;
	};
}

#endif
