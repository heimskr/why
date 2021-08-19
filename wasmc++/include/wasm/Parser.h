#pragma once

#include <vector>

#include "wasm/Types.h"

namespace Wasmc {
	class Parser {
		public:
			Parser(const std::vector<Long> &);

			static AnyBase * parse(Long);

		private:
			std::vector<Long> longs;
	};
}
