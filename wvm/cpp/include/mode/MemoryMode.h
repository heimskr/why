#ifndef WVM_MODE_MEMORYMODE_H_
#define WVM_MODE_MEMORYMODE_H_

#include <map>
#include <optional>
#include <thread>
#include <unordered_set>

#include "haunted/core/terminal.h"
#include "haunted/ui/boxes/expandobox.h"
#include "haunted/ui/textbox.h"
#include "mode/ClientMode.h"
#include "VM.h"

namespace WVM::Mode {
	class MemoryMode: public ClientMode {
		private:
			haunted::terminal terminal;
			std::thread networkThread;
			std::optional<haunted::ui::boxes::expandobox> expando;
			haunted::ui::textbox textbox;
			VM vm;
			std::map<int, std::shared_ptr<haunted::ui::textline>> lines;
			std::unordered_set<Word> symbolTableEdges;

		public:
			Word min = 0, max = 0;
			int padding = 5;

			MemoryMode(): ClientMode(), vm(0) {}
			~MemoryMode();

			void run(const std::string &hostname, int port) override;
			void remakeList();
			std::string stringify(Word address) const;
			void updateLine(Word address);
			void makeSymbolTableEdges();
			void stop() override;
			void handleMessage(const std::string &) override;
	};
}

#endif
