#include "TLB.h"
#include "VM.h"

namespace WVM {
	Word TLB::align(Word address) {
		return address - (address % VM::PAGE_SIZE);
	}

	TLB::TLB(size_t size_): size(size_) {}

	bool TLB::translate(VM &vm, Word virtual_address, Word &physical_address) {
		const auto aligned_virtual = align(virtual_address);
		const auto iter = map.find(aligned_virtual);

		if (iter != map.end()) {
			physical_address = iter->second + (virtual_address % VM::PAGE_SIZE);
			return true;
		}

		bool success = false;
		const Word translated = vm.walkTables(virtual_address, &success);

		if (!success)
			return false;

		if (size <= map.size())
			evictOne();

		map[aligned_virtual] = translated;
		set.insert(translated);
		physical_address = translated + (virtual_address % VM::PAGE_SIZE);
		return true;
	}

	void TLB::evictOne() {
		if (map.empty())
			return;

		const auto iter = set.begin();
		map.erase(*iter);
		set.erase(iter);
	}

	void TLB::clear() {
		map.clear();
		set.clear();
	}
}
