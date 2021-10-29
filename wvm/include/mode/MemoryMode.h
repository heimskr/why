#ifndef WVM_MODE_MEMORYMODE_H_
#define WVM_MODE_MEMORYMODE_H_

#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <optional>
#include <thread>
#include <unordered_set>

#include "haunted/core/Terminal.h"
#include "haunted/ui/boxes/ExpandoBox.h"
#include "haunted/ui/Textbox.h"
#include "haunted/ui/TextInput.h"
#include "mode/ClientMode.h"
#include "VM.h"

namespace WVM::Mode {
	class MemoryMode: public ClientMode {
		template <typename T>
		using Container = std::vector<T>;

		private:
			std::atomic_bool alive = true;
			Haunted::Terminal terminal;
			Haunted::UI::Boxes::ExpandoBox *expando;
			Haunted::UI::Textbox<Container> *textbox;
			Haunted::UI::TextInput *textinput;
			VM vm;
			std::map<int, std::shared_ptr<Haunted::UI::TextLine<Container>>> lines;
			std::unordered_set<Word> symbolTableEdges;
			bool follow = true, fastForward = false, showSymbols = false, searching = false;

			int autotick = -50'000;
			std::thread autotickThread;
			bool autotickReady = false;
			std::mutex autotickMutex;
			std::condition_variable autotickVariable;

			std::thread networkThread;
			std::mutex networkMutex;

			void startAutotick();
			void send(const std::string &);
			void jumpToPC();
			Haunted::UI::SimpleLine<Container> & getLine(Word address);
			Haunted::UI::SimpleLine<Container> & addLine(Word address);
			void addBreakpoint(Word);
			void removeBreakpoint(Word);

		public:
			Word min = 0, max = 0;
			int padding = 6;

			MemoryMode(): ClientMode(), vm(0) {}
			~MemoryMode();

			void run(const std::string &hostname, int port) override;
			void remakeList(const std::function<void()> on_complete = [] {});
			std::string stringify(Word address) const;
			void updateLine(Word address, bool careless = false);
			void makeSymbolTableEdges();
			void stop() override;
			void setFastForward(bool);
			void handleMessage(const std::string &) override;
			void toggleSearchbox();
	};
}

#endif
