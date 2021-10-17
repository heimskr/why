#include "DebugData.h"
#include "StringSet.h"

namespace WVM {
	DebugData::DebugData(const std::string &file_, const std::string &function_, int line_, int column_, int count_,
	                     Word address_):
		file(StringSet::intern(file_)), function(StringSet::intern(function_)), line(line_), column(column_),
		count(count_), address(address_) {}

	DebugData::DebugData(const std::string *file_, const std::string *function_, int line_, int column_, int count_,
	                     Word address_):
		file(file_), function(function_), line(line_), column(column_), count(count_), address(address_) {}

	DebugData::operator std::string() const {
		return *file + ":" + std::to_string(line) + ":" + std::to_string(column) + " (" + *function + ")";
	}
}
