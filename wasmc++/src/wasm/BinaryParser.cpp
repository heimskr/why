#include "util/Util.h"
#include "wasm/BinaryParser.h"
#include "wasm/Instructions.h"
#include "wasm/Nodes.h"
#include "wasm/Options.h"

namespace Wasmc {
	BinaryParser::BinaryParser(const std::vector<Long> &raw_): raw(raw_) {}

	AnyBase * BinaryParser::parse(const Long instruction) {
		auto get = [&](int offset, int length) -> Long {
			return (instruction >> (64 - offset - length)) & ((1 << length) - 1);
		};

		const auto opcode = static_cast<Opcode>(get(0, 12));

		if (opcode == 0)
			return new AnyBase(0, 0, 0, 0);

		if (RTYPES.count(opcode) != 0) {
			const auto rs = get(19, 7);
			const auto rt = get(12, 7);
			const auto rd = get(26, 7);
			const auto function = get(52, 12);
			const auto condition = get(46, 4);
			const auto flags = get(50, 2);
			return new AnyR(opcode, rs, rt, rd, function, condition, flags);
		}
		
		if (ITYPES.count(opcode) != 0) {
			const auto rs = get(18, 7);
			const auto rd = get(25, 7);
			const auto immediate = instruction & 0xffffffff;
			const auto condition = get(12, 4);
			const auto flags = get(16, 2);
			return new AnyI(opcode, rs, rd, immediate, condition, flags);
		}

		if (JTYPES.count(opcode) != 0) {
			const auto rs = get(12, 7);
			const auto link = get(19, 1);
			const auto address = instruction & 0xffffffff;
			const auto condition = get(26, 4);
			const auto flags = get(30, 2);
			return new AnyJ(opcode, rs, link, address, condition, flags);
		}

		throw std::runtime_error("Invalid instruction (opcode " + Util::toHex(opcode) + "): "
			+ Util::toHex(instruction, true));
	}

	void BinaryParser::parse() {
		offsets = {getSymbolTableOffset(), getCodeOffset(), getDataOffset(), getDebugOffset(), getEndOffset()};
		rawMeta = slice(0, offsets.symbolTable / 8);

		const std::vector<Long> nva_longs = slice(7, offsets.symbolTable / 8);
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

		rawSymbols = slice(offsets.symbolTable / 8, offsets.code / 8);
		symbols = getSymbols();

		rawCode = slice(offsets.code / 8, offsets.data / 8);
		for (const Long instruction: rawCode)
			code.emplace_back(parse(instruction));

		rawData = slice(offsets.data / 8, offsets.debug / 8);
		rawDebugData = slice(offsets.debug / 8, offsets.end / 8);

		debugData = getDebugData();
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
		return raw[1] - raw[0];
	}

	Long BinaryParser::getCodeLength() const {
		return raw[2] - raw[1];
	}

	Long BinaryParser::getDataLength() const {
		return raw[3] - raw[2];
	}

	Long BinaryParser::getDebugLength() const {
		return raw[4] - raw[3];
	}

	Long BinaryParser::getMetaOffset() const {
		return 0;
	}

	Long BinaryParser::getSymbolTableOffset() const {
		return raw[0];
	}

	Long BinaryParser::getCodeOffset() const {
		return raw[1];
	}

	Long BinaryParser::getDataOffset() const {
		return raw[2];
	}

	Long BinaryParser::getDebugOffset() const {
		return raw[3];
	}

	Long BinaryParser::getEndOffset() const {
		return raw[4];
	}

	std::vector<Long> BinaryParser::slice(size_t begin, size_t end) {
		return {raw.begin() + begin, raw.begin() + end};
	}

	std::string BinaryParser::toString(Long number) {
		std::string out;
		out.reserve(sizeof(number));
		for (size_t i = 0; i < sizeof(number); ++i)
			out += static_cast<char>((number >> (8 * (sizeof(number) - i - 1))) & 0xff);
		return out;
	}

	SymbolTable BinaryParser::getSymbols() const {
		SymbolTable out;

		const size_t end = getCodeOffset() / 8;

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

				for (size_t offset = removed; offset < sizeof(piece); ++offset)
					symbol_name += static_cast<char>((piece >> (8 * (sizeof(piece) - offset - 1))) & 0xff);
			}

			out.try_emplace(symbol_name, id, address, type);
			i += 2 + length;
		}

		return out;
	}

	std::vector<std::shared_ptr<DebugEntry>> BinaryParser::getDebugData() const {
		std::vector<std::shared_ptr<DebugEntry>> out;

		const size_t start = offsets.debug / 8, end = offsets.end / 8;

		for (size_t i = start; i < end; ++i) {
			Long piece = raw[i];
			const auto get = [&piece](unsigned char index) -> size_t {
				return (piece >> (64 - 8 * (index + 1))) & 0xff;
			};

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
				out.emplace_back(location->setCount(count)->setAddress(Util::swapEndian(raw[++i])));
			} else
				throw std::runtime_error("Invalid debug data entry type: " + std::to_string(type));
		}

		return out;
	}
}
