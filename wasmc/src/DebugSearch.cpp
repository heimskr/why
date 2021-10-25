#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <typeinfo>

#include "wasm/BinaryParser.h"
#include "util/Util.h"

std::string demangle(const char *name);


#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>

std::string demangle(const std::string &name) {
	int status = -1;
	std::unique_ptr<char, void(*)(void *)> res(abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status), std::free);
	return status == 0? res.get() : name;
}
#else
std::string demangle(const std::string &name) {
	return name;
}
#endif

int main(int argc, char **argv) {
	using namespace Wasmc;

	if (argc != 3) {
		std::cerr << "Usage: ds++ <filename> <function:line>\n";
		return 1;
	}

	std::unique_ptr<BinaryParser> parser;
	try {
		parser = std::make_unique<BinaryParser>(std::filesystem::path(argv[1]));
		parser->parse();
	} catch (const std::exception &err) {
		std::cerr << "Error parsing: " << err.what() << '\n';
		return 2;
	}

	const std::string combined = argv[2];

	try {
		const Long parsed = Util::parseUlong(combined);
		Long highest = 0;
		std::string highest_name;
		for (const auto &[name, symbol]: parser->symbols)
			if (symbol.address <= parsed && highest < symbol.address) {
				highest = symbol.address;
				highest_name = name;
			}
		if (highest == 0) {
			std::cout << "No symbol found for address " << parsed << ".\n";
		} else {
			if (highest_name.substr(0, 2) == "_Z") {
				highest_name = demangle(highest_name);
			} else {
				const std::regex mangled_label("^__(_Z.+)_label\\d+$");
				if (std::regex_match(highest_name, mangled_label)) {
					const std::string mangled = demangle(std::regex_replace(highest_name, mangled_label, "$1"));
					const std::string label(std::regex_replace(highest_name, std::regex("^___Z.+_label(\\d+)$"), "$1"));
					highest_name = mangled + " (label " + label + ")";
				}
			}
			std::cout << "Closest symbol to " << parsed << " is \e[1m"
			          << std::regex_replace(highest_name, std::regex("::__1"), "") << "\e[0m at " << highest << ".\n";
		}
	} catch (const std::invalid_argument &) {
		const size_t colon = combined.find(':');
		if (colon == std::string::npos) {
			std::cerr << "Couldn't parse function:line pair.\n";
			return 3;
		}

		const std::string filename = combined.substr(0, colon);
		size_t line_number;
		try {
			line_number = Util::parseUlong(combined.substr(colon + 1));
		} catch (const std::exception &err) {
			std::cerr << "Couldn't parse line number: " << err.what() << '\n';
			return 4;
		}

		const size_t debug_size = parser->debugData.size();
		for (const auto &debug: parser->debugData)
			if (debug->getType() == DebugEntry::Type::Location) {
				const DebugLocation *location = dynamic_cast<DebugLocation *>(debug.get());
				if (location->fileIndex < debug_size) {
					const DebugFilename *filename_entry =
						dynamic_cast<DebugFilename *>(parser->debugData.at(location->fileIndex).get());
					const std::string &fn = filename_entry->filename;
					bool is_match = fn == filename;
					if (!is_match) {
						const size_t last_slash = fn.find_last_of('/');
						if (last_slash != std::string::npos && fn.substr(last_slash + 1) == filename)
							is_match = true;

					}
					if (is_match && location->line == line_number)
						std::cout << location->address << " column " << location->column << '\n';
				}
			}
	}

	return 0;
}
