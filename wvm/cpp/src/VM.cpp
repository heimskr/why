#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "lib/ansi.h"
#include "Operations.h"
#include "Registers.h"
#include "StringSet.h"
#include "Util.h"
#include "VM.h"
#include "VMError.h"

// #define DEBUG_VIRTMEM
#define CATCH_DEBUG
#define CATCH_OPEN

namespace WVM {
	VM::VM(size_t memory_size, bool keep_initial): memorySize(memory_size), keepInitial(keep_initial) {}

	bool VM::getZ() { return (st() & 0b0001) != 0; }
	bool VM::getN() { return (st() & 0b0010) != 0; }
	bool VM::getC() { return (st() & 0b0100) != 0; }
	bool VM::getO() { return (st() & 0b1000) != 0; }
	void VM::setZ(bool on) { st() = (st() & ~0b0001) |  on;       }
	void VM::setN(bool on) { st() = (st() & ~0b0010) | (on << 1); }
	void VM::setC(bool on) { st() = (st() & ~0b0100) | (on << 2); }
	void VM::setO(bool on) { st() = (st() & ~0b1000) | (on << 3); }

	std::chrono::milliseconds VM::getMilliseconds() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch());
	}

	Word VM::translateAddress(Word virtual_address, bool *success, PageMeta *meta_out) {
		if (!pagingOn) {
			if (success)
				*success = true;
			return virtual_address;
		}

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

		P5Entry p5_entry = getWord(p4_entry.getNext() + pieces.p5Offset * sizeof(P04Entry));

		if (success)
			*success = p5_entry.present;

#ifdef DEBUG_VIRTMEM
		if (!p5_entry.present)
			warn() << "virtmem(" << programCounter << ":" << virtual_address << "): !p5.present " << p5_entry << "\n";
#endif

		lastMeta = p5_entry;

		if (meta_out)
			*meta_out = lastMeta;

#ifdef DEBUG_VIRTMEM
		info() << "virtmem(" << programCounter << "): " << virtual_address << " -> "
		       << (p5_entry.getStart() + pieces.pageOffset) << " = " << p5_entry.getStart() << " + "
		       << pieces.pageOffset << "\n";
#endif

		return p5_entry.getStart() + pieces.pageOffset;
	}

	void VM::setWord(Word address, UWord value, Endianness endianness) {
		if (static_cast<Word>(memorySize) <= address - 7 || address < 0) {
			throw VMError("Out-of-bounds memory access in VM::setWord (" + std::to_string(address - 7) + ")");
		}

		if (endianness == Endianness::Little) {
			for (char i = 0; i < 8; i++)
				memory[address + i] = (value >> (8*i)) & 0xff;
		} else {
			for (char i = 0; i < 8; i++)
				memory[address + 7 - i] = (value >> (8*i)) & 0xff;
		}
		onUpdateMemory(address - (address % 8), address, Size::Word);
		if (address % 8 != 0)
			onUpdateMemory(address - (address % 8) + 8, address, Size::Word);
	}

	void VM::setHalfword(Word address, UHWord value, Endianness endianness) {
		if (static_cast<Word>(memorySize) <= address - 3 || address < 0) {
			throw VMError("Out-of-bounds memory access in VM::setHalfword (" + std::to_string(address - 3) + ")");
		}

		if (endianness == Endianness::Little) {
			for (char i = 0; i < 4; i++)
				memory[address + i] = (value >> (8*i)) & 0xff;
		} else {
			for (char i = 0; i < 4; i++)
				memory[address + 3 - i] = (value >> (8*i)) & 0xff;
		}

		onUpdateMemory(address - (address % 8), address, Size::HWord);
		if (4 < address % 8)
			onUpdateMemory(address - (address % 8) + 8, address, Size::HWord);
	}

	void VM::setQuarterword(Word address, UQWord value, Endianness endianness) {
		if (static_cast<Word>(memorySize) <= address - 1 || address < 0) {
			throw VMError("Out-of-bounds memory access in VM::setQuarterword (" + std::to_string(address - 1) + ")");
		}

		if (endianness == Endianness::Little) {
			memory[address] = value & 0xff;
			memory[address + 1] = value >> 8;
		} else {
			memory[address] = value >> 8;
			memory[address + 1] = value & 0xff;
		}

		onUpdateMemory(address - (address % 8), address, Size::QWord);
		if (6 < address % 8)
			onUpdateMemory(address - (address % 8) + 8, address, Size::QWord);
	}

	void VM::setByte(Word address, UByte value) {
		if (static_cast<Word>(memorySize) <= address || address < 0)
			throw VMError("Out-of-bounds memory access in VM::setByte (" + std::to_string(address) + ")");

		memory[address] = value;
		onUpdateMemory(address - (address % 8), address, Size::Byte);
	}

	UWord VM::getWord(Word address, Endianness endianness) const {
		if (static_cast<Word>(memorySize) <= address - 7 || address < 0)
			throw VMError("Out-of-bounds memory access in VM::getWord (" + std::to_string(address - 7) + ")");

		UWord out = 0;
		if (endianness == Endianness::Little)
			for (char i = 0; i < 8; i++)
				out |= static_cast<Word>(memory[address + i]) << (i * 8);
		else
			for (char i = 0; i < 8; i++)
				out |= static_cast<Word>(memory[address + i]) << ((7 - i) * 8);

		return out;
	}

	UHWord VM::getHalfword(Word address, Endianness endianness) const {
		if (static_cast<Word>(memorySize) <= address - 3 || address < 0) {
			throw VMError("Out-of-bounds memory access in VM::getHalfword (" + std::to_string(address - 3) + ")");
		}

		UHWord out = 0;
		if (endianness == Endianness::Little)
			for (char i = 0; i < 4; i++)
				out |= static_cast<HWord>(memory[address + i]) << (8*i);
		else
			for (char i = 0; i < 4; i++)
				out |= static_cast<HWord>(memory[address + 3 - i]) << (8*i);

		return out;
	}

	UQWord VM::getQuarterword(Word address, Endianness endianness) const {
		if (static_cast<Word>(memorySize) <= address - 1 || address < 0)
			throw VMError("Out-of-bounds memory access in VM::getQuarterword (" + std::to_string(address - 1) + ")");

		if (endianness == Endianness::Little)
			return static_cast<UHWord>(memory[address] | (memory[address + 1] << 8));
		return static_cast<UHWord>(memory[address + 1] | (memory[address] << 8));
	}

	UByte VM::getByte(Word address) const {
		if (static_cast<Word>(memorySize) <= address || address < 0)
			throw VMError("Out-of-bounds memory access in VM::getByte (" + std::to_string(address) + ")");

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

	Word VM::getInstruction(Word address) const {
		return getWord(address, Endianness::Big);
	}

	unsigned char VM::registerID(Word &word) const {
		return &word - registers;
	}

	void VM::resize(size_t new_size) {
		memory.resize(new_size);
		memorySize = new_size;
	}

	void VM::jump(Word address, bool should_link) {
		if (should_link)
			link(false);
		recordChange<JumpChange>(*this, address, should_link);
		Word old_address = programCounter;
		programCounter = address;
		onJump(old_address, address);
	}

	void VM::link(bool record) {
		if (record)
			recordChange<RegisterChange>(*this, Why::returnAddressOffset, programCounter + 8);
		registers[Why::returnAddressOffset] = programCounter + 8;
	}

	void VM::increment() {
		recordChange<JumpChange>(*this, programCounter + 8, false);
		programCounter += 8;
		onJump(programCounter - 8, programCounter);
	}

	bool VM::changeRing(Ring new_ring) {
		const Ring old_ring = ring;
		if (static_cast<int>(new_ring) < Why::ringMin || Why::ringMax < static_cast<int>(new_ring) || new_ring < ring) {
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
		Word old_value = st();
		st() = 0;
		if (result == 0)
			setZ(true);
		else if (result < 0)
			setN(true);
		if (old_value != st())
			onRegisterChange(Why::statusOffset);
	}

	bool VM::checkConditions(Conditions conditions) {
		switch (conditions) {
			case Conditions::Positive: return !getN() && !getZ();
			case Conditions::Negative: return getN();
			case Conditions::Zero:     return getZ();
			case Conditions::Nonzero:  return !getZ();
			case Conditions::Disabled: return true;
		}

		throw std::runtime_error("Invalid conditions flag: " + std::to_string(static_cast<int>(conditions)));
	}

	bool VM::interrupt(int type, bool force) {
		return interrupt(static_cast<InterruptType>(type), force);
	}

	bool VM::interrupt(InterruptType type, bool force) {
		if (interrupts.count(type) == 0)
			throw std::runtime_error("Invalid interrupt: " + std::to_string(static_cast<int>(type)));
		interrupts.at(type)(*this, force);
		return true; // Can't remember what the return value is supposed to represent...
	}

	bool VM::checkRing(Ring check) {
		if (static_cast<int>(check) != -1 && static_cast<int>(check) < static_cast<int>(ring)) {
			intProtec();
			return false;
		}

		return true;
	}

	bool VM::intProtec() {
		return interrupt(InterruptType::Protec, true);
	}

	bool VM::intPfault() {
		return interrupt(InterruptType::Pfault, true);
	}

	bool VM::intBwrite() {
		return interrupt(InterruptType::Bwrite, true);
	}

	bool VM::intTimer() {
		return interrupt(InterruptType::Timer, true);
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
		playThread = std::thread([this](size_t microdelay) {
			const std::chrono::microseconds delay(microdelay);
			while (playing && active) {
				tick();
				if (microdelay)
					std::this_thread::sleep_for(delay);
			}
		}, microdelay);
		playThread.detach();
		return true;
	}

	bool VM::pause() {
		return playing.exchange(false);
	}

	bool VM::undo() {
		if (undoPointer == 0)
			return false;

		DBG("size == " << undoStack.size() << ", pointer == " << undoPointer << " (access index: " << (undoPointer - 1) << ")");
		const std::vector<std::unique_ptr<Change>> &changes = undoStack.at(--undoPointer);
		for (auto iter = changes.rbegin(), rend = changes.rend(); iter != rend; ++iter)
			(*iter)->undo(*this, strict);

		return true;
	}

	bool VM::redo() {
		if (undoPointer == undoStack.size())
			return false;

		DBG("size == " << undoStack.size() << ", pointer == " << undoPointer << " (access index: " << undoPointer << ")");
		for (std::unique_ptr<Change> &change: undoStack.at(undoPointer++))
			change->apply(*this, strict);

		return true;
	}

	bool VM::tick() {
		UWord instruction = getWord(programCounter, Endianness::Big);
		Operations::execute(*this, instruction);

		++cycles;

		if (hasBreakpoint(programCounter)) {
			paused = true;
			return false;
		}

		return active;
	}

	Word VM::nextInstructionAddress() const {
		return programCounter + 8;
	}

	bool VM::checkWritable() {
		if (pagingOn && !lastMeta.writable) {
			intProtec();
			return false;
		}

		return true;
	}

	void VM::setTimer(UWord microseconds) {
		timerThread = std::thread([this](UWord microseconds, size_t id) {
			timerStart = getMilliseconds();
			std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
			if (timerThreadID == id) {
				auto lock = lockVM();
				bufferChange<RegisterChange>(*this, REG_E + 2, microseconds);
				intTimer();
			}
		}, microseconds, ++timerThreadID);
		timerThread.detach();
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

	void VM::load(const std::string &path) {
		load(std::filesystem::path(path));
	}

	void VM::load(const std::filesystem::path &path) {
		loadedFrom = path;
		std::ifstream stream;
#ifdef CATCH_OPEN
		try {
#endif
			stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			stream.open(path);
			stream.exceptions(std::ifstream::goodbit);
			load(stream);
			stream.close();
#ifdef CATCH_OPEN
		} catch (std::exception &err) {
			error() << "Couldn't open \e[1m" << path << "\e[22m: " << err.what() << "\n";
			throw;
		}
#endif
	}

	void VM::load(std::istream &stream) {
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
		if (symbolsOffset == -1)
			symbolsOffset = getWord(0, Endianness::Big);
		if (codeOffset == -1)
			codeOffset = programCounter = getWord(8, Endianness::Big);
		if (dataOffset == -1)
			dataOffset = getWord(16, Endianness::Big);
		if (debugOffset == -1)
			debugOffset = getWord(24, Endianness::Big);
		if (endOffset == -1)
			endOffset = getWord(32, Endianness::Big);
		registers[Why::globalAreaPointerOffset] = endOffset;
		sp() = memorySize;
		onRegisterChange(Why::globalAreaPointerOffset);
		onRegisterChange(Why::stackPointerOffset);
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
		for (Word i = symbolsOffset; i < codeOffset && static_cast<size_t>(i + 16) < memorySize;) {
			const HWord hash = getHalfword(i, Endianness::Big);
			const HWord length = getHalfword(i + 4, Endianness::Big);
			const Word location = getWord(i + 8, Endianness::Big);
			if (memorySize <= static_cast<size_t>(i + 16 + length * 8))
				break;
			const std::string name = getString(i + 16, length * 8);
			symbolTable.emplace(name, Symbol(hash, location));
			i += 16 + length * 8;
		}
	}

	void VM::loadDebugData() {
		debugMap.clear();
#ifdef CATCH_DEBUG
		try {
#endif
			int index = 0;
			for (Word i = debugOffset; i < endOffset;) {
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
						address += 8;
					}
					i += 8;
					++index;
				} else throw VMError("Unrecognized debug data entry type: " + std::to_string(type));
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

		if (static_cast<size_t>(undoPointer) < undoStack.size())
			undoStack.erase(std::next(undoStack.begin(), undoPointer), undoStack.end());
		++undoPointer;
		undoStack.emplace_back(std::move(changeBuffer));
		changeBuffer.clear();
	}

	Word & VM::hi() {
		return registers[Why::hiOffset];
	}

	Word & VM::lo() {
		return registers[Why::loOffset];
	}

	Word & VM::st() {
		return registers[Why::statusOffset];
	}

	Word & VM::sp() {
		return registers[Why::stackPointerOffset];
	}

	Word & VM::fp() {
		return registers[Why::framePointerOffset];
	}

	Word & VM::rt() {
		return registers[Why::returnAddressOffset];
	}

	const Word & VM::hi() const {
		return registers[Why::hiOffset];
	}

	const Word & VM::lo() const {
		return registers[Why::loOffset];
	}

	const Word & VM::st() const {
		return registers[Why::statusOffset];
	}

	const Word & VM::sp() const {
		return registers[Why::stackPointerOffset];
	}

	const Word & VM::fp() const {
		return registers[Why::framePointerOffset];
	}

	const Word & VM::rt() const {
		return registers[Why::returnAddressOffset];
	}
}
