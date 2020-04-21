#ifndef WVM_MODE_REGISTERSMODE_H_
#define WVM_MODE_REGISTERSMODE_H_

#include <array>
#include <optional>
#include <sstream>
#include <thread>

#include "haunted/core/terminal.h"
#include "haunted/ui/boxes/expandobox.h"
#include "haunted/ui/textbox.h"
#include "mode/ClientMode.h"
#include "VM.h"
#include "Why.h"

namespace WVM::Mode {
	class RegistersMode: public ClientMode {
		private:
			enum class Base: char {Binary='b', Octal='o', Decimal='d', Hex='h'};

			Base base = Base::Decimal;
			haunted::terminal terminal;
			std::thread networkThread;
			std::optional<haunted::ui::boxes::expandobox> expando;
			haunted::ui::textbox textbox;
			std::array<Word, Why::totalRegisters> registers;
			bool fastForward = false, ready = false;

			haunted::ui::simpleline & getLine(int reg);

		public:
			RegistersMode(): ClientMode() {}
			~RegistersMode();

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
