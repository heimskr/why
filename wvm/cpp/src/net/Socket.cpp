#include <cstring>
#include <cerrno>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "net/Socket.h"
#include "net/NetError.h"
#include "net/ResolutionError.h"
#include "lib/ansi.h"

namespace WVM::Net {
	int Socket::socketCount = 0;

	Socket::Socket(const std::string &hostname_, int port_): hostname(hostname_), port(port_) {
		struct addrinfo hints = {};
		std::memset(&hints, 0, sizeof(hints));
		hints.ai_family   = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		int status = getaddrinfo(hostname.c_str(), std::to_string(port).c_str(), &hints, &info);
		if (status != 0)
			throw ResolutionError(errno);
	}
	
	Socket::~Socket() {
		freeaddrinfo(info);
	}

	void Socket::connect() {
		netFD = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
		int status = ::connect(netFD, info->ai_addr, info->ai_addrlen);
		if (status != 0) {
			DBG("connect(): " << strerror(errno));
			throw NetError(errno);
		}

		int control_pipe[2];
		status = pipe(control_pipe);
		if (status != 0) {
			DBG("pipe(): " << strerror(errno));
			throw NetError(errno);
		}

		controlRead = control_pipe[0];
		controlWrite = control_pipe[1];

		FD_ZERO(&fds);
		FD_SET(netFD, &fds);
		FD_SET(controlRead, &fds);

		connected = true;
	}

	void Socket::close() {
		if (connected) {
			ControlMessage message = ControlMessage::Close;
			::write(controlWrite, &message, 1);
			connected = false;
		}
	}

	ssize_t Socket::send(const void *data, size_t bytes) {
		if (!connected)
			throw std::invalid_argument("Socket not connected");
		return ::send(netFD, data, bytes, 0);
	}

	ssize_t Socket::recv(void *data, size_t bytes) {
		if (!connected)
			throw std::invalid_argument("Socket not connected");

		fd_set fds_copy = fds;
		int status = select(FD_SETSIZE, &fds_copy, NULL, NULL, NULL);
		if (status < 0) {
			DBG("select status: " << strerror(status));
			throw NetError(errno);
		}
			
		if (FD_ISSET(netFD, &fds_copy)) {
			ssize_t bytes_read = ::recv(netFD, data, bytes, 0);
			if (bytes_read == 0)
				close();

			return bytes_read;
		} else if (FD_ISSET(controlRead, &fds_copy)) {
			ControlMessage message;
			status = ::read(controlRead, &message, 1);
			if (status < 0) {
				DBG("control_fd status: " << strerror(status));
				throw NetError(errno);
			}

			if (message != ControlMessage::Close) {
				DBG("Unknown control message: '" << static_cast<char>(message) << "'");
			}

			return 0;
		} else {
			DBG("No file descriptor is ready.");
		}

		return -1;
	}

	int Socket::accept() {

	}
}
