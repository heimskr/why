#pragma once

#include <string>
#include <vector>

#include "wasm/Types.h"

namespace Wasmc {
	class BinaryParser {
		public:
			std::vector<Long> rawMeta, rawSymbols;
			std::string name, version, author, orcid;

			BinaryParser(const std::vector<Long> &);

			static AnyBase * parse(Long);

			void parse();

			inline Long getMetaLength() const;
			inline Long getSymbolTableLength() const;
			inline Long getCodeLength() const;
			inline Long getDataLength() const;
			inline Long getDebugLength() const;

			inline Long getMetaOffset() const;
			inline Long getSymbolTableOffset() const;
			inline Long getCodeOffset() const;
			inline Long getDataOffset() const;
			inline Long getDebugOffset() const;
			inline Long getEndOffset() const;

		private:
			std::vector<Long> longs;

			std::vector<Long> slice(size_t begin, size_t end);

			static std::string toString(Long);
	};
}
