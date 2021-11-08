#include <iostream>

#include "wasm/Section.h"

namespace Wasmc {
	Section::ValueType & Section::operator[](size_t index) {
		return bytes[index];
	}

	const Section::ValueType & Section::operator[](size_t index) const {
		return bytes[index];
	}

	Section & Section::operator+=(const std::string *label) {
		if (!label)
			throw std::invalid_argument("Can't add a null label");

		labels.try_emplace(counter, label);

		if (allLabels)
			allLabels->insert(label);

		return *this;
	}

	Section & Section::operator+=(const std::pair<size_t, std::shared_ptr<Expression>> &pair) {
		values.try_emplace(counter, pair);
		extend<char>(pair.first);
		return *this;
	}

	Section & Section::operator+=(size_t offset) {
		counter += offset;
		return *this;
	}

	Section & Section::go(size_t new_counter) {
		counter = new_counter;
		return *this;
	}

	size_t Section::alignUp(size_t alignment) {
		if (counter % alignment != 0) {
			size_t to_add = alignment - (counter % alignment);
			extend<char>(to_add);
			*this += to_add;
		}

		return counter;
	}

	void Section::clear() {
		bytes.clear();
		counter = 0;
	}

	size_t Section::size() const {
		return bytes.size();
	}

	std::vector<Long> Section::combine(std::initializer_list<std::reference_wrapper<Section>> list) {
		std::vector<Long> out;
		size_t to_reserve = 0;
		for (Section &section: list) {
			if (section.size() % 8 != 0)
				throw std::invalid_argument("Section " + section.name + " has a size not divisible by eight: " +
					std::to_string(section.size()));
			to_reserve += section.size() / 8;
		}
		out.reserve(to_reserve);
		for (Section &section: list)
			for (size_t i = 0, size = section.size(); i < size; i += 8)
				out.push_back(Long(section.bytes[i])
					| (Long(section.bytes[i + 1]) << 8)
					| (Long(section.bytes[i + 2]) << 16)
					| (Long(section.bytes[i + 3]) << 24)
					| (Long(section.bytes[i + 4]) << 32)
					| (Long(section.bytes[i + 5]) << 40)
					| (Long(section.bytes[i + 6]) << 48)
					| (Long(section.bytes[i + 7]) << 56));
		return out;
	}
}
