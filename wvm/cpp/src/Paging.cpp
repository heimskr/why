#include "Paging.h"

namespace WVM {
	Address::Address(Word in):
		p0Offset(in >> 56),
		p1Offset((in >> 48) & 0xff),
		p2Offset((in >> 40) & 0xff),
		p3Offset((in >> 32) & 0xff),
		p4Offset((in >> 24) & 0xff),
		p5Offset((in >> 16) & 0xff),
		pageOffset(in & 0xffff) {}

	P04Entry::P04Entry(Word in): next(in >> 8), unused(0), present(in & 1) {}

	Word P04Entry::getNext() const {
		return next << 8;
	}

	P5Entry::P5Entry(Word in):
		pageStart(in >> 16),
		unused(0),
		modified  ((in >> 5) & 1),
		accessed  ((in >> 4) & 1),
		user      ((in >> 3) & 1),
		executable((in >> 2) & 1),
		writable  ((in >> 1) & 1),
		present(in & 1) {}

	Word P5Entry::getStart() const {
		return pageStart << 16;
	}

	PageMeta::PageMeta(const P5Entry &p5):
		present(p5.present),
		writable(p5.writable),
		executable(p5.executable),
		user(p5.user),
		accessed(p5.accessed),
		modified(p5.modified) {}
}
