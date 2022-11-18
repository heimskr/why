#ifndef WVM_MODE_REGISTERSMODE_H_
#define WVM_MODE_REGISTERSMODE_H_

#include <array>
#include <optional>
#include <sstream>
#include <thread>

#include "haunted/core/Terminal.h"
#include "haunted/ui/boxes/ExpandoBox.h"
#include "haunted/ui/Textbox.h"
#include "mode/ClientMode.h"
#include "VM.h"
#include "Why.h"

namespace WVM::Mode {
	class RegistersMode: public ClientMode {
		private:
			enum class Base: char {Binary='b', Octal='o', Decimal='d', Hex='h'};

			Base base = Base::Decimal;
			Haunted::Terminal terminal;
			std::thread networkThread;
			Haunted::UI::Boxes::ExpandoBox *expando;
			Haunted::UI::VectorBox *textbox;
			std::array<Register, Why::totalRegisters> registers;
			bool fastForward = false, ready = false;

			Haunted::UI::SimpleLine<std::vector> & getLine(int reg);

		public:
			RegistersMode(): ClientMode() {}

			void run(const std::string &hostname, int port) override;
			void remakeList();
			void stringify(int, std::stringstream &);
			std::string stringify(int);
			void updateLine(int);
			void stop() override;
			void setFastForward(bool);
			void handleMessage(const std::string &) override;
	};
}

#endif
