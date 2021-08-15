#pragma once

#include <string>

namespace Wasmc {
	struct DebugEntry {
		enum class Type: unsigned char {Filename = 1, Function = 2, Location = 3};
		virtual ~DebugEntry() {}
		virtual Type getType() const = 0;
	};

	struct DebugFilename: DebugEntry {
		const std::string filename;
		DebugFilename(const std::string &filename_): filename(filename_) {}
		Type getType() const override { return Type::Filename; }
	};

	struct DebugFunction: DebugEntry {
		const std::string function;
		DebugFunction(const std::string &function_): function(function_) {}
		Type getType() const override { return Type::Function; }
	};

	struct DebugLocation: DebugEntry {
		int fileIndex, line, column, functionIndex;
		DebugLocation(int file_index, int line_, int column_, int function_index):
			fileIndex(file_index), line(line_), column(column_), functionIndex(function_index) {}
		Type getType() const override { return Type::Location; }
	};
}
