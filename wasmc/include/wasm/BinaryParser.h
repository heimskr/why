#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "wasm/Debug.h"
#include "wasm/SymbolTable.h"
#include "wasm/Types.h"

namespace Wasmc {
	class BinaryParser {
		public:
			std::vector<Long> raw, rawMeta, rawSymbols, rawCode, rawData, rawDebugData;
			std::string name, version, author, orcid;
			SymbolTable symbols;
			std::vector<std::unique_ptr<AnyBase>> code;
			std::vector<std::shared_ptr<DebugEntry>> debugData;
			Offsets offsets;

			BinaryParser() = delete;
			BinaryParser(const BinaryParser &) = default;
			BinaryParser(BinaryParser &&) = default;

			BinaryParser(const std::vector<Long> &);
			BinaryParser(const std::filesystem::path &);

			BinaryParser & operator=(const BinaryParser &) = default;
			BinaryParser & operator=(BinaryParser &&) = default;

			static AnyBase * parse(Long);

			void parse();

			decltype(debugData) copyDebugData() const;

			Long getMetaLength() const;
			Long getSymbolTableLength() const;
			Long getCodeLength() const;
			Long getDataLength() const;
			Long getDebugLength() const;

			Long getMetaOffset() const;
			Long getSymbolTableOffset() const;
			Long getCodeOffset() const;
			Long getDataOffset() const;
			Long getDebugOffset() const;
			Long getEndOffset() const;

		private:
			std::vector<Long> slice(size_t begin, size_t end);
			SymbolTable getSymbols() const;
			std::vector<std::shared_ptr<DebugEntry>> getDebugData() const;

			static std::string toString(Long);
	};
}
