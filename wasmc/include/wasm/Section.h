#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace Wasmc {
	struct Section {
		using ValueType = uint8_t;

		std::vector<ValueType> bytes;
		size_t counter = 0;

		Section(size_t count = 0): bytes(count, 0) {}

		ValueType & operator[](size_t);
		const ValueType & operator[](size_t) const;

		template <typename T>
		T * extend(size_t count) {
			T *out = reinterpret_cast<T *>(bytes.data() + bytes.size());
			size_t new_capacity, new_size = bytes.size() + count * sizeof(T);
			for (new_capacity = 1; new_capacity < new_size; new_capacity <<= 1);
			bytes.reserve(new_capacity);
			bytes.resize(bytes.size() + count * sizeof(T), 0);
			return out;
		}

		template <typename T>
		void append(T item) {
			*extend<T>(1) = item;
			counter += sizeof(T);
		}

		template <typename T>
		void append(std::initializer_list<T> list) {
			T *pointer = extend<T>(list.size());
			for (const T &item: list)
				*pointer++ = item;
			counter += sizeof(T) * list.size();
		}

		void append(const std::string &string);

		size_t alignUp(size_t alignment);

		void clear();

		static std::vector<ValueType> combine(std::initializer_list<std::reference_wrapper<Section>>);
	};
}
