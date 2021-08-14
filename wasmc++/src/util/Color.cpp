#include "util/Color.h"

namespace Wasmc {
	std::string cyan(const std::string &interior) {
		return "\e[36m" + interior + "\e[39m";
	}

	std::string dim(const std::string &interior) {
		return "\e[2m" + interior + "\e[22m";
	}

	std::string red(const std::string &interior) {
		return "\e[31m" + interior + "\e[39m";
	}

	std::string green(const std::string &interior) {
		return "\e[32m" + interior + "\e[39m";
	}

	std::string yellow(const std::string &interior) {
		return "\e[33m" + interior + "\e[39m";
	}

	std::string blue(const std::string &interior) {
		return "\e[34m" + interior + "\e[39m";
	}

	std::string orange(const std::string &interior) {
		return "\e[38;5;202m" + interior + "\e[39m";
	}

	std::string bold(const std::string &interior) {
		return "\e[1m" + interior + "\e[22m";
	}
}