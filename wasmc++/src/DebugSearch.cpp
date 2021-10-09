#include <iostream>
#include <memory>

#include "wasm/BinaryParser.h"
#include "util/Util.h"

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
					std::cout << location->address << '\n';
			}
		}

	return 0;
}
