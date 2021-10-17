#ifndef WVM_VMERROR_H_
#define WVM_VMERROR_H_

#include <stdexcept>

namespace WVM {
	struct VMError: public std::runtime_error {
		using std::runtime_error::runtime_error;
	};
}

#endif
