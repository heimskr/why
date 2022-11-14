#include <cassert>
#include <fstream>
#include <optional>

#include "parser/Lexer.h"
#include "parser/Parser.h"
#include "util/Util.h"
#include "wasm/Assembler.h"
#include "wasm/BinaryParser.h"
#include "wasm/Linker.h"

namespace Wasmc {
	void Linker::addFile(const std::filesystem::path &path) {
		if (paths.count(path) != 0) {
			warn() << "Skipping file \e[1m" << path << "\e[22m: already included\n";
			return;
		}

		if (!std::filesystem::exists(path))
			throw std::runtime_error("Can't add file: path doesn't exist");

		std::ifstream stream(path);
		if (!stream)
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
				longs.push_back(Util::swapEndian(Util::parseUlong(line, 16)));
			if (firstDone)
				subunits.push_back(std::move(longs));
			else
				mainUnit = std::move(longs);
			firstDone = true;
		} else if (firstDone) {
			throw std::runtime_error("Only the first input file can be non-compiled assembly");
		} else {
			firstDone = true;
			std::string text;
			stream.seekg(0, std::ios::end);
			const std::streampos tellg = stream.tellg();
			if (tellg == -1 || !stream)
				throw std::runtime_error("Input stream is in an invalid state");
			text.reserve(tellg);
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
			mainUnit = std::move(assembler.concatenated);
			for (const ASTNode *node: *root)
				if (node->symbol == WASMTOK_INCLUDE_HEADER)
					for (const ASTNode *sub: *node->front())
						addFile(std::filesystem::canonical(path.parent_path() / sub->unquote()));
		}
	}

	std::string Linker::link() {
		if (mainUnit.empty())
			throw std::runtime_error("Can't link before any files are added");

		// Step 1
		BinaryParser main_parser(mainUnit);
		main_parser.parse();

		// Step 2
		const size_t meta_length = main_parser.getMetaLength();
		const std::vector<SymbolTableEntry> &main_symbols = main_parser.symbols;
		std::vector<SymbolTableEntry> combined_symbols = main_symbols;
		std::map<std::string, size_t> combined_symbol_indices = main_parser.symbolIndices;
		auto combined_code = main_parser.instructions;
		std::vector<Long> combined_data = main_parser.rawData;
		std::vector<std::shared_ptr<Wasmc::DebugEntry>> combined_debug = main_parser.copyDebugData();
		std::vector<RelocationData> combined_relocation = main_parser.relocationData;
		const size_t symbol_table_length = main_parser.rawSymbols.size();

		// Step 3
		size_t extra_symbol_length = symbol_table_length * 8;
		size_t extra_code_length = main_parser.getCodeLength();
		size_t extra_data_length = main_parser.getDataLength();
		size_t extra_debug_length = countStringTypes(combined_debug);
		size_t extra_relocation_length = main_parser.getRelocationLength();

		// Step 4
		for (const std::vector<Long> &unit: subunits) {
			BinaryParser subparser(unit);
			subparser.parse();

			auto &subcode = subparser.instructions;
			std::vector<Long> &subdata = subparser.rawData;
			std::vector<SymbolTableEntry> &subtable = subparser.symbols;
			std::map<std::string, size_t> &subindices = subparser.symbolIndices;
			std::vector<RelocationData> &subrelocation = subparser.relocationData;
			std::vector<std::shared_ptr<Wasmc::DebugEntry>> &subdebug = subparser.debugData;

			const size_t subcode_length = subparser.getCodeLength();
			const size_t subdata_length = subparser.getDataLength();
			size_t subtable_length = subparser.rawSymbols.size();
			const size_t subrelocation_length = subparser.getRelocationLength();

			if (subindices.count(".end") != 0) {
				const size_t end_index = subindices.at(".end");
				subtable.erase(subtable.begin() + subindices.at(".end"));
				subindices.erase(".end");
				for (auto &[key, index]: subindices)
					if (end_index <= index)
						--index;
				for (auto &entry: subrelocation)
					if (ssize_t(end_index) <= entry.symbolIndex)
						--entry.symbolIndex;
				subtable_length -= 3;
			}

			detectSymbolCollisions(combined_symbol_indices, subindices, combined_symbols, subtable);

			const ssize_t meta_difference = meta_length - subparser.getMetaLength(); // in bytes!

			for (auto &[symbol, index]: subindices) {
				auto &entry = subtable.at(index);
				if (entry.type == SymbolEnum::Code)
					// For each code symbol in the included symbol table, increase its address
					entry.address += extra_code_length;
				else if (entry.type == SymbolEnum::Data)
					// For each data symbol in the included symbol table, increase its address
					entry.address += extra_data_length;
			}

			for (auto &entry: subdebug)
				if (entry->getType() == DebugEntry::Type::Location) {
					DebugLocation *location = static_cast<DebugLocation *>(entry.get());
					if (location->address)
						location->address += meta_difference + extra_code_length;
					location->fileIndex += extra_debug_length;
					location->functionIndex += extra_debug_length;
				}

			for (auto &entry: subrelocation) {
				entry.sectionOffset += entry.isData? extra_data_length : extra_code_length;
				entry.label = StringSet::intern(subtable.at(entry.symbolIndex).label);
				combined_relocation.emplace_back(std::move(entry));
			}

			std::vector<std::shared_ptr<DebugEntry>> types1and2;
			Util::filter(subdebug, types1and2, [](const std::shared_ptr<DebugEntry> &entry) {
				const auto type = entry->getType();
				return type == DebugEntry::Type::Filename || type == DebugEntry::Type::Function;
			});

			for (const auto &[key, index]: subindices) {
				auto &entry = subtable.at(index);
				if (combined_symbol_indices.count(key) == 0) {
					combined_symbol_indices.try_emplace(key, combined_symbols.size());
					combined_symbols.emplace_back(entry);
				} else {
					auto &existing = combined_symbols.at(combined_symbol_indices.at(key));
					const bool entry_unknown = isUnknown(entry.type);
					const bool existing_unknown = isUnknown(existing.type);
					if (existing_unknown && !entry_unknown)
						existing = entry;
					else if (!existing_unknown && entry_unknown)
						entry = existing;
				}
			}

			extra_symbol_length += subtable_length * 8;
			extra_code_length += subcode_length;
			extra_data_length += subdata_length;
			extra_relocation_length += subrelocation_length;

			combined_code.insert(combined_code.end(), subcode.cbegin(), subcode.cend());
			combined_data.insert(combined_data.end(), subdata.cbegin(), subdata.cend());

			if (combined_debug.size() < extra_debug_length)
				throw std::runtime_error("Cannot insert into debug data: combined_debug.size() is " +
					std::to_string(combined_debug.size()) + " and extra_debug_length is " +
					std::to_string(extra_debug_length));

			combined_debug.insert(combined_debug.begin() + extra_debug_length, types1and2.begin(), types1and2.end());

			extra_debug_length += types1and2.size();

			for (const std::shared_ptr<DebugEntry> &entry: subdebug)
				if (entry->getType() == DebugEntry::Type::Location)
					combined_debug.push_back(entry);
		}

		if (combined_symbol_indices.count(".end") == 0) {
			combined_symbol_indices.try_emplace(".end", combined_symbols.size());
			combined_symbols.emplace_back(".end", 0, SymbolEnum::Unknown);
		}

		const std::vector<Long> encoded_debug = encodeDebugData(combined_debug);
		combined_symbols.at(combined_symbol_indices.at(".end")).address = 8 * (main_parser.rawMeta.size()
			+ encodeSymbolTable(combined_symbols).size() + combined_code.size() + combined_data.size()
			+ encoded_debug.size());
		const std::vector<Long> encoded_combined_symbols = encodeSymbolTable(combined_symbols);

		std::vector<Long> &meta = main_parser.rawMeta;
		meta.at(1) = meta.at(0) + combined_code.size() * 12;
		meta.at(2) = meta.at(1) + combined_data.size() * 8;
		meta.at(3) = meta.at(2) + encoded_combined_symbols.size() * 8;
		meta.at(4) = meta.at(3) + encoded_debug.size() * 8;

		applyRelocation(combined_relocation, combined_symbols, combined_symbol_indices, combined_data, combined_code,
		                meta.at(1), meta.at(0));
		const std::vector<Long> encoded_relocation = encodeRelocationData(combined_relocation);

		meta.at(5) = meta.at(4) + encoded_relocation.size() * 8;

		linked.clear();

		std::vector<uint8_t> code_bytes;
		code_bytes.reserve(combined_code.size() * 12);

		for (const auto &instruction: combined_code) {
			const auto instruction_bytes = instruction.toBytes();
			code_bytes.insert(code_bytes.end(), instruction_bytes.cbegin(), instruction_bytes.cend());
		}

		const auto code_longs = Util::getLongs(code_bytes);

		for (const auto &longs: {meta, code_longs, combined_data, encoded_combined_symbols, encoded_debug,
		                         encoded_relocation})
			linked.insert(linked.end(), longs.cbegin(), longs.cend());

		return Assembler::stringify(linked);
	}

	void Linker::applyRelocation(std::vector<RelocationData> &relocation,
	                             const std::vector<SymbolTableEntry> &symbols,
	                             const std::map<std::string, size_t> &symbol_indices,
	                             std::vector<Long> &data, std::vector<TypedInstruction> &code,
	                             Long data_offset, Long code_offset) {
		for (auto &entry: relocation) {
			if (!entry.label) {
				if (entry.symbolIndex != -1)
					entry.label = StringSet::intern(symbols.at(entry.symbolIndex).label);
			} else
				entry.symbolIndex = symbol_indices.at(*entry.label);

			if (entry.symbolIndex == -1) {
				warn() << "Can't find symbol entry for label " << (entry.label? *entry.label : "nullptr") << ".\n";
				continue;
			}

			const SymbolTableEntry &symbol = symbols.at(entry.symbolIndex);
			long new_value = symbol.address + entry.offset;
			switch (symbol.type) {
				case SymbolEnum::Code: new_value += code_offset; break;
				case SymbolEnum::Data: new_value += data_offset; break;
				case SymbolEnum::UnknownData:
				case SymbolEnum::UnknownCode:
				case SymbolEnum::Unknown:
					// There's not really any reason to apply a relocation if the symbol is still unknown.
					continue;
				default:
					throw std::runtime_error("Unhandled SymbolEnum encountered in Linker::applyRelocation: " +
						std::to_string(int(symbol.type)));
			}

			if (entry.isData) {
				if (entry.sectionOffset % 8 != 0) {
					Long &first  = data[entry.sectionOffset / 8];
					Long &second = data[entry.sectionOffset / 8 + 1];
					auto bytes = Util::getBytes(first);
					const auto second_bytes = Util::getBytes(second);
					bytes.reserve(16);
					bytes.insert(bytes.end(), second_bytes.cbegin(), second_bytes.cend());
					if (entry.type == RelocationType::Full)
						for (int i = 0; i < 8; ++i)
							bytes[i + entry.sectionOffset % 8] = uint8_t(new_value >> (8 * i));
					else if (entry.type == RelocationType::Lower4)
						for (int i = 0; i < 4; ++i)
							bytes[i + entry.sectionOffset % 8] = uint8_t(new_value >> (8 * i));
					else if (entry.type == RelocationType::Upper4)
						for (int i = 0; i < 4; ++i)
							bytes[i + entry.sectionOffset % 8] = uint8_t(new_value >> (32 + (8 * i)));
					else
						throw std::runtime_error("Invalid RelocationType: " + std::to_string(int(entry.type)));
					auto adjusted = Util::getLongs(bytes);
					assert(adjusted.size() == 2);
					first = adjusted[0];
					second = adjusted[1];
				} else {
					Long &value = data[entry.sectionOffset / 8];
					if (entry.type == RelocationType::Full) {
						value = new_value;
					} else if (entry.type == RelocationType::Lower4 || entry.type == RelocationType::Upper4) {
						auto bytes = Util::getBytes(value);
						if (entry.type == RelocationType::Lower4) {
							bytes[0] = uint8_t(new_value);
							bytes[1] = uint8_t(new_value >>  8);
							bytes[2] = uint8_t(new_value >> 16);
							bytes[3] = uint8_t(new_value >> 24);
						} else {
							bytes[0] = uint8_t(new_value >> 32);
							bytes[1] = uint8_t(new_value >> 40);
							bytes[2] = uint8_t(new_value >> 48);
							bytes[3] = uint8_t(new_value >> 56);
						}
						auto adjusted = Util::getLongs(bytes);
						assert(adjusted.size() == 1);
						value = adjusted.front();
					} else
						throw std::runtime_error("Invalid RelocationType: " + std::to_string(int(entry.type)));
				}
			} else {
				// if ((entry.sectionOffset - 4) % 12 != 0) {
				// 	error() << std::string(entry) << '\n';
				// 	throw std::runtime_error("Code symbol section offset (" + std::to_string(entry.sectionOffset) +
				// 		") must be divisible by 12 after subtracting 4");
				// }

				// TODO!: verify correctness, something's probably amiss

				Long &instruction = code.at(entry.sectionOffset / 12).instruction;

				if (entry.type == RelocationType::Lower4) {
					if (0xffffffff < new_value)
						throw std::runtime_error("New value too high: 0x" + Util::hex(new_value));
					instruction &= ~0xffffffff;
					instruction |= new_value;
				} else if (entry.type == RelocationType::Upper4) {
					instruction &= ~0xffffffff;
					instruction |= new_value >> 32;
				} else {
					throw std::runtime_error("Code relocation has invalid type: " +
						std::to_string(static_cast<int>(entry.type)));
				}
			}
		}
	}

	std::string Linker::findSymbolFromAddress(Long address, const SymbolTable &table, Long end_offset) {
		for (const auto &[symbol, entry]: table)
			if (entry.address == address)
				return symbol;
		return address == end_offset? ".end" : "";
	}

	std::string Linker::findSymbolFromID(uint32_t id, const SymbolTable &table) {
		for (const auto &[symbol, entry]: table)
			if (entry.id == id)
				return symbol;
		return "";
	}

	size_t Linker::countStringTypes(const std::vector<std::shared_ptr<DebugEntry>> &entries) {
		size_t out = 0;
		for (const std::shared_ptr<DebugEntry> &entry: entries) {
			const DebugEntry::Type type = entry->getType();
			if (type == DebugEntry::Type::Filename || type == DebugEntry::Type::Function)
				++out;
		}
		return out;
	}

	void Linker::detectSymbolCollisions(const std::map<std::string, size_t> &one_indices,
	                                    const std::map<std::string, size_t> &two_indices,
	                                    const std::vector<SymbolTableEntry> &one_table,
	                                    const std::vector<SymbolTableEntry> &two_table) {
		for (const auto &[key, value]: one_indices) {
			if (key != ".end" && two_indices.count(key) != 0) {
				const auto &first = one_table.at(value);
				const auto &second = two_table.at(two_indices.at(key));
				const bool first_unknown = isUnknown(first.type);
				const bool second_unknown = isUnknown(second.type);
				if (first_unknown || second_unknown)
					// Not a collision if one of the symbol tables includes it only for relocation purposes.
					continue;
				std::cerr << "key[" << key << "], type1[" << int(first.type) << "], type2[" << int(second.type) << ":"
				          << second.label << "]\nSecond {\n";
				for (const auto &[key2, index2]: two_indices)
					std::cerr << "\t" << key2 << " => " << int(two_table.at(index2).type) << "\n";
				std::cerr << "}\n";
				throw std::runtime_error("Encountered a symbol collision: \"" + key + "\"");
			}
		}
	}

	std::vector<Long> Linker::encodeDebugData(const std::vector<std::shared_ptr<Wasmc::DebugEntry>> &debug) {
		std::vector<Long> out;

		for (const std::shared_ptr<DebugEntry> &entry: debug) {
			const DebugEntry::Type type = entry->getType();
			if (type == DebugEntry::Type::Filename || type == DebugEntry::Type::Function) {
				const std::string *value = entry->getString();
				const size_t length = value->size();
				if (0xffffff < length)
					throw std::runtime_error("Value for type " + std::to_string(static_cast<int>(type)) + " is too long"
						" (" + std::to_string(length) + " characters)");
				std::vector<uint8_t> bytes {
					static_cast<uint8_t>(type),
					static_cast<uint8_t>(length & 0xff),
					static_cast<uint8_t>((length >> 8) & 0xff),
					static_cast<uint8_t>((length >> 16) & 0xff)
				};
				bytes.insert(bytes.end(), value->begin(), value->end());
				for (const Long piece: Util::getLongs(bytes))
					out.push_back(piece);
			} else if (type == DebugEntry::Type::Location) {
				const DebugLocation &location = static_cast<DebugLocation &>(*entry);
				if (0xff < location.count)
					warn() << "Instruction count too high: " << location.count << "\n";
				if (0xffffff < location.fileIndex)
					warn() << "File index too high: " << location.fileIndex << "\n";
				if (0xffffffff < location.line)
					warn() << "Line number too high: " << location.line << "\n";
				if (0xffffff < location.column)
					warn() << "Column number too high: " << location.column << "\n";
				if (0xffffff < location.functionIndex)
					warn() << "Function index too high: " << location.functionIndex << "\n";
				std::vector<uint8_t> bytes {static_cast<uint8_t>(type)};
				auto add = [&](size_t number, char byte_count) {
					for (char i = 0; i < byte_count; ++i)
						bytes.push_back((number >> (8 * i)) & 0xff);
				};
				add(location.fileIndex, 3);
				add(location.line, 4);
				add(location.column, 3);
				bytes.push_back(location.count);
				add(location.functionIndex, 4);
				add(location.address, 8);

				for (const Long piece: Util::getLongs(bytes))
					out.push_back(piece);
			} else
				throw std::runtime_error("Invalid DebugEntry type: " + std::to_string(static_cast<int>(type)));
		}

		return out;
	}

	std::vector<Long> Linker::encodeSymbolTable(const std::vector<SymbolTableEntry> &table) {
		std::vector<Long> out;

		for (const auto &entry: table) {
			if (entry.label.empty())
				throw std::runtime_error("Symbol table entry is missing a label in Linker::encodeSymbolTable");
			out.push_back((static_cast<uint64_t>(entry.id) << 32)
				| (Util::updiv(entry.label.size(), size_t(8)) & 0xffff) | (size_t(entry.type) << 16));
			out.push_back(entry.address);
			for (const Long piece: Util::getLongs(entry.label))
				out.push_back(piece);
		}

		return out;
	}

	std::vector<Long> Linker::encodeRelocationData(const std::vector<RelocationData> &relocation) {
		std::vector<Long> out;

		for (const auto &entry: relocation) {
			auto encoded = entry.encode();
			out.insert(out.end(), encoded.cbegin(), encoded.cend());
		}

		return out;
	}
}
