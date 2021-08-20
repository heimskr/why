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
		main_parser.parse();

		const size_t meta_length = main_parser.getMetaLength();

		const auto &main_symbols = main_parser.symbols;
		auto combined_symbols = main_symbols;
		auto combined_code = main_parser.rawCode;
		auto combined_data = main_parser.rawData;
		auto combined_debug = main_parser.copyDebugData();
		auto symbol_types = collectSymbolTypes(main_parser.offsets, combined_symbols);

		desymbolize(combined_code, main_parser.offsets, main_symbols);

		size_t extra_symbol_length = main_parser.rawSymbols.size() * 8;
		size_t extra_code_length = main_parser.getCodeLength();
		size_t extra_data_length = main_parser.getDataLength();
		size_t extra_debug_length = countStringTypes(combined_debug);

		for (const std::vector<Long> &unit: subunits) {
			BinaryParser subparser(unit);
			subparser.parse();

			auto &subcode = subparser.rawCode;
			auto &subdata = subparser.rawData;
			auto &subtable = subparser.symbols;
			depointerize(subtable, subdata, subparser.offsets.data);
			const size_t subcode_length = subparser.getCodeLength();
			const size_t subdata_length = subparser.getDataLength();
			size_t subtable_length = subparser.rawSymbols.size();
			auto &subdebug = subparser.debugData;
			if (subtable.count(".end") != 0) {
				subtable.erase(".end");
				subtable_length -= 3;
			}

			detectSymbolCollisions(combined_symbols, subtable);

			const size_t meta_difference = meta_length - subparser.getMetaLength(); // in bytes!

			desymbolize(subcode, subparser.offsets, subtable);

			for (auto &[symbol, entry]: subtable) {
				const SymbolType type = getSymbolType(subparser.offsets, subtable, symbol);
				if (type == SymbolType::Code)
					entry.address += extra_symbol_length + extra_code_length + meta_difference - 24;
				else if (type == SymbolType::Data)
					entry.address += extra_symbol_length + extra_code_length + extra_data_length + meta_difference - 24;
				else if (symbol != ".end")
					throw std::runtime_error("Encountered symbol \"" + symbol + "\" of unknown type");
				symbol_types[symbol] = type;
			}

			for (auto &[symbol, entry]: combined_symbols) {
				const SymbolType type = symbol_types.at(symbol);
				if (type == SymbolType::Code)
					entry.address += subtable_length * 8;
				else if (type == SymbolType::Code || symbol == ".end") // TODO: is `symbol == ".end"` correct?
					entry.address += subtable_length * 8 + subcode_length;
			}

			for (auto &entry: combined_debug)
				if (entry->getType() == DebugEntry::Type::Location) {
					auto *location = static_cast<DebugLocation *>(entry.get());
					if (location->address)
						location->address += subtable_length * 8;
				}

			for (auto &entry: subdebug)
				if (entry->getType() == DebugEntry::Type::Location) {
					auto *location = static_cast<DebugLocation *>(entry.get());
					if (location->address)
						location->address += extra_symbol_length + extra_code_length + meta_difference - 24;
					location->fileIndex += extra_debug_length;
					location->functionIndex += extra_debug_length;
				}

			extra_symbol_length += subtable_length * 8;
			extra_code_length += subcode_length;
			extra_data_length += subdata_length;

			std::vector<std::shared_ptr<DebugEntry>> types1and2;
			Util::filter(subdebug, types1and2, [](const auto &entry) {
				const auto type = entry->getType();
				return type == DebugEntry::Type::Filename || type == DebugEntry::Type::Function;
			});

			extra_debug_length += types1and2.size();

			for (const auto &[key, value]: subtable)
				if (combined_symbols.count(key) == 0)
					combined_symbols.try_emplace(key, value);
				else
					combined_symbols.at(key) = value;

			for (const Long piece: subcode)
				combined_code.push_back(piece);

			for (const Long piece: subdata)
				combined_data.push_back(piece);

			combined_debug.insert(combined_debug.begin() + extra_debug_length + 1,
				types1and2.begin(), types1and2.end());

			for (const auto &entry: subdebug)
				if (entry->getType() == DebugEntry::Type::Location)
					combined_debug.push_back(entry);
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

	void Linker::desymbolize(std::vector<Long> &longs, const Offsets &offsets, const SymbolTable &table) {
		const size_t longs_size = longs.size();
		for (size_t i = 0; i < longs_size; ++i) {
			const auto parsed = std::unique_ptr<AnyBase>(BinaryParser::parse(longs[i]));
			if (parsed->flags == static_cast<uint16_t>(LinkerFlags::KnownSymbol)) {
				if (parsed->type != AnyBase::Type::I && parsed->type != AnyBase::Type::J)
					throw std::runtime_error("Found an instruction not of type I or J with KnownSymbol set at " +
						Util::toHex(i * 8 + offsets.code));
				const uint32_t immediate = static_cast<AnyImmediate *>(parsed.get())->immediate;
				const std::string name = findSymbolFromAddress(immediate, table, offsets.end);
				if (name.empty() || table.count(name) == 0)
					throw std::runtime_error("Couldn't find a symbol corresponding to " + Util::toHex(immediate)
						+ " while desymbolizing.");

				const uint32_t id = table.at(name).id;
				if (parsed->type == AnyBase::Type::I) {
					const auto *itype = static_cast<AnyI *>(parsed.get());
					longs[i] = Assembler::compileI(itype->opcode, itype->rs, itype->rd, id,
						static_cast<uint8_t>(LinkerFlags::SymbolID), itype->condition);
				} else {
					const auto *jtype = static_cast<AnyJ *>(parsed.get());
					longs[i] = Assembler::compileJ(jtype->opcode, jtype->rs, jtype->immediate, jtype->link,
						static_cast<uint8_t>(LinkerFlags::SymbolID), jtype->condition);
				}
			}
		}
	}

	std::string Linker::findSymbolFromAddress(Long address, const SymbolTable &table, Long end_offset) {
		for (const auto &[key, value]: table)
			if (value.address == address)
				return key;
		return address == end_offset? ".end" : "";
	}

	size_t Linker::countStringTypes(std::vector<std::shared_ptr<DebugEntry>> &entries) {
		size_t out = 0;
		for (const auto &entry: entries) {
			const auto type = entry->getType();
			if (type == DebugEntry::Type::Filename || type == DebugEntry::Type::Function)
				++out;
		}
		return out;
	}

	void Linker::detectSymbolCollisions(const SymbolTable &one, const SymbolTable &two) {
		for (const auto &[key, value]: one)
			if (key != ".end" && two.count(key) != 0)
				throw std::runtime_error("Encountered a symbol collision: \"" + key + "\"");
	}
}
