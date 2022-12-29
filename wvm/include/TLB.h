#pragma once

#include <cstddef>
#include <map>
#include <unordered_set>

#include "Defs.h"

namespace WVM {
	class VM;

	class TLB {
		private:
			size_t size;
			std::map<Word, Word> map;
			std::unordered_set<Word> set;

			void evictOne();

			static Word align(Word);

		public:
			TLB(size_t size_);

			bool translate(VM &, Word virtual_address, Word &physical_address);
			void clear();

			size_t getSize() const { return size; }
	};
}
