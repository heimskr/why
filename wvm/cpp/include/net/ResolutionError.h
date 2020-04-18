#ifndef WVM_NET_RESOLUTIONERROR_H_
#define WVM_NET_RESOLUTIONERROR_H_

#include <stdexcept>

namespace WVM::Net {
	class ResolutionError: public std::runtime_error {
		public:
			int statusCode;

			ResolutionError(int status_code):
				std::runtime_error("Resolution failed"), statusCode(status_code) {}

			virtual const char * what() const throw() override {
				return gai_strerror(statusCode);
			}
	};
}

#endif
