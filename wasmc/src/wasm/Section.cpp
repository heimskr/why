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

	std::vector<Section::ValueType> Section::combine(std::initializer_list<std::reference_wrapper<Section>> list) {
		std::vector<Section::ValueType> out;
		size_t to_reserve = 0;
		for (const Section &section: list)
			to_reserve += section.size();
		out.reserve(to_reserve);
		for (Section &section: list)
			out.insert(out.end(), section.bytes.begin(), section.bytes.end());
		return out;
	}
}
