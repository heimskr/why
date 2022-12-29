#include <cerrno>
#include <csignal>
#include <cstring>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>

#include <fcntl.h>
#include <unistd.h>

#include "lib/ansi.h"
#include "haunted/core/Util.h"
#include "Operations.h"
#include "Registers.h"
#include "StringSet.h"
#include "Util.h"
#include "VM.h"
#include "VMError.h"
#include "Why.h"

#define DEBUG_VIRTMEM
// #define DEBUG_VIRTMEM_EXTRA
#define CATCH_DEBUG
#define CATCH_OPEN
// #define CATCH_TICK
#define CATCH_TICK_IN_PLAY

namespace WVM {
	VM::VM(size_t memory_size, bool keep_initial): memorySize(memory_size), keepInitial(keep_initial), tlb(TLB_SIZE) {}

	VM::~VM() {
		for (Drive &drive: drives)
			if (!drive.close())
				std::cerr << "Couldn't close " << drive.name << " (" << drive.fd << "): " << strerror(errno) << "\n";
	}

	bool VM::getZ() { return (st().value & 0b0001) != 0; }
	bool VM::getN() { return (st().value & 0b0010) != 0; }
	bool VM::getC() { return (st().value & 0b0100) != 0; }
	bool VM::getO() { return (st().value & 0b1000) != 0; }
	void VM::setZ(bool on) { st().value = (st().value & ~0b0001) |  on;       }
	void VM::setN(bool on) { st().value = (st().value & ~0b0010) | (on << 1); }
	void VM::setC(bool on) { st().value = (st().value & ~0b0100) | (on << 2); }
	void VM::setO(bool on) { st().value = (st().value & ~0b1000) | (on << 3); }

