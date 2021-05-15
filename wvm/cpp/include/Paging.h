#pragma once

#include <string>

#include "Defs.h"

namespace WVM {
	struct Address {
		UWord p0Offset: 8;
		UWord p1Offset: 8;
		UWord p2Offset: 8;
		UWord p3Offset: 8;
		UWord p4Offset: 8;
		UWord p5Offset: 8;
		UWord pageOffset: 16;

		Address(Word);
	};

	struct P04Entry {
		Word next:   56;
		int unused:   7;
		bool present: 1;

		P04Entry(Word);
		Word getNext() const;
		operator std::string() const;
	};

	struct P5Entry {
		Word pageStart: 48;
		int unused:     10;
		bool modified:   1;
		bool accessed:   1;
		bool user:       1;
		bool executable: 1;
		bool writable:   1;
		bool present:    1;

		P5Entry(Word);
		Word getStart() const;
		operator std::string() const;
	};

	struct PageMeta {
		bool present    = false;
		bool writable   = false;
		bool executable = false;
		bool user       = false;
		bool accessed   = false;
		bool modified   = false;

		PageMeta() = default;
		PageMeta(const P5Entry &);
	};

	std::ostream & operator<<(std::ostream &, const P04Entry &);
	std::ostream & operator<<(std::ostream &, const P5Entry &);
}
