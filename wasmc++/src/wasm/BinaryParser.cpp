#include "util/Util.h"
#include "wasm/BinaryParser.h"
#include "wasm/Instructions.h"
#include "wasm/Nodes.h"
#include "wasm/Options.h"

namespace Wasmc {
	BinaryParser::BinaryParser(const std::vector<Long> &longs_): longs(longs_) {}

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
		rawMeta = slice(0, getSymbolTableOffset() / 8);

		const std::vector<Long> nva_longs = slice(getMetaOffset() / 8 + 7, getSymbolTableOffset() / 8);
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

		orcid = toString(longs[getMetaOffset() / 8 + 5]) + toString(longs[getMetaOffset() / 8 + 6]);
		name = nva_string.substr(0, first);
		version = nva_string.substr(first + 1, second - first - 1);
		author = nva_string.substr(second + 1, third - second - 1);

		rawSymbols = slice(getSymbolTableOffset() / 8, getCodeOffset() / 8);
		symbols = getSymbols();

		rawCode = slice(getCodeOffset() / 8, getDataOffset() / 8);
		for (const Long instruction: rawCode)
			code.emplace_back(parse(instruction));
	}

	Long BinaryParser::getMetaLength() const {
		return longs[0];
	}

	Long BinaryParser::getSymbolTableLength() const {
		return longs[1] - longs[0];
	}

	Long BinaryParser::getCodeLength() const {
		return longs[2] - longs[1];
	}

	Long BinaryParser::getDataLength() const {
		return longs[3] - longs[2];
	}

	Long BinaryParser::getDebugLength() const {
		return longs[4] - longs[3];
	}

	Long BinaryParser::getMetaOffset() const {
		return 0;
	}

	Long BinaryParser::getSymbolTableOffset() const {
		return longs[0];
	}

	Long BinaryParser::getCodeOffset() const {
		return longs[1];
	}

	Long BinaryParser::getDataOffset() const {
		return longs[2];
	}

	Long BinaryParser::getDebugOffset() const {
		return longs[3];
	}

	Long BinaryParser::getEndOffset() const {
		return longs[4];
	}

	std::vector<Long> BinaryParser::slice(size_t begin, size_t end) {
		return {longs.begin() + begin, longs.begin() + end};
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
			const uint32_t id = longs[i] >> 32;
			const short length = longs[i] & 0xffff;
			const SymbolType type = static_cast<SymbolType>((longs[i] >> 16) & 0xffff);
			const Long address = longs[i + 1];

			std::string symbol_name;
			symbol_name.reserve(8ul * length);

			for (size_t index = i + 2; index < i + 2 + length; ++index) {
				Long piece = longs[index];
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
}
