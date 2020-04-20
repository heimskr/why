#ifndef WVM_NET_SOCKET_H_
#define WVM_NET_SOCKET_H_

#include <string>
#include <netdb.h>

namespace WVM::Net {
	class Socket {
		friend class SocketBuffer;

		private:
			static int socketCount;
			struct addrinfo *info;
			int netFD = -1, controlRead = -1, controlWrite = -1;
			bool connected = false;
			fd_set fds = {0};

			enum class ControlMessage: char {Close='C'};

		public:
			std::string hostname;
			int port;

			Socket(const std::string &hostname_, int port_);

			Socket() = delete;
			Socket(const Socket &) = delete;
			Socket & operator=(const Socket &) = delete;
			Socket & operator=(Socket &&);

			~Socket();

			/** Connects to the socket. */
			void connect();

			/** Closes the socket. */
			void close();

			/** Sends a given number of bytes from a buffer through the socket and returns the number of bytes sent. */
			ssize_t send(const void *, size_t);

			/** Reads a given number of bytes into a buffer from the socket and returns the number of bytes read. */
			ssize_t recv(void *, size_t);

			int accept();
	};
}

#endif
