#ifndef WVM_MODE_MODE_H_
#define WVM_MODE_MODE_H_

namespace WVM::Mode {
	class Mode {
		protected:
			Mode() {}

		public:
			Mode(const Mode &) = delete;
			Mode(Mode &&) = delete;
	};
}

#endif
