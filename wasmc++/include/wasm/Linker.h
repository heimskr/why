#pragma once

#include <filesystem>
#include <set>
#include <vector>

#include "wasm/Types.h"

namespace Wasmc {
	class ASTNode;

	class Linker {
		public:
			Linker() = default;

			void addFile(const std::filesystem::path &);

			std::string link();

			const std::vector<std::vector<Long>> & getUnits() const { return units; }

		private:
			std::set<std::filesystem::path> paths;
			bool firstDone = false;
			const ASTNode *root = nullptr;
			std::vector<std::vector<Long>> units;
			std::vector<Long> linked;
	};
}
