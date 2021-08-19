#include <fstream>
#include <optional>

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
			std::vector<Long> longs;
			std::string line;
			while (std::getline(stream, line))
				longs.push_back(Util::parseUlong(line, 16));
			if (firstDone)
				subunits.push_back(longs);
			else
				mainUnit = std::move(longs);
			firstDone = true;
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
			mainUnit = std::move(assembler.getAssembled());
			for (const ASTNode *node: *root)
				if (node->symbol == WASMTOK_INCLUDE_HEADER)
					for (const ASTNode *sub: *node->front())
						addFile(std::filesystem::canonical(path.parent_path() / sub->unquote()));
		}
	}

	std::string Linker::link() {
		if (mainUnit.empty())
			throw std::runtime_error("Can't link before any files are added");

		BinaryParser main_parser(mainUnit);

		auto combined_symbols = main_parser.symbols;
		auto combined_code = main_parser.rawCode;
		auto combined_data = main_parser.rawData;
		auto combined_debug = main_parser.copyDebugData();

		for (const std::vector<Long> &unit: subunits) {
			BinaryParser binary_parser(unit);
			binary_parser.parse();
		}

		return Assembler::stringify(linked);
	}

	void Linker::depointerize(const SymbolTable &table, std::vector<Long> &data, Long data_offset) {
		for (const auto &[key, value]: table) {
			const auto address = value.address;
			const auto type = value.type;
			const auto index = (address - data_offset) / 8;
			if (type == SymbolEnum::KnownPointer) {
				const Long current_value = Util::swapEndian(data.at(index));
				std::optional<SymbolTableEntry> match;
				for (const auto &pair: table)
					if (pair.second.address == current_value) {
						match = pair.second;
						break;
					}
				if (!match.has_value())
					throw std::runtime_error("Found no matches for " + Util::toHex(current_value) + "  from key \""
						+ key + "\"");
				data[index] = match->id;
			}
		}
	}

	std::unordered_map<std::string, SymbolType>
	Linker::collectSymbolTypes(const Offsets &offsets, const SymbolTable &table) {
		std::unordered_map<std::string, SymbolType> out;
		for (const auto &[key, value]: table)
			out[key] = getSymbolType(offsets, table, key);
		return out;
	}

	SymbolType Linker::getSymbolType(const Offsets &offsets, const SymbolTable &table, const std::string &symbol) {
		const auto address = table.at(symbol).address;
		if (offsets.code <= address && address < offsets.data)
			return SymbolType::Code;
		if (offsets.data <= address && address < offsets.debug)
			return SymbolType::Data;
		return SymbolType::Other;
	}
}
