#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "parser/Parser.h"
#include "util/Util.h"
#include "wasm/Assembler.h"
#include "wasm/Linker.h"

// #define CATCH_ASSEMBLE

int link(int argc, char **argv);
int test();
int assemble(int argc, char **argv);

int main(int argc, char **argv) {
	srand(time(nullptr));
	if (1 < argc) {
		if (strcmp(argv[1], "-l") == 0)
			return link(argc, argv);
		if (strcmp(argv[1], "--test") == 0)
			return test();
	}
	return assemble(argc, argv);
}

std::string stringify(const std::string &str) {
	return "\"" + str + "\"";
}

template <typename T>
std::string stringify(const std::vector<T> &bytes) {
	std::stringstream ss;
	bool first = true;
	ss << "{";
	for (T byte: bytes) {
		if (first)
			first = false;
		else
			ss << ", ";
		ss << Wasmc::Util::toHex(byte, 2);
	}
	ss << "}";
	return ss.str();
}

template <typename T>
void run_tests(const T &tests) {
	using namespace Wasmc;
		for (const auto &[input, expected]: tests) {
		std::cout << '\n' << stringify(input) << '\n';
		std::vector<Long> actual = Util::getLongs(input);
		if (actual.size() != expected.size()) {
			std::cout << "Size mismatch: expected " << expected.size() << ", got " << actual.size() << '\n';
		} else {
			for (size_t i = 0; i < actual.size(); ++i) {
				if (actual[i] == expected[i])
					std::cout << "Pass: " << Util::toHex(expected[i], 16) << '\n';
				else
					std::cout << "Fail: " << Util::toHex(actual[i], 16) << " != " << Util::toHex(expected[i], 16) << '\n';
			}
		}
	}
}

int test() {
	using namespace Wasmc;
	run_tests(std::vector<std::pair<std::string, std::vector<Long>>> {
		{"F", {0x4600000000000000}},
	});
	run_tests(std::vector<std::pair<std::vector<uint8_t>, std::vector<Long>>> {
		{{'F'}, {0x4600000000000000}},
		{{0x03, 0x0a, 0x00, 0x00, 0x98, 0x03, 0x00, 0x00, 0x01, 0x00, 0x00, 0x1b, 0x52, 0x00, 0x00, 0x00, 0xb0, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
			{0x030a000098030000, 0x0100001b52000000, 0xb079000000000000}},
	});
	return 0;
}

int link(int argc, char **argv) {
	if (argc < 4) {
		std::cerr << "Usage: " << argv[0] << " -l <output binary> <main input> [other input binaries...]\n";
		return 1;
	}

	Wasmc::Linker linker;

	for (int i = 3; i < argc; ++i)
		linker.addFile(argv[i]);

	std::ofstream outfile(argv[2]);
	if (!outfile.is_open())
		throw std::runtime_error("Couldn't open file for writing");

	try {
		outfile << linker.link();
	} catch (const std::exception &err) {
		Wasmc::error() << "Linking failed: " << err.what() << "\n";
		outfile.close();
		return 3;
	}

	outfile.close();
	Wasmc::success() << "Linked \e[1m" << argv[3] << "\e[22m and saved the results to \e[1m" << argv[2]
		<< "\e[22m.\n";

	return 0;
}

int assemble(int argc, char **argv) {
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

	if (Wasmc::wasmParser.errorCount != 0) {
		Wasmc::error() << "Failed to compile \e[1m" << argv[1] << "\e[22m.\n";
		return 2;
	}

	Wasmc::Assembler assembler(parser.root);
	parser.root = nullptr;
	parser.done();

	std::ofstream outfile(argv[2]);
	if (!outfile.is_open())
		throw std::runtime_error("Couldn't open file for writing");
#ifdef CATCH_ASSEMBLE
	try {
#endif
		outfile << assembler.assemble();
#ifdef CATCH_ASSEMBLE
	} catch (const std::exception &err) {
		Wasmc::error() << "Compilation failed: " << err.what() << "\n";
		outfile.close();
		return 3;
	}
#endif

	outfile.close();
	Wasmc::success() << "Assembled \e[1m" << argv[1] << "\e[22m and saved the results to \e[1m" << argv[2]
		<< "\e[22m.\n";

	return 0;
}
