#include <fstream>

#include "parser/Lexer.h"
#include "parser/Parser.h"
#include "util/Util.h"
#include "wasm/Assembler.h"
#include "wasm/Linker.h"
#include "wasm/BinaryParser.h"

namespace Wasmc {
	void Linker::addFile(const std::filesystem::path &path) {
		if (paths.count(path) != 0) {
			warn() << "Skipping file \e[1m" << path << "\e[22m: already included\n";
			return;
		}

		if (!std::filesystem::exists(path))
			throw std::runtime_error("Can't add file: path doesn't exist");

		std::ifstream stream(path);
		if (!stream.is_open())
			throw std::runtime_error("Couldn't open file for reading");

		paths.insert(path);

		char first17[17];
		stream.read(first17, 17);
		bool is_long = true;
		for (int i = 0; i < 16; ++i) {
			const char ch = first17[i];
			if (!(('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'f'))) {
				is_long = false;
				break;
			}
		}

		if (is_long)
			is_long = first17[16] == '\n' || first17[16] == '\r';

		stream.seekg(0);

		if (is_long) {
			firstDone = true;
			std::vector<Long> longs;
			std::string line;
			while (std::getline(stream, line))
				longs.push_back(Util::parseUlong(line, 16));
			units.push_back(longs);
		} else if (firstDone) {
			throw std::runtime_error("Only the first input file can be non-compiled assembly");
		} else {
			firstDone = true;
			std::string text;
			stream.seekg(0, std::ios::end);
			text.reserve(stream.tellg());
			stream.seekg(0, std::ios::beg);
			text.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
			stream.close();
			wasmParser.in(text);
			wasmParser.debug(false, false);
			wasmParser.parse();
			root = wasmParser.root;
			wasmParser.root = nullptr;
			wasmParser.done();
			Assembler assembler(root);
			assembler.assemble();
			units.push_back(assembler.getAssembled());
			for (const ASTNode *node: *root)
				if (node->symbol == WASMTOK_INCLUDE_HEADER)
					for (const ASTNode *sub: *node->front())
						addFile(std::filesystem::canonical(path.parent_path() / sub->unquote()));
		}
	}

	std::string Linker::link() {
		if (units.empty())
			throw std::runtime_error("Can't link before any files are added");

		for (const std::vector<Long> &unit: units) {
			BinaryParser binary_parser(unit);
			binary_parser.parse();
		}

		return Assembler::stringify(linked);
	}
}
