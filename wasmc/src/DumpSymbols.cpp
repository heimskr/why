#include <iostream>
#include <map>
#include <memory>
#include <string.h>

#include "wasm/BinaryParser.h"
#include "util/Util.h"

int main(int argc, char **argv) {
	if (argc < 2) {
		std::cerr << "Usage: syms <filename>\n";
		return 1;
	}

	const bool any = argc == 2;

	using namespace Wasmc;
	std::unique_ptr<BinaryParser> parser;
	try {
		parser = std::make_unique<BinaryParser>(std::filesystem::path(argv[1]));
		parser->parse();
	} catch (const std::exception &err) {
		std::cerr << "Error parsing: " << err.what() << '\n';
		return 2;
	}

	std::map<std::string, SymbolTableEntry> ordered;
	
	for (auto &[name, index]: parser->symbolIndices)
		ordered.emplace(name, parser->symbols.at(index));

	for (const auto &[name, entry]: ordered) {
		if (!any) {
			bool found = false;
			for (int i = 2; i < argc; ++i)
				if (strcmp(name.c_str(), argv[i]) == 0) {
					found = true;
					break;
				}
			if (!found)
				continue;
		}

		std::cerr << "\e[1m";
		std::cout << name;
		std::cerr << "\e[22m";
		std::cout << "\n\t";
		std::cerr << "\e[2m";
		std::cout << "addr ";
		std::cerr << "\e[22m";
		std::cout << entry.address << "\n\t  ";
		std::cerr << "\e[2m";
		std::cout << "id ";
		std::cerr << "\e[22m";
		std::cout << entry.id << "\n\t";
		std::cerr << "\e[2m";
		std::cout << "type ";
		std::cerr << "\e[22m";
		switch (entry.type) {
			case SymbolEnum::Code: std::cout << "code"; break;
			case SymbolEnum::Data: std::cout << "data"; break;
			case SymbolEnum::UnknownCode: std::cout << "unknowncode break;
			case SymbolEnum::UnknownData: std::cout << "unknowndata"; break;
			case SymbolEnum::Unknown: std::cout << "unknown"; break;
			default: std::cout << static_cast<int>(entry.type); break;
		}
		std::cout << "\n\n";
	}
}
