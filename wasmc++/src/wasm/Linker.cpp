#include <fstream>

#include "util/Util.h"
#include "wasm/Linker.h"

namespace Wasmc {
	void Linker::addFile(const std::filesystem::path &path) {
		if (!std::filesystem::exists(path))
			throw std::runtime_error("Can't add file: path doesn't exist");

		std::ifstream stream(path);
		if (!stream.is_open())
			throw std::runtime_error("Couldn't open file for reading");
		
		std::vector<Long> longs;
		std::string line;
		while (std::getline(stream, line)) {
			std::cout << "[" << line << "]\n";
		}

		std::cout << "==================\n";
	}
}
