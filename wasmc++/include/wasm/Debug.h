#pragma once

#include <string>

namespace Wasmc {
	struct DebugEntry {
		enum class Type {Filename, Function, Location};
		virtual Type getType() const = 0;
	};

	struct DebugFilename: DebugEntry {
		const std::string *filename;
		DebugFilename(const std::string *filename_): filename(filename_) {}
		Type getType() const override { return Type::Filename; }
	};

	struct DebugFunction: DebugEntry {
		const std::string *function;
		DebugFunction(const std::string *function_): function(function_) {}
		Type getType() const override { return Type::Function; }
	};
}
