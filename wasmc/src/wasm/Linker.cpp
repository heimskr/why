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
		std::vector<Long> combined_code = main_parser.rawCode;
		std::vector<Long> combined_data = main_parser.rawData;
		// depointerize(combined_symbols, combined_data, main_parser.offsets.data);
		std::vector<std::shared_ptr<Wasmc::DebugEntry>> combined_debug = main_parser.copyDebugData();
		std::vector<RelocationData> combined_relocation = main_parser.relocationData;
		std::unordered_map<std::string, SymbolType> symbol_types =
			collectSymbolTypes(main_parser.offsets, combined_symbols);
		const size_t symbol_table_length = main_parser.rawSymbols.size();

		// desymbolize(combined_code, main_parser.offsets, main_symbols);

		// Step 3
		size_t extra_symbol_length = symbol_table_length * 8;
		size_t extra_code_length = main_parser.getCodeLength();
		size_t extra_data_length = main_parser.getDataLength();
		size_t extra_debug_length = countStringTypes(combined_debug);
		size_t extra_relocation_length = main_parser.getRelocationLength();

		// Step 4
		for (const std::vector<Long> &unit: subunits) {
			// Open the included binary
			BinaryParser subparser(unit);
			subparser.parse();

			std::vector<Long> &subcode = subparser.rawCode;
			std::vector<Long> &subdata = subparser.rawData;
			std::vector<SymbolTableEntry> &subtable = subparser.symbols;
			std::map<std::string, size_t> &subindices = subparser.symbolIndices;
			std::vector<RelocationData> &subrelocation = subparser.relocationData;
			// depointerize(subtable, subdata, subparser.offsets.data);
			const size_t subcode_length = subparser.getCodeLength();
			const size_t subdata_length = subparser.getDataLength();
			size_t subtable_length = subparser.rawSymbols.size();
			const size_t subrelocation_length = subparser.getRelocationLength();
			std::vector<std::shared_ptr<Wasmc::DebugEntry>> &subdebug = subparser.debugData;
			if (subindices.count(".end") != 0) {
				const size_t end_index = subindices.at(".end");
				subtable.erase(subtable.begin() + subindices.at(".end"));
				subindices.erase(".end");
				for (auto &[key, index]: subindices)
					if (end_index <= index)
						--index;
				subtable_length -= 3;
			}

			detectSymbolCollisions(combined_symbol_indices, subindices, combined_symbols, subtable);

			const ssize_t meta_difference = meta_length - subparser.getMetaLength(); // in bytes!

			// desymbolize(subcode, subparser.offsets, subtable);

			for (auto &[symbol, index]: subindices) {
				auto &entry = subtable.at(index);
				const SymbolType type = getSymbolType(subparser.offsets, entry.address);
				if (type == SymbolType::Function || type == SymbolType::Instruction)
					// For each code symbol in the included symbol table, increase its address
					entry.address += meta_difference + extra_code_length;
				else if (type == SymbolType::Object)
					// For each data symbol in the included symbol table, increase its address
					entry.address += meta_difference + extra_code_length + extra_data_length;
				symbol_types[symbol] = type;
			}

			for (auto &[symbol, index]: combined_symbol_indices) {
				auto &entry = combined_symbols.at(index);
				if (symbol_types.at(symbol) == SymbolType::Object)
					entry.address += subcode_length;
			}

			// for (auto &entry: combined_debug)
			// 	if (entry->getType() == DebugEntry::Type::Location) {
			// 		DebugLocation *location = static_cast<DebugLocation *>(entry.get());
			// 		if (location->address)
			// 			location->address += subtable_length * 8;
			// 	}

			for (auto &entry: subdebug)
				if (entry->getType() == DebugEntry::Type::Location) {
					DebugLocation *location = static_cast<DebugLocation *>(entry.get());
					if (location->address)
						location->address += meta_difference + extra_code_length;
					location->fileIndex += extra_debug_length;
					location->functionIndex += extra_debug_length;
				}

			for (auto &entry: subrelocation) {
				if (entry.isData)
					entry.sectionOffset += extra_data_length;
				else
					entry.sectionOffset += extra_code_length;
				entry.label = StringSet::intern(subtable.at(entry.symbolIndex).label);
				combined_relocation.emplace_back(std::move(entry));
			}

			extra_symbol_length += subtable_length * 8;
			extra_code_length += subcode_length;
			extra_data_length += subdata_length;
			extra_relocation_length += subrelocation_length;

			std::vector<std::shared_ptr<DebugEntry>> types1and2;
			Util::filter(subdebug, types1and2, [](const std::shared_ptr<DebugEntry> &entry) {
				const auto type = entry->getType();
				return type == DebugEntry::Type::Filename || type == DebugEntry::Type::Function;
			});

			extra_debug_length += types1and2.size();

			for (const auto &[key, index]: subindices) {
				const auto &entry = subtable.at(index);
				if (combined_symbol_indices.count(key) == 0) {
					combined_symbol_indices.try_emplace(key, combined_symbols.size());
					combined_symbols.emplace_back(entry);
				} else
					combined_symbols.at(combined_symbol_indices.at(key)) = entry;
			}

			for (const Long piece: subcode)
				combined_code.push_back(piece);

			for (const Long piece: subdata)
				combined_data.push_back(piece);

			combined_debug.insert(combined_debug.begin() + extra_debug_length + 1,
				types1and2.begin(), types1and2.end());

			for (const std::shared_ptr<DebugEntry> &entry: subdebug)
				if (entry->getType() == DebugEntry::Type::Location)
					combined_debug.push_back(entry);
		}

		if (combined_symbol_indices.count(".end") == 0) {
			combined_symbol_indices.try_emplace(".end", combined_symbols.size());
			combined_symbols.emplace_back(".end", 0, SymbolEnum::Unknown);
		}

		for (auto &entry: combined_relocation) {
			entry.symbolIndex = combined_symbol_indices.at(*entry.label);
			// TODO!: apply relocation
			if (entry.isData) {
				
			} else {

			}
		}


		const std::vector<Long> encoded_debug = encodeDebugData(combined_debug);
		combined_symbols.at(combined_symbol_indices.at(".end")).address = 8 * (main_parser.rawMeta.size()
			+ encodeSymbolTable(combined_symbols).size() + combined_code.size() + combined_data.size()
			+ encoded_debug.size());
		const std::vector<Long> encoded_combined_symbols = encodeSymbolTable(combined_symbols);
		const size_t code_offset = (encoded_combined_symbols.size() - symbol_table_length) * 8;
		const std::vector<Long> encoded_relocation; // TODO!

		// resymbolize(combined_code, combined_symbols);

		std::vector<Long> &meta = main_parser.rawMeta;
		meta.at(1) = meta.at(0) + combined_code.size() * 8;
		meta.at(2) = meta.at(1) + combined_data.size() * 8;
		meta.at(3) = meta.at(2) + encoded_combined_symbols.size() * 8;
		meta.at(4) = meta.at(3) + encoded_debug.size() * 8;
		meta.at(5) = meta.at(4) + encoded_relocation.size() * 8;

		linked.clear();
		for (const auto &longs: {meta, combined_code, combined_data, encoded_combined_symbols, encoded_debug,
		                         encoded_relocation})
			for (const Long piece: longs)
				linked.push_back(piece);

		// repointerize(combined_symbols, linked);

		return Assembler::stringify(linked);
	}

	void Linker::depointerize(const SymbolTable &table, std::vector<Long> &data, Long data_offset) {
		for (const auto &[key, value]: table) {
			const Long address = value.address;
			const SymbolEnum type = value.type;
			const Long index = (address - data_offset) / 8;
			if (type == SymbolEnum::KnownPointer) {
				const Long current_value = data.at(index);
				std::optional<SymbolTableEntry> match;
				for (const auto &pair: table)
					if (pair.second.address == current_value) {
						match = pair.second;
						break;
					}
				if (!match.has_value())
					throw std::runtime_error("Found no matches for " + Util::toHex(current_value) + " from key \"" + key
						+ "\"");
				data[index] = match->id;
			}
		}
	}

	void Linker::repointerize(const SymbolTable &table, std::vector<Long> &combined) {
		for (const auto &[key, value]: table) {
			const auto address = value.address;
			const auto type = value.type;
			if (type == SymbolEnum::KnownPointer || type == SymbolEnum::UnknownPointer) {
				const auto index = address / 8;
				const std::string ptr = findSymbolFromID(combined[index] & 0xffffffff, table);
				if (table.count(ptr) != 0)
					combined[index] = table.at(ptr).address;
				else
					warn() << "Couldn't find pointer for \e[1m" << key << "\e[22m.\n";
			}
		}
	}

	std::unordered_map<std::string, SymbolType>
	Linker::collectSymbolTypes(const Offsets &offsets, const std::vector<SymbolTableEntry> &table) {
		std::unordered_map<std::string, SymbolType> out;
		for (const SymbolTableEntry &entry: table)
			out[entry.label] = getSymbolType(offsets, entry.address);
		return out;
	}

	SymbolType Linker::getSymbolType(const Offsets &offsets, Long address) {
		if (offsets.code <= address && address < offsets.data)
			return SymbolType::Instruction;
		if (offsets.data <= address && address < offsets.debug)
			return SymbolType::Object;
		return SymbolType::Other;
	}

	void Linker::desymbolize(std::vector<Long> &longs, const Offsets &offsets, const SymbolTable &table) {
		// const size_t longs_size = longs.size();
		// for (size_t i = 0; i < longs_size; ++i) {
		// 	const std::unique_ptr<AnyBase> parsed = std::unique_ptr<AnyBase>(BinaryParser::parse(longs[i]));
		// 	if (parsed->flags == static_cast<uint16_t>(LinkerFlags::KnownSymbol)) {
		// 		if (parsed->type != AnyBase::Type::I && parsed->type != AnyBase::Type::J)
		// 			throw std::runtime_error("Found an instruction not of type I or J with KnownSymbol set at " +
		// 				Util::toHex(i * 8 + offsets.code));
		// 		const uint32_t immediate = static_cast<AnyImmediate *>(parsed.get())->immediate;
		// 		const std::string name = findSymbolFromAddress(immediate, table, offsets.end);
		// 		if (name.empty() || table.count(name) == 0)
		// 			throw std::runtime_error("Couldn't find a symbol corresponding to " + Util::toHex(immediate)
		// 				+ " while desymbolizing.");

		// 		const uint32_t id = table.at(name).id;
		// 		if (parsed->type == AnyBase::Type::I) {
		// 			const auto *itype = static_cast<AnyI *>(parsed.get());
		// 			longs[i] = Assembler::compileI(itype->opcode, itype->rs, itype->rd, id,
		// 				static_cast<uint8_t>(LinkerFlags::SymbolID), itype->condition);
		// 		} else {
		// 			const auto *jtype = static_cast<AnyJ *>(parsed.get());
		// 			longs[i] = Assembler::compileJ(jtype->opcode, jtype->rs, id, jtype->link,
		// 				static_cast<uint8_t>(LinkerFlags::SymbolID), jtype->condition);
		// 		}
		// 	}
		// }
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

	size_t Linker::countStringTypes(std::vector<std::shared_ptr<DebugEntry>> &entries) {
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
				if (first.type == SymbolEnum::Unknown || second.type == SymbolEnum::Unknown) {
					// Not a collision if one of the symbol tables includes it only for relocation purposes.
					continue;
				}
				std::cerr << "key[" << key << "], type1[" << int(first.type) << "], type2[" << int(second.type) << ":" << second.label << "]\n";
				std::cerr << "Second {\n";
				for (const auto &[key2, index2]: two_indices) {
					std::cerr << "\t" << key2 << " => " << int(two_table.at(index2).type) << "\n";
				}
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

	void Linker::resymbolize(std::vector<Long> &instructions, const SymbolTable &table) {
		// size_t offset = 0;
		// for (Long &instruction: instructions) {
		// 	const AnyBase *parsed = BinaryParser::parse(instruction);
		// 	const LinkerFlags flags = static_cast<LinkerFlags>(parsed->flags);
		// 	if (flags == LinkerFlags::SymbolID || flags == LinkerFlags::UnknownSymbol) {
		// 		if (parsed->type != AnyBase::Type::I && parsed->type != AnyBase::Type::J)
		// 			throw std::runtime_error("Found an instruction not of type I or J with "
		// 				+ std::string(flags == LinkerFlags::UnknownSymbol? "UnknownSymbol" : "SymbolID") + " set at "
		// 				"offset " + std::to_string(offset));

		// 		const size_t immediate = dynamic_cast<const AnyImmediate &>(*parsed).immediate;
		// 		const std::string name = findSymbolFromID(immediate, table);

		// 		if (name.empty() || table.count(name) == 0) {
		// 			// Unknown labels in included binaries are okay if they're resolved later.
		// 			// For example, B could reference symbols defined in C without including C,
		// 			// but if A includes B and C, then the symbols will be resolved in the compiled
		// 			// output for A.
		// 			if (flags == LinkerFlags::UnknownSymbol)
		// 				continue;
		// 			throw std::runtime_error("Couldn't find symbol for immediate " + Util::toHex(immediate));
		// 		}

		// 		Long address = table.at(name).address;
		// 		if (0xffffffff < address)
		// 			warn() << "Truncating address of label \e[1m" << name << "\e[22m from \e[1m" << Util::toHex(address)
		// 			       << "\e[22m to \e[1m" << Util::toHex(address & 0xffffffff) << "\e[22m.\n";

		// 		if (parsed->type == AnyBase::Type::I) {
		// 			const AnyI *itype = static_cast<const AnyI *>(parsed);
		// 			instruction = Assembler::compileI(itype->opcode, itype->rs, itype->rd,
		// 				static_cast<uint32_t>(address), static_cast<uint8_t>(LinkerFlags::KnownSymbol),
		// 				itype->condition);
		// 		} else {
		// 			const AnyJ *jtype = static_cast<const AnyJ *>(parsed);
		// 			instruction = Assembler::compileJ(jtype->opcode, jtype->rs, static_cast<uint32_t>(address),
		// 				jtype->link, static_cast<uint8_t>(LinkerFlags::KnownSymbol), jtype->condition);
		// 		}
		// 	}

		// 	offset += 8;
		// }
	}
}
