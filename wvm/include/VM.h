#pragma once

#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <functional>
#include <istream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_set>
#include <vector>

#include "Changes.h"
#include "DebugData.h"
#include "Defs.h"
#include "Interrupts.h"
#include "Paging.h"
#include "Symbol.h"
#include "Why.h"

namespace WVM {
	struct Drive {
		const std::string name;
		const int fd;
		Drive(const std::string &name_, int fd_): name(name_), fd(fd_) {}
	};

	class VM {
		private:
			std::vector<UByte> initial;
			std::filesystem::path loadedFrom;
			size_t memorySize;
			bool keepInitial;
			std::atomic<bool> active = false;
			size_t cycles = 0;
			std::unordered_set<Word> breakpoints;
			std::vector<std::vector<std::unique_ptr<Change>>> undoStack;
			std::vector<std::unique_ptr<Change>> changeBuffer;
			size_t undoPointer = 0;
			PageMeta lastMeta;
			std::recursive_mutex mutex;
			std::atomic<size_t> timerThreadID = 0;
			std::thread timerThread;
			std::chrono::milliseconds timerStart;
			std::thread playThread;
			std::atomic<bool> playing = false;
			std::mutex restMutex;
			std::condition_variable restCondition;

			bool getZ();
			bool getN();
			bool getC();
			bool getO();
			void setZ(bool);
			void setN(bool);
			void setC(bool);
			void setO(bool);
			static std::chrono::milliseconds getMilliseconds();
			static std::string demangleLabel(const std::string &str);

		public:
			static constexpr size_t PAGE_SIZE = 65536;

			std::vector<UByte> memory;
			Ring ring = Ring::Zero;
			Word programCounter = -1;
			Word interruptTableAddress = 0;
			Word registers[Why::totalRegisters];
			std::map<std::string, Symbol> symbolTable;
			std::multimap<Word, std::string> symbolsByPosition;
			std::map<Word, DebugData> debugMap;
			std::map<int, const std::string *> debugFiles, debugFunctions;
			std::vector<Drive> drives;
			Word symbolsOffset = -1;
			Word    codeOffset = -1;
			Word    dataOffset = -1;
			Word   debugOffset = -1;
			Word     endOffset = -1;
			Word p0 = 0;
			std::atomic_bool paused = false;
			bool strict = false;
			bool pagingOn = false;
			bool enableHistory = false;
			std::atomic_bool resting = false;
			bool hardwareInterruptsEnabled = true;
			bool logJumps = false;
			std::list<const std::string *> jumpStack;

			std::function<void(unsigned char)> onRegisterChange = [](unsigned char) {};
			std::function<void(Ring, Ring)> onRingChange = [](Ring, Ring) {};
			std::function<void()> onInterruptTableChange = [] {};
			/** PC, address, unadjusted, size */
			std::function<void(Word, Word, Word, Size)> onUpdateMemory = [](Word, Word, Word, Size) {};
			std::function<void(Word, Word)> onJump = [](Word, Word) {};
			std::function<void(const std::string &)> onPrint = [](const std::string &) {};
			std::function<void(Word)> onAddBreakpoint = [](Word) {};
			std::function<void(Word)> onRemoveBreakpoint = [](Word) {};
			std::function<void(bool)> onPagingChange = [](bool) {};
			std::function<void(Word)> onP0Change = [](Word) {};
			std::function<void()> onPlayStart = [] {}, onPlayEnd = [] {};

			VM(size_t memory_size, bool keep_initial = true);
			~VM();

			Word translateAddress(Word virtual_address, bool *success = nullptr, PageMeta *meta_out = nullptr);

			void setWord(Word address, UWord value, Endianness = Endianness::Little);
			void setHalfword(Word address, UHWord value, Endianness = Endianness::Little);
			void setQuarterword(Word address, UQWord value, Endianness = Endianness::Little);
			void setByte(Word address, UByte value);
			UWord getWord(Word address, Endianness = Endianness::Little) const;
			UHWord getHalfword(Word address, Endianness = Endianness::Little) const;
			UQWord getQuarterword(Word address, Endianness = Endianness::Little) const;
			UByte getByte(Word address) const;
			UWord get(Word address, Size, Endianness = Endianness::Little) const;
			void set(Word address, Word value, Size, Endianness = Endianness::Little);
			std::string getString(Word address, int max = -1) const;
			Word getInstruction(Word address) const;
			unsigned char registerID(Word &) const;
			void resize(size_t);

			void jump(Word, bool should_link = false, bool from_rt = false);
			void link(bool record = false);
			void increment();
			bool changeRing(Ring);
			void updateFlags(Word);
			bool checkConditions(Conditions);
			bool interrupt(int, bool force);
			bool interrupt(InterruptType, bool force);
			bool checkRing(Ring);
			bool intProtec();
			bool intPfault();
			bool intBwrite();
			bool intTimer();
			bool intKeybrd(UWord);
			void start();
			void stop();
			bool play(size_t microdelay = 0);
			bool pause();
			void wakeRest();
			void rest();
			bool undo();
			bool redo();
			bool getActive() const { return active; }
			bool tick();
			Word nextInstructionAddress() const;
			bool checkWritable();
			void setTimer(UWord microseconds);

			void addBreakpoint(Word);
			void removeBreakpoint(Word);
			const std::unordered_set<Word> & getBreakpoints() const;
			bool hasBreakpoint(Word) const;

			void load(const std::string &, const std::vector<std::string> &disks = {});
			void load(const std::filesystem::path &, const std::vector<std::string> &disks = {});
			void load(std::istream &, const std::vector<std::string> &disks = {});
			void init();
			void reset(bool reload = false);
			void loadSymbols();
			void loadDebugData();

			size_t getMemorySize() { return memorySize; }
			std::unique_lock<std::recursive_mutex> lockVM() { return std::unique_lock(mutex); }

			void finishChange();

			template <typename T, typename... Args>
			void recordChange(Args && ...args) {
				if (enableHistory) {
					auto lock = lockVM();
					changeBuffer.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
					finishChange();
				}
			}

			template <typename T, typename... Args>
			void bufferChange(Args && ...args) {
				if (enableHistory) {
					auto lock = lockVM();
					changeBuffer.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
				}
			}

			Word & hi();
			Word & lo();
			Word & st();
			Word & sp();
			Word & fp();
			Word & rt();
			const Word & hi() const;
			const Word & lo() const;
			const Word & st() const;
			const Word & sp() const;
			const Word & fp() const;
			const Word & rt() const;
	};
}
