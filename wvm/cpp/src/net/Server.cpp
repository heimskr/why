// Based in large part on example code from the GNU libc documentation.

#include <iostream>

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "net/NetError.h"
#include "net/Server.h"

namespace WVM::Net {
	Server::Server(uint16_t port_, bool line_mode, size_t chunk_size):
		port(port_), chunkSize(chunk_size), buffer(new char[chunk_size]), lineMode(line_mode) {}

	Server::~Server() {
		delete[] buffer;
	}

	int Server::makeSocket() {
		int sock;

		sock = ::socket(PF_INET, SOCK_STREAM, 0);
		if (sock < 0)
			throw NetError(errno);

		sockaddr_in name = {.sin_family = AF_INET, .sin_port = htons(port), .sin_addr = {.s_addr = htonl(INADDR_ANY)}};
		if (::bind(sock, (sockaddr *) &name, sizeof(name)) < 0)
			throw NetError(errno);

		return sock;
	}

	void Server::readFromClient(int descriptor) {
		std::string &str = buffers[descriptor];
		str.reserve(chunkSize);

		int byte_count = ::read(descriptor, buffer, chunkSize);
		if (byte_count < 0) {
			throw NetError("Reading", errno);
		} else if (byte_count == 0) {
			end(descriptor);
		} else if (!lineMode) {
			handleMessage(clients.at(descriptor), str);
			buffers[descriptor].clear();
		} else {
			str.insert(str.size(), buffer, byte_count);
			ssize_t index;
			size_t delimiter_size;
			bool done = false;
			std::tie(index, delimiter_size) = isMessageComplete(str);
			do {
				if (index != -1) {
					handleMessage(clients.at(descriptor), str.substr(0, index));
					if (clients.count(descriptor) == 1) {
						str.erase(0, index + delimiter_size);
						std::tie(index, delimiter_size) = isMessageComplete(str);
					} else done = true;
				} else done = true;
			} while (!done);
		}
	}

	void Server::handleMessage(int client, const std::string &message) {
		if (messageHandler) {
			messageHandler(client, message);
		} else {
			std::cerr << "Server: got message from client " << client << ": \"" << message << "\"\n";
		}
	}

	void Server::end(int descriptor) {
		::close(descriptor);
		const int client = clients.at(descriptor);
		descriptors.erase(client);
		buffers.erase(descriptor);
		clients.erase(descriptor);
		allClients.erase(client);
		FD_CLR(descriptor, &activeSet);
		if (onEnd)
			onEnd(client, descriptor);
	}

	void Server::send(int client, const std::string &message, bool suppress_newline) {
		::write(descriptors.at(client), message.c_str(), message.size());
		if (!suppress_newline)
			::write(descriptors.at(client), "\n", 1);
	}

	void Server::removeClient(int client) {
		end(descriptors.at(client));
	}

	void Server::run() {
		struct sockaddr_in clientname;
		size_t size;

		int control_pipe[2];
		if (pipe(control_pipe) < 0) {
			throw NetError("pipe()", errno);
		}

		controlRead  = control_pipe[0];
		controlWrite = control_pipe[1];

		sock = makeSocket();
		if (::listen(sock, 1) < 0)
			throw NetError("Listening", errno);

		FD_ZERO(&activeSet);
		FD_SET(sock, &activeSet);
		FD_SET(controlRead, &activeSet);
		connected = true;

		fd_set read_set;
		for (;;) {
			// Block until input arrives on one or more active sockets.
			read_set = activeSet;
			if (::select(FD_SETSIZE, &read_set, NULL, NULL, NULL) < 0)
				throw NetError("select()", errno);

			if (FD_ISSET(controlRead, &read_set)) {
				std::cerr << "Closed server socket.\n";
				::close(sock);
				break;
			}

			// Service all the sockets with input pending.
			for (int i = 0; i < FD_SETSIZE; ++i) {
				if (FD_ISSET(i, &read_set)) {
					if (i == sock) {
						// Connection request on original socket.
						int new_fd;
						size = sizeof(clientname);
						new_fd = ::accept(sock, (sockaddr *) &clientname, (socklen_t *) &size);
						if (new_fd < 0)
							throw NetError("accept()", errno);
						std::cerr << "Server: connect from host " << inet_ntoa(clientname.sin_addr) << ", port "
						          << ntohs(clientname.sin_port) << "\n";
						FD_SET(new_fd, &activeSet);
						int new_client = ++lastClient;
						descriptors.emplace(new_client, new_fd);
						clients.erase(new_fd);
						clients.emplace(new_fd, new_client);
						allClients.insert(new_client);
					} else if (i != controlRead) {
						// Data arriving on an already-connected socket.
						try {
							readFromClient(i);
						} catch (const NetError &err) {
							std::cerr << err.what() << "\n";
							removeClient(clients.at(i));
						}
					}
				}
			}
		}
	}

	void Server::stop() {
		if (connected) {
			ControlMessage message = ControlMessage::Close;
			::write(controlWrite, &message, 1);
			connected = false;
		}
	}

	std::pair<ssize_t, size_t> Server::isMessageComplete(const std::string &buf) {
		const size_t found = buf.find("\n");
		return found == std::string::npos? std::pair<ssize_t, size_t>(-1, 0) : std::pair<ssize_t, size_t>(found, 1);
	}
}
