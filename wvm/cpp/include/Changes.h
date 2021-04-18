#ifndef WVM_CHANGES_H_
#define WVM_CHANGES_H_

#include "Defs.h"

namespace WVM {
	class VM;

	struct Change {
		virtual void apply(VM &, bool strict = false) = 0;
		virtual void undo(VM &, bool strict = false)  = 0;
		virtual ~Change() {}
	};

	struct MemoryChange: public Change {
		UWord address, from, to;
		Size size;

		MemoryChange(Word address_, Word from_, Word to_, Size size_):
			address(address_), from(from_), to(to_), size(size_) {}
		MemoryChange(const VM &, Word address_, Word to_, Size size_);

		void apply(VM &, bool strict = false) override;
		void undo(VM &, bool strict = false) override;
	};

	struct RegisterChange: public Change {
		UByte reg;
		Word from, to;

		RegisterChange(UByte reg_, Word from_, Word to_): reg(reg_), from(from_), to(to_) {}
		RegisterChange(const VM &, UByte reg_, Word to_);

		void apply(VM &, bool strict = false) override;
		void undo(VM &, bool strict = false) override;
	};

	struct JumpChange: public Change {
		Word from, to;
		Word returnFrom = -1, returnTo = -1;
		bool link;

		JumpChange(Word from_, Word to_): from(from_), to(to_), link(false) {}
		JumpChange(Word from_, Word to_, Word return_from, Word return_to):
			from(from_), to(to_), returnFrom(return_from), returnTo(return_to), link(true) {}
		JumpChange(const VM &, Word to_, bool link_);

		void apply(VM &, bool strict = false) override;
		void undo(VM &, bool strict = false) override;
	};

	struct InterruptTableChange: public Change {
		Word from, to;

		InterruptTableChange(Word from_, Word to_): from(from_), to(to_) {}
		InterruptTableChange(const VM &, Word to_);

		void apply(VM &, bool strict = false) override;
		void undo(VM &, bool strict = false) override;
	};

	struct RingChange: public Change {
		Ring from, to;

		RingChange(Ring from_, Ring to_): from(from_), to(to_) {}
		RingChange(const VM &, Ring to_);

		void apply(VM &, bool strict = false) override;
		void undo(VM &, bool strict = false) override;
	};

	struct HaltChange: public Change {
		void apply(VM &, bool strict = false) override;
		void undo(VM &, bool strict = false) override;
	};

	struct PagingChange: public Change {
		bool from, to;

		PagingChange(bool from_, bool to_): from(from_), to(to_) {}
		PagingChange(const VM &, bool to_);

		void apply(VM &, bool strict = false) override;
		void undo(VM &, bool strict = false) override;
	};

	struct P0Change: public Change {
		Word from, to;

		P0Change(Word from_, Word to_): from(from_), to(to_) {}
		P0Change(const VM &, Word to_);

		void apply(VM &, bool strict = false) override;
		void undo(VM &, bool strict = false) override;
	};
}

#endif
