#ifndef WVM_CHANGES_H_
#define WVM_CHANGES_H_

#include "Defs.h"

namespace WVM {
	class VM;

	struct Change {
		virtual void apply(VM &, bool strict = false) = 0;
		virtual void undo(VM &, bool strict = false)  = 0;
	};

	struct MemoryChange: public Change {
		Word address, from, to;
		Size size;
		MemoryChange(Word address_, Word from_, Word to_, Size size_):
			address(address_), from(from_), to(to_), size(size_) {}

		void apply(VM &, bool strict = false) override;
		void undo(VM &, bool strict = false) override;
	};

	struct RegisterChange: public Change {
		UByte reg;
		Word from, to;
		RegisterChange(UByte reg_, Word from_, Word to_): reg(reg_), from(from_), to(to_) {}

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

		void apply(VM &, bool strict = false) override;
		void undo(VM &, bool strict = false) override;
	};
}

#endif
