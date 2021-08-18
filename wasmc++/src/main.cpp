#include <fstream>
#include <iostream>

#include "parser/Parser.h"
#include "util/Util.h"
#include "wasm/Assembler.h"

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <input> <output>\n";
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
	file.close();

	auto &parser = Wasmc::wasmParser;
	parser.in(text);
	parser.debug(false, false);
	parser.parse();

	Wasmc::Assembler assembler(parser);

	std::ofstream outfile(argv[2]);
	if (!outfile.is_open())
		throw std::runtime_error("Couldn't open file for writing");
	try {
		outfile << assembler.assemble();
	} catch (const std::exception &err) {
		Wasmc::error() << "Compilation failed: " << err.what() << "\n";
		outfile.close();
		return 2;
	}

	outfile.close();
	Wasmc::success() << "Assembled \e[1m" << argv[1] << "\e[22m and saved the results to \e[1m" << argv[2]
		<< "\e[22m.\n";

	return 0;
}
