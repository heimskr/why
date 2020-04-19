#ifndef WVM_NET_NETERROR_H_
#define WVM_NET_NETERROR_H_

#include <stdexcept>
#include <string>

#include <string.h>

namespace WVM::Net {
	class NetError: public std::runtime_error {
		private:
			std::string message;

		public:
			int statusCode;

			NetError(int status_code):
				NetError("Network operation", status_code) {}

			NetError(const std::string &type, int status_code):
				std::runtime_error(type + " failed: " + strerror(status_code)),
				message(type + " failed: " + strerror(status_code)),
				statusCode(status_code) {}

			const char * what() const throw() override {
				return message.c_str();
			}
	};
}

#endif
