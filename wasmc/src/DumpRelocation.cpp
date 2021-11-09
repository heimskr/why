#include <iostream>
#include <memory>

#include "wasm/BinaryParser.h"
#include "util/Util.h"

int main(int argc, char **argv) {
	if (argc < 2) {
		std::cerr << "Usage: reloc <filename>\n";
		return 1;
	}

	using namespace Wasmc;
	std::unique_ptr<BinaryParser> parser;
	try {
		parser = std::make_unique<BinaryParser>(std::filesystem::path(argv[1]));
		parser->parse();
	} catch (const std::exception &err) {
		std::cerr << "Error parsing: " << err.what() << '\n';
		return 2;
	}

	size_t i = 0;
	std::cerr << "Relocation entries: " << parser->relocationData.size() << "\n";
	for (const auto &entry: parser->relocationData) {
		std::cerr << i++ << ": ";
		switch (entry.type) {
			case RelocationType::Full:   std::cerr << "full";   break;
			case RelocationType::Lower4: std::cerr << "lower4"; break;
			case RelocationType::Upper4: std::cerr << "upper4"; break;
			default: std::cerr << "(" << int(entry.type) << "?)"; break;
		}
		std::cerr << " " << (entry.isData? "data" : "code") << " offset(\e[1m" << entry.offset << "\e[22m) ";
		std::cerr << "sectionOffset(\e[1m" << entry.sectionOffset << "\e[22m) " << entry.symbolIndex;
		if (size_t(entry.symbolIndex) < parser->symbols.size())
			std::cerr << "(\e[32m" << parser->symbols.at(entry.symbolIndex).label << "\e[39m)";
		std::cerr << "\n";
	}
}
