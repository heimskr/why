#include "parser/StringSet.h"

namespace Wasmc {
	std::unordered_set<std::string> StringSet::set;
	StringSet set;

	StringSet::StringSet() {
		StringSet::set.max_load_factor(0.5);
	}

	const std::string * StringSet::intern(const char *str) {
		return intern(std::string(str));
	}

	const std::string * StringSet::intern(const std::string &str) {
		auto handle = set.insert(str);
		return &*handle.first;
	}
}