	std::chrono::milliseconds VM::getMilliseconds() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch());
	}

	std::string VM::demangleLabel(const std::string &str) {
		static std::regex why_label_regex("^__(.+)_label\\d+$");
		if (std::regex_match(str, why_label_regex))
			return Haunted::Util::demangle(std::regex_replace(str, why_label_regex, "$1"));
		return Haunted::Util::demangle(str);
	}

	Word VM::translateAddress(Word virtual_address, bool *success) {
		if constexpr (!USE_TLB) {
			return walkTables(virtual_address, success, nullptr);
		}

		if (!pagingOn) {
			if (success != nullptr)
				*success = true;
			return virtual_address;
		}

		Word physical_address = 0;
		bool tlb_success = tlb.translate(*this, virtual_address, physical_address);

		if (success != nullptr)
			*success = tlb_success;

		return physical_address;
	}

	Word VM::walkTables(Word virtual_address, bool *success, PageMeta *meta_out) {
		if (!pagingOn) {
			if (success != nullptr)
				*success = true;
			return virtual_address;
		}

		lastVirtual = virtual_address;

		if (!p0) {
#ifdef DEBUG_VIRTMEM
			warn() << "virtmem(" << programCounter << ":" << virtual_address << "): !p0\n";
#endif
			if (success)
				*success = false;
			return 0;
		}

		Address pieces = virtual_address;

		P04Entry p0_entry = getWord(p0 + pieces.p0Offset * sizeof(P04Entry));
		if (!p0_entry.present) {
#ifdef DEBUG_VIRTMEM
			warn() << "virtmem(" << programCounter << ":" << virtual_address << "): !p0.present " << p0_entry << "\n";
#endif
			if (success)
				*success = false;
			return 0;
		}

		P04Entry p1_entry = getWord(p0_entry.getNext() + pieces.p1Offset * sizeof(P04Entry));
		if (!p1_entry.present) {
#ifdef DEBUG_VIRTMEM
			warn() << "virtmem(" << programCounter << ":" << virtual_address << "): !p1.present " << p1_entry << "\n";
#endif
			if (success)
				*success = false;
			return 0;
		}

		P04Entry p2_entry = getWord(p1_entry.getNext() + pieces.p2Offset * sizeof(P04Entry));
		if (!p2_entry.present) {
#ifdef DEBUG_VIRTMEM
			warn() << "virtmem(" << programCounter << ":" << virtual_address << "): !p2.present " << p2_entry << "\n";
#endif
			if (success)
				*success = false;
			return 0;
		}

		P04Entry p3_entry = getWord(p2_entry.getNext() + pieces.p3Offset * sizeof(P04Entry));
		if (!p3_entry.present) {
#ifdef DEBUG_VIRTMEM
			warn() << "virtmem(" << programCounter << ":" << virtual_address << "): !p3.present " << p3_entry << "\n";
#endif
			if (success)
				*success = false;
			return 0;
		}

		P04Entry p4_entry = getWord(p3_entry.getNext() + pieces.p4Offset * sizeof(P04Entry));
		if (!p4_entry.present) {
#ifdef DEBUG_VIRTMEM
			warn() << "virtmem(" << programCounter << ":" << virtual_address << "): !p4.present " << p4_entry << "\n";
#endif
			if (success)
				*success = false;
			return 0;
		}

		P5Entry p5_entry = getWord(p4_entry.getNext() + pieces.p5Offset * sizeof(P5Entry));

		if (success)
			*success = p5_entry.present;

#ifdef DEBUG_VIRTMEM
		if (!p5_entry.present) {
			warn() << "virtmem(" << programCounter << ":" << virtual_address << "): !p5[" << pieces.p5Offset
			       << "].present " << p5_entry << "\n";
			for (int i = 0; i < 256; ++i) {
				P5Entry p5e_ = getWord(p4_entry.getNext() + i * sizeof(P5Entry));
				if (p5e_.pageStart != 0)
					std::cerr << i << ": " << p5e_ << '\n';
			}
		}
#endif

		lastMeta = p5_entry;

		if (meta_out)
			*meta_out = lastMeta;

#ifdef DEBUG_VIRTMEM
#ifdef DEBUG_VIRTMEM_EXTRA
		info() << "virtmem(" << programCounter << "): " << virtual_address << " -> "
		       << (p5_entry.getStart() + pieces.pageOffset) << " = " << p5_entry.getStart() << " + "
		       << pieces.pageOffset << "\n";
#endif
#endif

		return p5_entry.getStart() + pieces.pageOffset;
	}

	void VM::setWord(Word address, UWord value, Endianness endianness) {
		if (Word(memorySize) <= address - 7 || address < 0)
			throw VMError("Out-of-bounds memory access in VM::setWord (" + std::to_string(address - 7) + ") at " +
				std::to_string(programCounter));

		if (endianness == Endianness::Little)
			for (char i = 0; i < 8; ++i)
				memory[address + i] = (value >> (8*i)) & 0xff;
		else
			for (char i = 0; i < 8; ++i)
				memory[address + 7 - i] = (value >> (8*i)) & 0xff;
		onUpdateMemory(programCounter, address - (address % 8), address, Size::Word);
		if (address % 8 != 0)
			onUpdateMemory(programCounter, address - (address % 8) + 8, address, Size::Word);
	}

	void VM::setHalfword(Word address, UHWord value, Endianness endianness) {
		if (Word(memorySize) <= address - 3 || address < 0)
			throw VMError("Out-of-bounds memory access in VM::setHalfword (" + std::to_string(address - 3) + ") at " +
				std::to_string(programCounter));

		if (endianness == Endianness::Little) {
			for (char i = 0; i < 4; ++i)
				memory[address + i] = (value >> (8*i)) & 0xff;
		} else {
			for (char i = 0; i < 4; ++i)
				memory[address + 3 - i] = (value >> (8*i)) & 0xff;
		}

		onUpdateMemory(programCounter, address - (address % 8), address, Size::HWord);
		if (4 < address % 8)
			onUpdateMemory(programCounter, address - (address % 8) + 8, address, Size::HWord);
	}

	void VM::setQuarterword(Word address, UQWord value, Endianness endianness) {
		if (Word(memorySize) <= address - 1 || address < 0)
			throw VMError("Out-of-bounds memory access in VM::setQuarterword (" + std::to_string(address - 1) + ") at "
				+ std::to_string(programCounter));

		if (endianness == Endianness::Little) {
			memory[address] = value & 0xff;
			memory[address + 1] = value >> 8;
		} else {
			memory[address] = value >> 8;
			memory[address + 1] = value & 0xff;
		}

		onUpdateMemory(programCounter, address - (address % 8), address, Size::QWord);
		if (6 < address % 8)
			onUpdateMemory(programCounter, address - (address % 8) + 8, address, Size::QWord);
	}

	void VM::setByte(Word address, UByte value) {
		if (Word(memorySize) <= address || address < 0)
			throw VMError("Out-of-bounds memory access in VM::setByte (" + std::to_string(address) + ") at " +
				std::to_string(programCounter));

		memory[address] = value;
		onUpdateMemory(programCounter, address - (address % 8), address, Size::Byte);
	}

	Wasmc::TypedInstruction VM::getInstruction(Word address) const {
		Wasmc::TypedInstruction out;

		for (char i = 0; i < 8; ++i)
			out.instruction |= UWord(memory[address + i]) << (i * 8);

		for (char i = 0; i < 4; ++i)
			out.typeInfo |= UWord(memory[address + 8 + i]) << (i * 8);

		return out;
	}

	UWord VM::getWord(Word address, Endianness endianness) const {
		if (Word(memorySize) <= address - 7 || address < 0)
			throw VMError("Out-of-bounds memory access in VM::getWord (" + std::to_string(address - 7) + ") at " +
				std::to_string(programCounter));

		UWord out = 0;
		if (endianness == Endianness::Little)
			for (char i = 0; i < 8; ++i)
				out |= Word(memory[address + i]) << (i * 8);
		else
			for (char i = 0; i < 8; ++i)
				out |= Word(memory[address + i]) << ((7 - i) * 8);

		return out;
	}

	UHWord VM::getHalfword(Word address, Endianness endianness) const {
		if (Word(memorySize) <= address - 3 || address < 0)
			throw VMError("Out-of-bounds memory access in VM::getHalfword (" + std::to_string(address - 3) + ") at " +
				std::to_string(programCounter));

		UHWord out = 0;
		if (endianness == Endianness::Little)
			for (char i = 0; i < 4; ++i)
				out |= HWord(memory[address + i]) << (8*i);
		else
			for (char i = 0; i < 4; ++i)
				out |= HWord(memory[address + 3 - i]) << (8*i);

		return out;
	}

	UQWord VM::getQuarterword(Word address, Endianness endianness) const {
		if (Word(memorySize) <= address - 1 || address < 0)
			throw VMError("Out-of-bounds memory access in VM::getQuarterword (" + std::to_string(address - 1) + ") at "
				+ std::to_string(programCounter));

		if (endianness == Endianness::Little)
			return UHWord(memory[address] | (memory[address + 1] << 8));
		return UHWord(memory[address + 1] | (memory[address] << 8));
	}

	UByte VM::getByte(Word address) const {
		if (Word(memorySize) <= address || address < 0)
			throw VMError("Out-of-bounds memory access in VM::getByte (" + std::to_string(address) + ") at " +
				std::to_string(programCounter));

		return memory[address];
	}

	UWord VM::get(Word address, Size size, Endianness endianness) const {
		switch (size) {
			case Size::Byte:  return getByte(address);
			case Size::QWord: return getQuarterword(address, endianness);
			case Size::HWord: return getHalfword(address, endianness);
			case Size::Word:  return getWord(address, endianness);
			default: throw std::runtime_error("Invalid size for VM::get");
		}
	}

	void VM::set(Word address, Word value, Size size, Endianness endianness) {
		switch (size) {
			case Size::Byte:  return setByte(address, value);
			case Size::QWord: return setQuarterword(address, value, endianness);
			case Size::HWord: return setHalfword(address, value, endianness);
			case Size::Word:  return setWord(address, value, endianness);
			default: throw std::runtime_error("Invalid size for VM::set");
		}
	}

	std::string VM::getString(Word address, int max) const {
		std::string out;
		out.reserve(32);
		int j = 0;
		for (UWord i = address; i < memorySize; ++i) {
			if (j++ == max || memory[i] == '\0')
				break;
			out.push_back(memory[i]);
		}
		return out;
	}

	unsigned char VM::registerID(const Register &reg) const {
		return &reg - registers.begin();
	}

	void VM::resize(size_t new_size) {
		memory.resize(new_size);
		memorySize = new_size;
	}

	void VM::jump(Word address, bool should_link, bool from_rt) {
		if (should_link) {
			link(false);
			if (logJumps) {
				auto [begin, end] = symbolsByPosition.equal_range(address);
				if (begin != end) {
					std::cerr << "\e[32mJumping\e[39m to " << begin->second << '\n';
					jumpStack.push_back(&begin->second);
				}
			}
		}

		if (logJumps && from_rt && !jumpStack.empty()) {
			std::cerr << "\e[31mLeaving\e[39m " << *jumpStack.back() << '\n';
			jumpStack.pop_back();
		}

		recordChange<JumpChange>(*this, address, should_link);
		Word old_address = programCounter;
		programCounter = address;
		onJump(old_address, address);
	}

	void VM::link(bool record) {
		if (record)
			recordChange<RegisterChange>(*this, Why::returnAddressOffset, programCounter + Why::instructionSize,
				OperandType::VOID_PTR);
		registers[Why::returnAddressOffset] = {programCounter + Why::instructionSize, OperandType::VOID_PTR};
	}

	void VM::increment() {
		recordChange<JumpChange>(*this, programCounter + Why::instructionSize, false);
		programCounter += Why::instructionSize;
		onJump(programCounter - Why::instructionSize, programCounter);
	}

	bool VM::changeRing(Ring new_ring) {
		const Ring old_ring = ring;
		if (int(new_ring) < Why::ringMin || Why::ringMax < int(new_ring) || new_ring < ring) {
			intProtec();
			return false;
		}

		bufferChange<RingChange>(old_ring, new_ring);
		ring = new_ring;
		if (old_ring != new_ring)
			onRingChange(old_ring, new_ring);
		return true;
	}

	void VM::updateFlags(Word result) {
		Word old_value = st().value;
		st().value = 0;
		if (result == 0)
			setZ(true);
		else if (result < 0)
			setN(true);
		if (old_value != st().value)
			onRegisterChange(Why::statusOffset);
	}

	void VM::updateFlags(const Register &reg) {
		updateFlags(reg.value);
	}

	bool VM::checkConditions(Conditions conditions) {
		switch (conditions) {
			case Conditions::Positive: return !getN() && !getZ();
			case Conditions::Negative: return getN();
			case Conditions::Zero:     return getZ();
			case Conditions::Nonzero:  return !getZ();
			case Conditions::Disabled: return true;
		}

		throw std::runtime_error("Invalid conditions flag: " + std::to_string(int(conditions)));
	}

	bool VM::interrupt(int type, bool force) {
		return interrupt(InterruptType(type), force);
	}

	bool VM::interrupt(InterruptType type, bool force) {
		if (interrupts.count(type) == 0)
			throw std::runtime_error("Invalid interrupt: " + std::to_string(int(type)));

		const Interrupt &in_map = interrupts.at(type);
		if (!in_map.canDisable || hardwareInterruptsEnabled) {
			// Disable hardware interrupts if jumping to one.
			if (in_map.canDisable)
				hardwareInterruptsEnabled = false;
			pagingStack.emplace_back(*this);
			pagingOn = false;
			// TODO: Add an instruction to set a "kernel P0" that's stored in a separate field in the VM and set p0 to
			// its value here.
			in_map(*this, force);
			wakeRest();
			return true;
		}

		return false;
	}

	bool VM::checkRing(Ring check) {
		if (int(check) != -1 && int(check) < int(ring)) {
			intProtec();
			return false;
		}

		return true;
	}

	bool VM::intProtec() {
		return interrupt(InterruptType::Protec, true);
	}

	bool VM::intPfault() {
		auto lock = lockVM();
		bufferChange<RegisterChange>(*this, Why::exceptionOffset + 2, lastVirtual, OperandType::VOID_PTR);
		registers[Why::exceptionOffset + 2] = {lastVirtual, OperandType::VOID_PTR};
		onRegisterChange(Why::exceptionOffset + 2);
		return interrupt(InterruptType::Pfault, true);
	}

	bool VM::intBwrite(Word address) {
		auto lock = lockVM();
		bufferChange<RegisterChange>(*this, Why::exceptionOffset + 2, address, OperandType::VOID_PTR);
		registers[Why::exceptionOffset + 2] = {address, OperandType::VOID_PTR};
		onRegisterChange(Why::exceptionOffset + 2);
		return interrupt(InterruptType::Bwrite, true);
	}

	bool VM::intTimer() {
		return interrupt(InterruptType::Timer, true);
	}

	bool VM::intKeybrd(UWord key) {
		static size_t stamp = Util::nanotime();

		const size_t now = Util::nanotime();
		const double diff = double(now - stamp) / 1e3;
		stamp = now;

		// For maximum safety, I could lock the VM before reading hardwareInterruptsEnabled, but the chance of a race
		// condition if I don't do it is small enough that it shouldn't matter.
		if (hardwareInterruptsEnabled) {
			auto lock = lockVM();
			bufferChange<RegisterChange>(*this, Why::exceptionOffset + 2, key, OperandType::ULONG);
			registers[Why::exceptionOffset + 2] = {static_cast<Word>(key), OperandType::ULONG};
			onRegisterChange(Why::exceptionOffset + 2);
			return interrupt(InterruptType::Keybrd, true);
		}

		std::cerr << "Skipping keyboard input (" << diff << " μs)\n";
		return false;
	}

	bool VM::intBadtyp() {
		error() << "Typechecking failed at " << programCounter << '\n';
		return interrupt(InterruptType::Badtyp, true);
	}

	void VM::start() {
		active = true;
	}

	void VM::stop() {
		active = false;
	}

	bool VM::play(size_t microdelay) {
		if (playing.exchange(true))
			return false;
		paused = false;
		start();
		playThread = std::thread([this](size_t microdelay) {
			if (playing && active && !paused) {
				const std::chrono::microseconds delay(microdelay);
				onPlayStart();
				playThreadAlive = true;
				do {
					if (resting.load()) {
						std::unique_lock<std::mutex> lock(restMutex);
						restCondition.wait(lock, [this] { return !resting.load(); });
						restAcknowledged.store(true);
						restAcknowledgeCondition.notify_all();
					}
#ifdef CATCH_TICK_IN_PLAY
					try {
						tick();
					} catch (const std::exception &err) {
						std::cerr << "Play thread caught an exception";
						if (!jumpStack.empty()) {
							const std::string demangled = demangleLabel(*jumpStack.back());
							std::cerr << " in \e[31m" << *jumpStack.back() << "\e[39m";
							if (*jumpStack.back() != demangled)
								std::cerr << " (\e[33;1m" << demangled << "\e[22;39m)";
						}
						std::cerr << ": " << err.what() << std::endl;
						size_t i = 0;
						for (const std::string *label: jumpStack)
							std::cerr << i++ << ": " << *label << '\n';
						break;
					}
#else
					tick();
#endif
					if (microdelay)
						std::this_thread::sleep_for(delay);
				} while (playing && active && !paused);
				playThreadAlive = false;
				onPlayEnd();
			}
			playing = false;
		}, microdelay);
		playThread.detach();
		return true;
	}

	bool VM::pause() {
		return playing.exchange(false);
	}

	void VM::wakeRest() {
		if (!resting.load())
			return;

		restAcknowledged = false;
		{
			std::unique_lock<std::mutex> lock(restMutex);
			resting = false;
			restCondition.notify_all();
		}
		if (playThreadAlive.load()) {
			std::unique_lock<std::mutex> lock(restAcknowledgeMutex);
			restAcknowledgeCondition.wait(lock, [this] { return restAcknowledged.load(); });
		}
	}

	void VM::rest() {
		// std::unique_lock<std::mutex> lock(restMutex);
		resting = true;
	}

	bool VM::undo() {
		if (undoPointer == 0)
			return false;

		const std::vector<std::unique_ptr<Change>> &changes = undoStack.at(--undoPointer);
		for (auto iter = changes.rbegin(), rend = changes.rend(); iter != rend; ++iter)
			(*iter)->undo(*this, strict);

		return true;
	}

	bool VM::redo() {
		if (undoPointer == undoStack.size())
			return false;

		for (std::unique_ptr<Change> &change: undoStack.at(undoPointer++))
			change->apply(*this, strict);

		return true;
	}

	bool VM::tick() {
		auto lock = lockVM();
		bool success = false;
		Word translated = translateAddress(programCounter, &success);
		if (!success) {
			std::cerr << "Failed to translate " << programCounter << "\n";
			// intPfault();
			recordChange<HaltChange>();
			stop();
			return false;
		}

		auto typed = getInstruction(translated);
#ifdef CATCH_TICK
		try {
#endif
			Operations::execute(*this, typed);
#ifdef CATCH_TICK
		} catch (const std::exception &err) {
			error() << "Error while ticking: " << err.what() << '\n';
			paused = true;
			return false;
		}
#endif

		++cycles;

		if (hasBreakpoint(programCounter)) {
			paused = true;
			return false;
		}

		return active;
	}

	Word VM::nextInstructionAddress() const {
		return programCounter + Why::instructionSize;
	}

	bool VM::checkWritable() {
		return !pagingOn || lastMeta.writable;
	}

	void VM::setTimer(UWord microseconds) {
		timerTicks = microseconds;
		if (!timerActive) {
			timerActive = true;
			timerThread = std::thread([this] {
				while (timerActive) {
					if (0 < timerTicks && --timerTicks == 0) {
						if (active) {
							auto lock = lockVM();
							intTimer();
						}

						timerActive = false;
						return;
					}
					std::this_thread::sleep_for(std::chrono::microseconds(1));
				}
			});
			timerThread.detach();
		}
	}

	void VM::addBreakpoint(Word breakpoint) {
		breakpoints.insert(breakpoint);
		onAddBreakpoint(breakpoint);
	}

	void VM::removeBreakpoint(Word breakpoint) {
		breakpoints.erase(breakpoint);
		onRemoveBreakpoint(breakpoint);
	}

	const std::unordered_set<Word> & VM::getBreakpoints() const {
		return breakpoints;
	}

	bool VM::hasBreakpoint(Word breakpoint) const {
		return 0 < breakpoints.count(breakpoint);
	}

	void VM::load(const std::string &path, const std::vector<std::string> &disks) {
		load(std::filesystem::path(path), disks);
	}

	void VM::load(const std::filesystem::path &path, const std::vector<std::string> &disks) {
		loadedFrom = path;
		std::ifstream stream;
#ifdef CATCH_OPEN
		try {
#endif
			stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			stream.open(path);
			stream.exceptions(std::ifstream::goodbit);
			load(stream, disks);
			stream.close();
#ifdef CATCH_OPEN
		} catch (std::exception &err) {
			error() << "Couldn't open \e[1m" << path << "\e[22m: " << err.what() << "\n";
			throw;
		}
#endif
	}

	void VM::load(std::istream &stream, const std::vector<std::string> &disks) {
		for (const std::string &disk: disks) {
			const int fd = open(disk.c_str(), O_RDWR);
			if (fd == -1)
				std::cerr << "Couldn't open " << disk << ": " << strerror(errno) << "\n";
			else
				drives.emplace_back(disk, fd);
		}

		std::string line;
		int lineno = 0;
		memory.clear();
		memory.resize(memorySize);
		while (std::getline(stream, line)) {
			++lineno;
			char *endptr;
			UWord word = strtoul(line.c_str(), &endptr, 16);
			if (line.size() != 16 || endptr - line.c_str() != 16)
				throw std::runtime_error("Invalid line (" + std::to_string(lineno) + ")");
			setWord(8 * (lineno - 1), word, Endianness::Big);
		}

		if (keepInitial)
			initial = memory;

		init();
	}

	void VM::init() {
		if (codeOffset == -1)
			codeOffset = programCounter = getWord(0, Endianness::Little);
		if (dataOffset == -1)
			dataOffset = getWord(8, Endianness::Little);
		if (symbolsOffset == -1)
			symbolsOffset = getWord(16, Endianness::Little);
		if (debugOffset == -1)
			debugOffset = getWord(24, Endianness::Little);
		if (relocationOffset == -1)
			relocationOffset = getWord(32, Endianness::Little);
		if (endOffset == -1)
			endOffset = getWord(40, Endianness::Little);

		Word index = 0;
		for (Register &reg: registers) {
			reg = {0, OperandType::VOID};
			reg.index = index++;
		}

		registers[Why::globalAreaPointerOffset] = {endOffset, OperandType::VOID_PTR};
		sp() = {static_cast<Word>(memorySize), OperandType::VOID_PTR};
		rt().type = OperandType::VOID_PTR;

		onRegisterChange(Why::globalAreaPointerOffset);
		onRegisterChange(Why::stackPointerOffset);
		onRegisterChange(Why::returnAddressOffset);
		loadSymbols();
		loadDebugData();
	}

	void VM::reset(bool reload) {
		if (reload) {
			if (!loadedFrom.empty())
				load(loadedFrom);
			else
				throw std::runtime_error("Unable to reset VM: path was stored");
		} else {
			if (keepInitial)
				memory = initial;
			else if (!loadedFrom.empty())
				load(loadedFrom);
			else
				throw std::runtime_error("Unable to reset VM: no initial memory or path was stored");
		}
		init();
	}

	void VM::loadSymbols() {
		symbolTable.clear();
		for (Word i = symbolsOffset; i < debugOffset && size_t(i + 16) < memorySize;) {
			const QWord length = getQuarterword(i, Endianness::Little);
			const QWord type = getQuarterword(i + 2, Endianness::Little);
			const HWord hash = getHalfword(i + 4, Endianness::Little);
			const Word location = getWord(i + 8, Endianness::Little);
			if (memorySize <= size_t(i + 16 + length * 8))
				break;
			const std::string name = getString(i + 16, length * 8);
			symbolTable.emplace(name, Symbol(hash, location, SymbolEnum(type)));
			symbolsByPosition.emplace(location, name);
			i += 16 + length * 8;
		}
	}

	void VM::loadDebugData() {
		debugMap.clear();
#ifdef CATCH_DEBUG
		try {
#endif
			int index = 0;
			for (Word i = debugOffset; i < relocationOffset;) {
				Word word = getWord(i, Endianness::Little);
				const uint8_t type = word & 0xff;
				if (type == 1 || type == 2) {
					const unsigned length = (word >> 8) & 0xffffff;
					std::string str;
					str.reserve(length);
					for (unsigned j = 0; j < length; ++j)
						str += getByte(i + 4 + j);
					if (type == 1)
						debugFiles.emplace(index++, StringSet::intern(str));
					else
						debugFunctions.emplace(index++, StringSet::intern(str));
					i += 8 + (length <= 4? 0 : Util::upalign(length - 4, 8));
				} else if (type == 3) {
					const unsigned file = (word >> 8) & 0xffffff;
					const unsigned line = word >> 32;
					word = getWord(i += 8, Endianness::Little);
					const unsigned column = word & 0xffffff;
					const unsigned char count = word >> 24;
					const unsigned function = word >> 32;
					Word address = getWord(i += 8, Endianness::Little);
					for (unsigned j = 0; j < count; ++j) {
						debugMap.try_emplace(address, debugFiles.at(file), debugFunctions.at(function), line, column,
							count, address);
						address += Why::instructionSize;
					}
					i += 8;
					++index;
				} else
					throw VMError("Unrecognized debug data entry type: " + std::to_string(type));
			}
#ifdef CATCH_DEBUG
		} catch (std::exception &err) {
			debugMap.clear();
			warn() << "Failed to load debug data: " << err.what() << '\n';
			throw;
		}
#endif
	}

	void VM::finishChange() {
		if (!enableHistory || changeBuffer.empty())
			return;

		if (size_t(undoPointer) < undoStack.size())
			undoStack.erase(std::next(undoStack.begin(), undoPointer), undoStack.end());
		++undoPointer;
		undoStack.emplace_back(std::move(changeBuffer));
		changeBuffer.clear();
	}

	void VM::mathError() {
		recordChange<HaltChange>();
		stop();
	}

	Register & VM::hi() {
		return registers[Why::hiOffset];
	}

	Register & VM::lo() {
		return registers[Why::loOffset];
	}

	Register & VM::st() {
		return registers[Why::statusOffset];
	}

	Register & VM::sp() {
		return registers[Why::stackPointerOffset];
	}

	Register & VM::fp() {
		return registers[Why::framePointerOffset];
	}

	Register & VM::rt() {
		return registers[Why::returnAddressOffset];
	}

	const Register & VM::hi() const {
		return registers[Why::hiOffset];
	}

	const Register & VM::lo() const {
		return registers[Why::loOffset];
	}

	const Register & VM::st() const {
		return registers[Why::statusOffset];
	}

	const Register & VM::sp() const {
		return registers[Why::stackPointerOffset];
	}

	const Register & VM::fp() const {
		return registers[Why::framePointerOffset];
	}

	const Register & VM::rt() const {
		return registers[Why::returnAddressOffset];
	}
}
