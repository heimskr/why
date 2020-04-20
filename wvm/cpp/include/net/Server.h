#ifndef WVM_NET_SERVER_H_
#define WVM_NET_SERVER_H_

#include <functional>
#include <map>

#include <sys/types.h>

namespace WVM::Net {
	class Server {
		private:
			int makeSocket();
			int port;
			size_t chunkSize;
			char *buffer;
			int lastClient = -1;
			int sock = -1;
			fd_set activeSet;
			int controlRead = -1, controlWrite = -1;
			bool connected = false;

			enum class ControlMessage: char {Close='C'};

			/** Maps client IDs to descriptors. */
			std::map<int, int> descriptors;

			/** Maps descriptors to client IDs. */
			std::map<int, int> clients;

			/** Maps descriptors to buffers. */
			std::map<int, std::string> buffers;

		public:
			std::function<void(int, const std::string &)> messageHandler; // (int client, const std::string &message)
			std::function<void(int, int)> onEnd; // (int client, int descriptor)

			Server(uint16_t port_, size_t chunk_size = 512);
			~Server();

			int getPort() const { return port; }
			void readFromClient(int descriptor);
			virtual void handleMessage(int client, const std::string &message);
			virtual void end(int descriptor);
			void send(int client, const std::string &message, bool suppress_newline = false);
			void removeClient(int);
			void run();
			void stop();

			/** Given a buffer and the old size of the buffer before the last insertion, this function returns {-1, *}
			 *  if the message is still incomplete or the {i, l} if the buffer contains a complete message, where i is
			 *  the index at which the message ends and l is the size of the delimiter that ended the message.
			 *  By default, a message is considered complete after the first newline. */
			virtual std::pair<ssize_t, size_t> isMessageComplete(const std::string &, size_t);
	};
}

#endif