#pragma once

#include <cstddef>

namespace Wasmc::Util {
	template <typename T>
	struct Hash {
		size_t operator()(const T &data) const {
			size_t out = 0xcbf29ce484222325ul;
			const uint8_t *base = reinterpret_cast<const uint8_t *>(&data);
			for (size_t i = 0; i < sizeof(T); ++i)
				out = (out * 0x00000100000001b3) ^ base[i];
			return out;
		}
	};
}
