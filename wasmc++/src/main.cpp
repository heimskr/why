#include <fstream>
#include <iostream>

#include "parser/Parser.h"
#include "wasm/Assembler.h"

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <filename>\n";
		return 1;
	}

	std::ifstream file(argv[1]);
	if (!file.is_open())
		throw std::runtime_error("Couldn't open file for reading");
	std::string text;
	file.seekg(0, std::ios::end);
	text.reserve(file.tellg());
	file.seekg(0, std::ios::beg);
	text.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	auto &parser = Wasmc::wasmParser;
	parser.in(text);
	parser.debug(false, false);
	parser.parse();

	Wasmc::Assembler assembler(parser);
	std::cout << assembler.assemble() << "\n";

	return 0;
}
