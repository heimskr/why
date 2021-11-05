#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "wasm/Types.h"

namespace Wasmc {
	class Expression;

	struct Section {
		using ValueType = uint8_t;

		std::vector<ValueType> bytes;
		std::map<size_t, const std::string *> labels;
		StringPtrSet *allLabels = nullptr;
		/** Maps a counter to a pair of a value size and an expression. */
		std::map<size_t, std::pair<size_t, std::shared_ptr<Expression>>> values;
		size_t counter = 0;
		std::string name;

		Section(const std::string &name_, StringPtrSet *all_labels = nullptr,
		size_t count = 0):
			bytes(count, 0), allLabels(all_labels), name(name_) {}

		ValueType & operator[](size_t);
		const ValueType & operator[](size_t) const;
		Section & operator+=(const std::string *label);
		Section & operator+=(const std::pair<size_t, std::shared_ptr<Expression>> &);
		Section & operator+=(size_t);

		Section & go(size_t);

		template <typename T>
		T * extend(size_t count, uint8_t value = 0) {
			const size_t old_size = bytes.size();
			size_t new_capacity, new_size = bytes.size() + count * sizeof(T);
			for (new_capacity = 1; new_capacity < new_size; new_capacity <<= 1);
			bytes.reserve(new_capacity);
			bytes.resize(new_size, value);
			return reinterpret_cast<T *>(&bytes[old_size]);
		}

		template <typename T>
		void append(T item) {
			*extend<T>(1) = item;
			*this += sizeof(T);
		}

		template <typename T, template <typename...> typename C>
		void appendAll(const C<T> &container) {
			T *pointer = extend<T>(container.size());
			for (const T &item: container)
				*pointer++ = item;
			*this += sizeof(T) * container.size();
		}

		void append(const std::string &string) {
			char *pointer = extend<char>(string.size());
			for (char ch: string)
				*pointer++ = ch;
			*this += string.size();
		}

		template <typename T>
		void insert(size_t offset, const T &item) {
			if (size() < offset + sizeof(T))
				throw std::out_of_range("Can't insert " + std::to_string(sizeof(T)) + " bytes into a Section of size " +
					std::to_string(size()) + " at offset " + std::to_string(offset));
			*reinterpret_cast<T *>(&bytes[offset]) = item;
			counter += sizeof(T);
		}

		template <typename T, template <typename...> typename C>
		void insertAll(size_t offset, const C<T> &container) {
			if (size() < offset + container.size() * sizeof(T))
				throw std::out_of_range("Can't insert " + std::to_string(container.size()) + " * " +
					std::to_string(sizeof(T)) + " bytes into a Section of size " + std::to_string(size()) +
					" at offset " + std::to_string(offset));
			T *pointer = reinterpret_cast<T *>(&bytes[offset]);
			for (const T &item: container)
				*pointer++ = item;
			counter += container.size() * sizeof(T);
		}

		size_t alignUp(size_t alignment);

		void clear();

		size_t size() const;

		static std::vector<ValueType> combine(std::initializer_list<std::reference_wrapper<Section>>);
	};
}
