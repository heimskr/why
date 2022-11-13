#include <fstream>

#include "util/Util.h"
#include "wasm/BinaryParser.h"
#include "wasm/Instructions.h"
#include "wasm/Nodes.h"
#include "wasm/Options.h"

namespace Wasmc {
	BinaryParser::BinaryParser(const std::vector<Long> &raw_): raw(raw_) {}

	BinaryParser::BinaryParser(const std::filesystem::path &path) {
		if (!std::filesystem::exists(path))
			throw std::runtime_error("Can't construct BinaryParser: file doesn't exist");

		std::ifstream stream(path);
		if (!stream.is_open())
			throw std::runtime_error("Can't construct BinaryParser: can't open file");

		std::string line;
		while (std::getline(stream, line))
			raw.push_back(Util::swapEndian(Util::parseUlong(line, 16)));
	}

	AnyBase * BinaryParser::parse(const TypedInstruction &typed) {
		auto getI = [&](int offset, int length) -> Long {
			return (typed.instruction >> (64 - offset - length)) & ((1 << length) - 1);
		};

		auto getT = [&](int offset, int length) -> Long {
			// getI is weird. Let's do offset + 1 - length because it's better.
			return (typed.typeInfo >> (offset + 1 - length)) & ((1 << length) - 1);
		};

		const auto opcode = static_cast<Opcode>(getI(0, 12));

		if (opcode == 0)
			return new AnyBase(0, 0, 0, 0, 0);

		if (RTYPES.count(opcode) != 0) {
			const auto rs = getI(19, 7);
			const auto rt = getI(12, 7);
			const auto rd = getI(26, 7);
			const auto function = getI(52, 12);
			const auto condition = getI(46, 4);
			const auto flags = getI(50, 2);
			const auto rd_type = getT(7, 8);
			const auto rs_type = getT(15, 8);
			const auto rt_type = getT(23, 8);
			return new AnyR(opcode, rs, rt, rd, function, condition, flags, rs_type, rd_type, rt_type);
		}
		
		if (ITYPES.count(opcode) != 0) {
			const auto rs = getI(18, 7);
			const auto rd = getI(25, 7);
			const auto immediate = typed.instruction & 0xffffffff;
			const auto condition = getI(12, 4);
			const auto flags = getI(16, 2);
			const auto rs_type = getT(15, 8);
			const auto rd_type = getT(7, 8);
			const auto imm_type = getT(23, 8);
			return new AnyI(opcode, rs, rd, immediate, condition, flags, rs_type, rd_type, imm_type);
		}

		if (JTYPES.count(opcode) != 0) {
			const auto rs = getI(12, 7);
			const auto link = getI(19, 1);
			const auto address = typed.instruction & 0xffffffff;
			const auto condition = getI(26, 4);
			const auto flags = getI(30, 2);
			const auto rs_type = getT(7, 8);
			return new AnyJ(opcode, rs, link, address, condition, flags, rs_type, 0b00010000);
		}

		throw std::runtime_error("Invalid instruction (opcode " + Util::toHex(opcode) + "): "
			+ Util::toHex(typed.instruction, 16) + "/" + Util::toHex(typed.typeInfo, 8));
	}

	void BinaryParser::parse() {
		offsets = {
			getCodeOffset(), getDataOffset(), getSymbolTableOffset(), getDebugOffset(), getRelocationOffset(),
			getEndOffset()
		};

		rawMeta = slice(0, offsets.code / 8);

		const std::vector<Long> nva_longs = slice(7, offsets.code / 8);
		std::string nva_string;
		nva_string.reserve(8 * nva_longs.size());
		for (const Long piece: nva_longs)
			nva_string += toString(piece);
		
		const size_t first = nva_string.find('\0');
		if (first == std::string::npos)
			throw std::runtime_error("Invalid name-version-author string");
		const size_t second = nva_string.find('\0', first + 1);
		if (second == std::string::npos)
			throw std::runtime_error("Invalid name-version-author string");
		const size_t third = nva_string.find('\0', second + 1);
		if (third == std::string::npos)
			throw std::runtime_error("Invalid name-version-author string");

		orcid = toString(raw[5]) + toString(raw[6]);
		name = nva_string.substr(0, first);
		version = nva_string.substr(first + 1, second - first - 1);
		author = nva_string.substr(second + 1, third - second - 1);

		rawSymbols = slice(offsets.symbolTable / 8, offsets.debug / 8);
		extractSymbols();

		rawCode = slice(offsets.code / 8, offsets.data / 8);
		// Do nothing if there isn't at least one instruction in the code section.
		if (12 <= offsets.data - offsets.code) {
			for (size_t i = 0; i < offsets.data - offsets.code; i += 12) {
				const Long current = rawCode.at(i / 8);
				const Long next = rawCode.at(i / 8 + 1);
				if (i % 8 == 0) // 8-byte aligned
					code.emplace_back(parse({current, static_cast<TypeInfo>(next >> 32)}));
				else
					code.emplace_back(parse({(current << 32) | (next >> 32),
						static_cast<TypeInfo>(next & 0xffffffff)}));
			}
		}

		rawData = slice(offsets.data / 8, offsets.symbolTable / 8);

		rawDebugData = slice(offsets.debug / 8, offsets.relocation / 8);
		debugData = getDebugData();

		rawRelocation = slice(offsets.relocation / 8, offsets.end / 8);
		relocationData = getRelocationData();
	}

