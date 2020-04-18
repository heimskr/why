#ifndef WVM_NET_SOCKETBUFFER_H_
#define WVM_NET_SOCKETBUFFER_H_

#include <iostream>

namespace WVM::Net {
	class Socket;

	class SocketBuffer: public std::streambuf {
		private:
			Socket *source;
			char *buffer;
			size_t bufferSize;
			size_t putbackSize;
		
		protected:
			virtual std::streambuf::int_type overflow(std::streambuf::int_type) override;
			virtual std::streamsize xsputn(const char *, std::streamsize) override;
			virtual std::streambuf::int_type underflow() override;

		public:
			SocketBuffer(Socket *source_, size_t buffer_size = 64, size_t putback_size = 4);
			~SocketBuffer();

			/** Closes the underlying socket connection. */
			void close();
	};
}

#endif
