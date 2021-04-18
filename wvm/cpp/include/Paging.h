#pragma once

#include "Defs.h"

namespace WVM {
	struct Address {
		Word p0Offset: 8;
		Word p1Offset: 8;
		Word p2Offset: 8;
		Word p3Offset: 8;
		Word p4Offset: 8;
		Word p5Offset: 8;
		Word pageOffset: 16;

		Address(Word);
	};

	struct P04Entry {
		Word next:   56;
		int unused:   7;
		bool present: 1;

		P04Entry(Word);
		Word getNext() const;
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
	};

	struct PageMeta {
		bool present;
		bool writable;
		bool executable;
		bool user;
		bool accessed;
		bool modified;

		PageMeta(const P5Entry &);
	};
}
