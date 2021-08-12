#pragma once

#include <string>

#include "Defs.h"

namespace WVM {
	struct DebugData {
		DebugData() = delete;
		DebugData(const std::string &file_, const std::string &function_, int line_, int column_, int count_,
		          Word address_);
		DebugData(const std::string *file_, const std::string *function_, int line_, int column_, int count_,
		          Word address_);

		const std::string *file, *function;
		int line, column, count;
		Word address;

		operator std::string() const;
	};
}
