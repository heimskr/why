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
}

#endif
