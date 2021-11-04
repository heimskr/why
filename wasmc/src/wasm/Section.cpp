#include "wasm/Section.h"

namespace Wasmc {
	Section::ValueType & Section::operator[](size_t index) {
		return bytes[index];
	}

	const Section::ValueType & Section::operator[](size_t index) const {
		return bytes[index];
	}

	void Section::append(const std::string &string) {
		char *pointer = extend<char>(string.size());
		for (char ch: string)
			*pointer++ = ch;
		counter += string.size();
	}

	size_t Section::alignUp(size_t alignment) {
		if (counter % alignment != 0) {
			size_t to_add = alignment - (counter % alignment);
			extend<char>(to_add);
			counter += to_add;
		}

		return counter;
	}

	void Section::clear() {
		bytes.clear();
		counter = 0;
	}

	std::vector<Section::ValueType> Section::combine(std::initializer_list<std::reference_wrapper<Section>> list) {
		std::vector<Section::ValueType> out;
		for (Section &section: list)
			out.insert(out.end(), section.bytes.begin(), section.bytes.end());
		return out;
	}
}
