#pragma once

#include <filesystem>
#include <vector>

#include "wasm/Types.h"

namespace Wasmc {
	class Linker {
		public:
			Linker() = default;

			void addFile(const std::filesystem::path &);

		private:
			std::vector<std::vector<Long>> units;
	};
}
