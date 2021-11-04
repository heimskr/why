#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

#include "wasm/Types.h"

namespace Wasmc {
	struct Section {
		using ValueType = uint8_t;

		std::vector<ValueType> bytes;
		std::map<size_t, const std::string *> labels;
		std::unordered_set<const std::string *> *allLabels = nullptr;
		size_t counter = 0;
		std::string name;

		Section(const std::string &name_, std::unordered_set<const std::string *> *all_labels = nullptr,
		size_t count = 0):
			bytes(count, 0), allLabels(all_labels), name(name_) {}

		ValueType & operator[](size_t);
		const ValueType & operator[](size_t) const;
		Section & operator+=(const std::string *label);

		template <typename T>
		T * extend(size_t count) {
			const size_t old_size = bytes.size();
			size_t new_capacity, new_size = bytes.size() + count * sizeof(T);
			for (new_capacity = 1; new_capacity < new_size; new_capacity <<= 1);
			bytes.reserve(new_capacity);
			bytes.resize(new_size, 0);
			return reinterpret_cast<T *>(bytes.data() + old_size);
		}

		template <typename T>
		void append(T item) {
			*extend<T>(1) = item;
			counter += sizeof(T);
		}

		template <typename T, template <typename...> typename C>
		void appendAll(const C<T> &container) {
			T *pointer = extend<T>(container.size());
			for (const T &item: container)
				*pointer++ = item;
			counter += sizeof(T) * container.size();
		}

		void append(const std::string &string) {
			char *pointer = extend<char>(string.size());
			for (char ch: string)
				*pointer++ = ch;
			counter += string.size();
		}

		size_t alignUp(size_t alignment);

		void clear();

		size_t size() const;

		static std::vector<ValueType> combine(std::initializer_list<std::reference_wrapper<Section>>);
	};
}