	decltype(BinaryParser::debugData) BinaryParser::copyDebugData() const {
		decltype(debugData) out;
		for (const auto &entry: debugData)
			out.emplace_back(entry->copy());
		return out;
	}

	Long BinaryParser::getMetaLength() const {
		return raw[0];
	}

	Long BinaryParser::getSymbolTableLength() const {
		return getDebugOffset() - getSymbolTableOffset();
	}

	Long BinaryParser::getCodeLength() const {
		return getDataOffset() - getCodeOffset();
	}

	Long BinaryParser::getDataLength() const {
		return getSymbolTableOffset() - getDataOffset();
	}

	Long BinaryParser::getDebugLength() const {
		return getRelocationOffset() - getDebugOffset();
	}

	Long BinaryParser::getRelocationLength() const {
		return getEndOffset() - getRelocationOffset();
	}

	Long BinaryParser::getMetaOffset() const {
		return 0;
	}

	Long BinaryParser::getCodeOffset() const {
		return raw[0];
	}

	Long BinaryParser::getDataOffset() const {
		return raw[1];
	}

	Long BinaryParser::getSymbolTableOffset() const {
		return raw[2];
	}

	Long BinaryParser::getDebugOffset() const {
		return raw[3];
	}

	Long BinaryParser::getRelocationOffset() const {
		return raw[4];
	}

	Long BinaryParser::getEndOffset() const {
		return raw[5];
	}

	std::vector<Long> BinaryParser::slice(size_t begin, size_t end) {
		return {raw.begin() + begin, raw.begin() + end};
	}

	std::string BinaryParser::toString(Long number) {
		std::string out;
		out.reserve(sizeof(number));
		for (size_t i = 0; i < sizeof(number); ++i)
			out += static_cast<char>((number >> (8 * i)) & 0xff);
		return out;
	}

	void BinaryParser::extractSymbols() {
		symbols.clear();
		symbolIndices.clear();

		const size_t end = getDebugOffset() / 8;

		for (size_t i = getSymbolTableOffset() / 8, j = 0; i < end && j < MAX_SYMBOLS; ++j) {
			const uint32_t id = raw[i] >> 32;
			const short length = raw[i] & 0xffff;
			const SymbolEnum type = static_cast<SymbolEnum>((raw[i] >> 16) & 0xffff);
			const Long address = raw[i + 1];

			std::string symbol_name;
			symbol_name.reserve(8ul * length);

			for (size_t index = i + 2; index < i + 2 + length; ++index) {
				Long piece = raw[index];
				size_t removed = 0;
				// Take the next long and ignore any null bytes at the least significant end.
				while (piece && (piece & 0xff) == 0) {
					piece >>= 8;
					++removed;
				}

				for (size_t offset = removed; offset < sizeof(piece); ++offset) {
					char ch = static_cast<char>((piece >> (8 * offset)) & 0xff);
					if (ch == '\0')
						break;
					symbol_name += ch;
				}
			}

			symbolIndices.try_emplace(symbol_name, symbols.size());
			symbols.emplace_back(id, symbol_name, address, type);
			i += 2 + length;
		}
	}

	std::vector<std::shared_ptr<DebugEntry>> BinaryParser::getDebugData() const {
		std::vector<std::shared_ptr<DebugEntry>> out;

		const size_t start = offsets.debug / 8, end = offsets.relocation / 8;
		Long piece;
		const auto get = [&piece](unsigned char index) -> size_t {
			return (piece >> (8 * index)) & 0xff;
		};

		for (size_t i = start; i < end; ++i) {
			piece = raw[i];
			const uint8_t type = get(0);
			if (type == 1 || type == 2) {
				const size_t name_length = get(1) | (get(2) << 8) | (get(3) << 16);
				std::string debug_name;
				for (size_t j = 4; j < name_length + 4; ++j) {
					const size_t mod = j % 8;
					if (mod == 0)
						piece = raw[++i];
					debug_name += static_cast<char>(get(mod));
				}
				if (type == 1)
					out.emplace_back(new DebugFilename(debug_name));
				else
					out.emplace_back(new DebugFunction(debug_name));
			} else if (type == 3) {
				const size_t file_index = get(1) | (get(2) << 8) | (get(3) << 16);
				const uint32_t line = get(4) | (get(5) << 8) | (get(6) << 16) | (get(7) << 24);
				piece = raw[++i];
				const uint32_t column = get(0) | (get(1) << 8) | (get(2) << 16);
				const uint8_t count = get(3);
				const uint32_t function_index = get(4) | (get(5) << 8) | (get(6) << 16) | (get(7) << 24);
				DebugLocation *location = new DebugLocation(file_index, line, column, function_index);
				out.emplace_back(location->setCount(count)->setAddress(raw[++i]));
			} else {
				throw std::runtime_error("Invalid debug data entry type (" + std::to_string(type) + ") at line "
					+ std::to_string(i + 1) + " of " + std::to_string(raw.size()) + " in " + name);
			}
		}

		return out;
	}

	std::vector<RelocationData> BinaryParser::getRelocationData() const {
		std::vector<RelocationData> out;
		for (size_t i = 0, size = rawRelocation.size(); i < size; i += 3)
			out.emplace_back(rawRelocation[i], rawRelocation[i + 1], rawRelocation[i + 2]);
		return out;
	}
}
