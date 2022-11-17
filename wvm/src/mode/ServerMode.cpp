#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <signal.h>

#include "lib/ansi.h"
#include "mode/ServerMode.h"
#include "Unparser.h"
#include "Util.h"
#include "VMError.h"

#define CATCH_TICK

void sigint_handler(int) {
	if (WVM::Mode::ServerMode::instance)
		WVM::Mode::ServerMode::instance->stop();
}

namespace WVM::Mode {
	ServerMode * ServerMode::instance = nullptr;

	void ServerMode::run(const std::string &path, const std::vector<std::string> &disks) {
		instance = this;
		server.messageHandler = [&](int client, const std::string &message) { handleMessage(client, message); };
		ansi::out << ansi::info << "ServerMode is running on port " << ansi::style::bold << server.getPort()
		          >> ansi::style::bold << ".\n";
		std::ofstream port_stream;
		port_stream.open(".port");
		if (port_stream) {
			port_stream << server.getPort();
			port_stream.close();
		}
		vm.load(path, disks);
		initVM();
		signal(SIGINT, sigint_handler);
		server.onEnd = [this](int client, int) { cleanupClient(client); };
		keyThread = std::thread([this] {
			while (readingKeys) {
				{
					auto lock = lockKeys();
					if (!keys.empty()) {
						const UWord key = keys.front();
						keys.pop_front();
						vm.intKeybrd(key);
					}
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		});
		server.run();
		readingKeys = false;
		keyThread.join();
	}

	void ServerMode::initVM() {
		vm.onUpdateMemory = [this](Word pc, Word address, Word unadjusted, Size size) {
			if (logMemoryWrites)
				DBG("[" << unadjusted << "] <- " << vm.get(unadjusted, size));
			if (memorySubscribers.empty())
				return;
			writtenAddresses.insert(address);
			std::string message = ":MemoryWord " + std::to_string(address) + " " +
				std::to_string(static_cast<Word>(vm.getWord(address))) + " " + std::to_string(pc) + " ";
			switch (size) {
				case Size::Word:  message += "8B"; break;
				case Size::HWord: message += "4B"; break;
				case Size::QWord: message += "2B"; break;
				case Size::Byte:  message += "1B"; break;
				default: message += "?"; break;
			}
			auto lock = lockSubscribers();
			for (int client: memorySubscribers)
				server.send(client, message);
		};

		vm.onRegisterChange = [this](unsigned char id) {
			if (logRegisters)
				DBG(Why::coloredRegister(id) << " <- " << vm.registers[id].value);
			const std::string message = ":Register " + std::to_string(id) + " " +
				std::to_string(vm.registers[id].value);
			auto lock = lockSubscribers();
			for (int client: registerSubscribers)
				server.send(client, message);
		};

		vm.onJump = [this](Word, Word to) {
			const std::string message = ":PC " + std::to_string(to);
			auto lock = lockSubscribers();
			for (int client: pcSubscribers)
				server.send(client, message);
		};

		vm.onPrint = [this](const std::string &str) {
			std::vector<std::string> hex;
			hex.reserve(str.size());
			std::stringstream ss;
			ss << std::hex;
			for (const char ch: str) {
				if (ch < 16)
					ss << '0';
				ss << static_cast<int>(ch);
				hex.push_back(ss.str());
				ss.clear();
				ss.str("");
			}

			auto lock = lockSubscribers();
			for (int client: outputSubscribers) {
				for (const std::string &ch: hex)
					server.send(client, ch);
			}
		};

		vm.onAddBreakpoint = [this](Word breakpoint) {
			const std::string message = ":AddBP " + std::to_string(breakpoint);
			auto lock = lockSubscribers();
			for (int client: bpSubscribers)
				server.send(client, message);
		};

		vm.onRemoveBreakpoint = [this](Word breakpoint) {
			const std::string message = ":RemoveBP " + std::to_string(breakpoint);
			auto lock = lockSubscribers();
			for (int client: bpSubscribers)
				server.send(client, message);
		};

		vm.onPagingChange = [this](bool enabled) {
			const std::string message = ":Paging " + std::string(enabled? "enabled" : "disabled");
			auto lock = lockSubscribers();
			for (int client: pagingSubscribers)
				server.send(client, message);
		};

		vm.onP0Change = [this](Word addr) {
			const std::string message = ":P0 " + std::to_string(addr);
			auto lock = lockSubscribers();
			for (int client: p0Subscribers)
				server.send(client, message);
		};

		vm.onPlayStart = [this] {
			setFastForward(true);
			broadcast(":Log Playing...");
		};

		vm.onPlayEnd = [this] {
			setFastForward(false);
			broadcast(":Log Paused.");
		};
	}

	void ServerMode::cleanupClient(int client) {
		auto lock = lockSubscribers();
		memorySubscribers.erase(client);
		registerSubscribers.erase(client);
		pcSubscribers.erase(client);
		ffSubscribers.erase(client);
		bpSubscribers.erase(client);
		pagingSubscribers.erase(client);
		p0Subscribers.erase(client);
	}

	void ServerMode::stop() {
		readingKeys = false;
		auto lock = lockSubscribers();
		for (int client: server.getClients()) {
			cleanupClient(client);
			server.send(client, ":Quit");
		}

		server.stop();
	}

	void ServerMode::handleMessage(int client, const std::string &message) {
		if (message.empty() || message.front() != ':') {
			server.send(client, ":Error Invalid message");
			return;
		}

		const std::vector<std::string> split = Util::split(message, " ");
		const size_t size = split.size();
		const std::string verb = split[0].substr(1);

		auto invalid = [&] { server.send(client, ":InvalidMessage " + message); };

		if (verb == "Stop") {
			stop();
		} else if (verb == "Close") {
			cleanupClient(client);
			server.removeClient(client);
		} else if (verb == "Play") {
			if (size == 2) {
				UWord microdelay;
				if (!Util::parseUL(split[1], microdelay))
					server.send(client, ":Error Invalid delay.");
				else
					vm.play(microdelay);
			} else if (size == 1)
				vm.play(0);
			else
				invalid();
		} else if (verb == "Pause") {
			if (size == 1)
				vm.pause();
			else
				invalid();
		} else if (verb == "Subscribe") {
			if (size != 2) {
				invalid();
				return;
			}

			const std::string &to = split[1];
			if (to == "memory") {
				auto lock = lockSubscribers();
				memorySubscribers.insert(client);
				ffSubscribers.insert(client);
				server.send(client, ":MemorySize " + std::to_string(vm.getMemorySize()));
			} else if (to == "registers") {
				auto lock = lockSubscribers();
				registerSubscribers.insert(client);
				ffSubscribers.insert(client);
			} else if (to == "pc") {
				auto lock = lockSubscribers();
				pcSubscribers.insert(client);
				server.send(client, ":PC " + std::to_string(vm.programCounter));
				ffSubscribers.insert(client);
			} else if (to == "output") {
				auto lock = lockSubscribers();
				outputSubscribers.insert(client);
			} else if (to == "bp" || to == "breakpoints") {
				auto lock = lockSubscribers();
				bpSubscribers.insert(client);
				for (int breakpoint: vm.getBreakpoints())
					server.send(client, ":AddBP " + std::to_string(breakpoint));
			} else if (to == "paging") {
				auto lock = lockSubscribers();
				pagingSubscribers.insert(client);
				server.send(client, ":Paging " + std::string(vm.pagingOn? "enabled" : "disabled"));
			} else if (to == "p0") {
				auto lock = lockSubscribers();
				p0Subscribers.insert(client);
				server.send(client, ":P0 " + std::to_string(vm.p0));
			} else {
				invalid();
				return;
			}

			server.send(client, ":Subscribed " + to);
		} else if (verb == "GetMain") {
			sendMemory(client);
		} else if (verb == "Init") {
			vm.init();
		} else if (verb == "Tick") {
			if (vm.paused) {
				broadcast(":Paused");
			} else if (size == 1) {
				tick();
				if (vm.paused)
					broadcast(":Paused");
			} else if (size == 2) {
				Word ticks;
				if (!Util::parseLong(split[1], ticks)) {
					invalid();
					return;
				}

				server.send(client, ":Log Ticking...");

				setFastForward(true);
				Word i;
				vm.start();
				try {
					for (i = 0; i < ticks; ++i) {
						if (!tick())
							break;
					}
				} catch (VMError &err) {
					const bool old_strict = vm.strict;
					vm.strict = false;
					vm.undo();
					vm.strict = old_strict;
					vm.paused = true;
				}

				if (vm.paused)
					broadcast(":Paused");

				server.send(client, ":Log Ticked " + std::to_string(i) + " time" + (i == 1? "" : "s") + ".");

				DBG("Server ticked " << i << " time" << (i == 1? "" : "s") << ".");

				setFastForward(false);
			} else {
				invalid();
			}
		} else if (verb == "Unpause") {
			vm.paused = false;
			broadcast(":Unpaused");
		} else if (verb == "Reg") {
			if (size != 2 && size != 3) {
				invalid();
				return;
			}

			Word reg;
			if (!Util::parseLong(split[1], reg) && (reg = Why::registerID(split[1])) == -1) {
				server.send(client, ":Error Invalid register: " + split[1]);
				return;
			}

			if (size == 3) {
				UWord new_value;
				if (!Util::parseUL(split[2], new_value)) {
					invalid();
					return;
				}

				vm.registers[reg].value = new_value;
			}

			server.send(client, ":Register " + std::to_string(reg) + " " + std::to_string(vm.registers[reg].value));
		} else if (verb == "PrintOps") {
			if (size != 2) {
				invalid();
				return;
			}

			Word count;
			if (!Util::parseLong(split[1], count)) {
				invalid();
				return;
			}

			for (Word address = vm.programCounter, i = 0; i < count; ++i, address += 8) {
				bool success = false;
				Word translated = vm.translateAddress(address, &success);
				if (!success) {
					std::cout << "Couldn't translate " << address << ".\n";
					break;
				}
				std::cout << address << ": " << Unparser::stringify(vm.getInstruction(translated), &vm) << '\n';
			}
		} else if (verb == "Symbols") {
			for (const auto &[name, symbol]: vm.symbolTable)
				std::cout << "\e[1m" << name << "\e[22m: " << symbol.location << " \e[22;2m[" << std::hex << symbol.hash
				          << std::dec << "]\e[22m\n";
		} else if (verb == "GetWord") {
			if (size != 2 && size != 3) {
				invalid();
				return;
			}

			UWord address;
			if (!Util::parseUL(split[1], address)) {
				invalid();
				return;
			}

			Endianness endianness = Endianness::Little;
			if (size == 3) {
				if (split[2] == "B") {
					endianness = Endianness::Big;
				} else if (split[2] != "L") {
					invalid();
					return;
				}
			}

			server.send(client, ":MemoryWord " + std::to_string(address) + " " +
				std::to_string(vm.getWord(address, endianness)) + " " + static_cast<char>(endianness));
		} else if (verb == "SetWord") {
			if (size != 3 && size != 4) {
				invalid();
				return;
			}

			UWord address, value;
			if (!Util::parseUL(split[1], address) || !Util::parseUL(split[2], value)) {
				invalid();
				return;
			}

			Endianness endianness = Endianness::Little;
			if (size == 4) {
				if (split[3] == "B") {
					endianness = Endianness::Big;
				} else if (split[3] != "L") {
					invalid();
					return;
				}
			}

			vm.setWord(address, value, endianness);
			server.send(client, ":MemoryWord " + std::to_string(address) + " " +
				std::to_string(vm.getWord(address, endianness)) + " " + static_cast<char>(endianness));
		} else if (verb == "GetPC") {
			if (size != 1)
				invalid();
			else
				server.send(client, ":PC " + std::to_string(vm.programCounter));
		} else if (verb == "SetPC") {
			Word address;
			if (size != 2 || !Util::parseLong(split[1], address))
				invalid();
			else
				vm.jump(address);
		} else if (verb == "GetString") {
			if (size != 2) {
				invalid();
				return;
			}

			Word address;
			if (!Util::parseLong(split[1], address)) {
				// Look up from the symbol table.
				if (vm.symbolTable.count(split[1]) == 0)
					server.send(client, ":Error GetString: unknown symbol: " + split[1]);
				else
					address = vm.symbolTable.at(split[1]).location;
			}
		} else if (verb == "Registers") {
			if (size == 2 && split[1] == "raw")
				for (int i = 0; i < Why::totalRegisters; ++i)
					server.send(client, ":Register " + std::to_string(i) + " " + std::to_string(vm.registers[i].value));
			else
				for (int i = 0; i < Why::totalRegisters; ++i)
					server.send(client, ":Register $" + Why::registerName(i) + " " +
						std::to_string(vm.registers[i].value));
		} else if (verb == "Reset") {
			vm.reset(false);
			sendMemory(client);
			server.send(client, ":PC " + std::to_string(vm.programCounter));
			server.send(client, ":ResetComplete");
		} else if (verb == "AddBP") {
			Word breakpoint;
			if (size != 2 || !Util::parseLong(split[1], breakpoint)) {
				if (size == 2) {
					if (vm.symbolTable.count(split[1]) == 0) {
						server.send(client, ":Error Function not found.");
						return;
					}

					breakpoint = vm.symbolTable.at(split[1]).location;
				} else {
					invalid();
					return;
				}
			}

			vm.addBreakpoint(breakpoint);
			server.send(client, ":AddedBP " + std::to_string(breakpoint));
		} else if (verb == "RemoveBP") {
			Word breakpoint;
			if (size != 2 || !Util::parseLong(split[1], breakpoint))
				invalid();
			else
				vm.removeBreakpoint(breakpoint);
		} else if (verb == "AskAbout") {
			Word address;
			if (size < 2 || 3 < size || !Util::parseLong(split[1], address))
				invalid();
			else
				try {
					if (size == 3 && split[2] == "t")
						address = vm.translateAddress(address);
					const Word word = vm.getWord(address);
					server.send(client, ":Log " + std::to_string(address) + ": " +
						std::to_string(vm.getByte(address)) + ", " + std::to_string(vm.getHalfword(address)) + " & " +
						std::to_string(vm.getHalfword(address + 4)) + "; " + std::to_string(word) + " == " +
						Util::toHex(word));
				} catch (const std::exception &err) {
					server.send(client, ":Error " + std::string(err.what()));
				}
		} else if (verb == "Undo") {
			vm.undo();
		} else if (verb == "Redo") {
			vm.redo();
		} else if (verb == "LogMemoryWrites") {
			logMemoryWrites = !logMemoryWrites;
			server.send(client, ":LogMemoryWrites " + std::string(logMemoryWrites? "on" : "off"));
		} else if (verb == "LogRegisters") {
			logRegisters = !logRegisters;
			server.send(client, ":LogRegisters " + std::string(logRegisters? "on" : "off"));
		} else if (verb == "Strict") { // useful if you want to mess with registers.
			if (size == 1) {
				vm.strict = !vm.strict;
			} else if (size == 2) {
				if (split[1] == "off" || split[1] == "0" || split[1] == "false" || split[1] == "no") {
					vm.strict = false;
				} else if (split[1] == "on" || split[1] == "1" || split[1] == "true" || split[1] == "yes") {
					vm.strict = true;
				} else {
					invalid();
					return;
				}
			} else {
				invalid();
				return;
			}

			server.send(client, ":Strict " + std::string(vm.strict? "on" : "off"));
		} else if (verb == "GetPC") {
			server.send(client, ":PC " + std::to_string(vm.programCounter));
		} else if (verb == "DebugMap") {
			if (vm.debugMap.empty())
				warn() << "The debug map is empty.\n";
			else
				for (const auto &[address, debug]: vm.debugMap)
					info() << address << ": " << std::string(debug) << '\n';
		} else if (verb == "DebugData") {
			Word address = vm.programCounter;
			if (size != 1) {
				if (split[1] == "all") {
					std::cerr << "Debug map entries: " << vm.debugMap.size() << '\n';
					for (const auto &[offset, data]: vm.debugMap)
						std::cerr << offset << ": " << std::string(data) << '\n';
					std::cerr.flush();
					return;
				}

				if (!Util::parseLong(split[1], address)) {
					invalid();
					return;
				}
			}

			if (vm.debugMap.count(address) == 0)
				broadcast(":Debug " + std::to_string(address) + " Not found");
			else
				broadcast(":Debug " + std::to_string(address) + " " + std::string(vm.debugMap.at(address)));
		} else if (verb == "SetReg") {
			if (size != 3) {
				invalid();
				return;
			}

			const int reg = Why::registerID(split[1]);
			if (reg == -1) {
				server.send(client, ":Error Invalid register.");
				return;
			}

			Word new_value;
			if (!Util::parseLong(split[2], new_value)) {
				invalid();
				return;
			}

			vm.bufferChange<RegisterChange>(vm, reg, new_value, vm.registers.at(reg).type);
			vm.registers[reg].value = new_value;
			vm.onRegisterChange(reg);
			server.send(client, ":SetReg " + std::to_string(reg) + " " + std::to_string(new_value));
		} else if (verb == "History") {
			if (size == 1) {
				server.send(client, ":Log History recording is " + std::string(vm.enableHistory? "on" : "off") + ".");
				return;
			} else if (size != 2) {
				invalid();
				return;
			} else if (split[1] == "on") {
				vm.enableHistory = true;
			} else if (split[1] == "off") {
				vm.enableHistory = false;
			} else if (split[1] == "toggle") {
				vm.enableHistory = !vm.enableHistory;
			} else {
				invalid();
				return;
			}

			broadcast(":Log History recording turned " + std::string(vm.enableHistory? "on" : "off") + ".");
		} else if (verb == "Keybrd") {
			UWord key;
			if (size != 2 || !Util::parseUL(split[1], key, 16)) {
				invalid();
				return;
			}
			auto lock = lockKeys();
			keys.push_back(key);
		} else if (verb == "Dump") {
			Word address, length;
			if ((size != 3 && size != 4) || !Util::parseLong(split[1], address) || !Util::parseLong(split[2], length)) {
				invalid();
				return;
			}

			if (length <= 0) {
				invalid();
				return;
			}

			try {
				if (size == 4 && (split[3] == "t" || split[3] == "tr" || split[3] == "rt"))
					address = vm.translateAddress(address);

				if (size == 4 && (split[3] == "r" || split[3] == "tr" || split[3] == "rt"))
					for (Word i = length - 1; 0 <= i; --i)
						std::cout << Util::toHex(vm.memory.at(address + i), 2).substr(2);
				else
					for (Word i = 0; i < length; ++i)
						std::cout << Util::toHex(vm.memory.at(address + i), 2).substr(2);

				std::cout << std::endl;
			} catch (const std::exception &err) {
				error() << "Failed to dump memory (address=" << address << ", length=" << length << "): "
				        << err.what() << std::endl;
			}
		} else if (verb == "Stacktrace") {
			try {
				size_t i = 0;
				std::cerr << "Stacktrace:\n";
				Word m5 = vm.registers[Why::assemblerOffset + 5].value;
				std::cerr << "    " << i << ": " << vm.registers[Why::returnAddressOffset].value << '\n';
				while (m5 != 0) {
					bool success;
					const Word rt_addr = vm.translateAddress(m5 + 16, &success);
					if (!success)
						throw std::runtime_error("Address translation failed");
					std::cerr << "    " << ++i << ": " << vm.getWord(rt_addr) << std::endl;
					const Word m5_addr = vm.translateAddress(m5, &success);
					if (!success)
						throw std::runtime_error("Address translation failed");
					m5 = vm.getWord(m5_addr);
				}
				if (i == 0)
					std::cerr << "    (empty)\n";
			} catch (const std::exception &err) {
				error() << "Printing stacktrace failed: " << err.what() << std::endl;
			}
		} else {
			server.send(client, ":UnknownVerb " + verb);
		}
	}

	void ServerMode::setFastForward(bool to) {
		auto lock = lockSubscribers();
		if (to) {
			for (int subscriber: ffSubscribers)
				server.send(subscriber, ":FastForward on");
		} else {
			for (int subscriber: ffSubscribers) {
				server.send(subscriber, ":FastForward off");
				server.send(subscriber, ":PC " + std::to_string(vm.programCounter));
			}
		}
	}

	void ServerMode::broadcast(const std::string &message) {
		auto lock = lockSubscribers();
		for (int client: server.getClients())
			server.send(client, message);
	}

	void ServerMode::sendMemory(int client) {
		std::stringstream to_send;
		server.send(client, ":Offsets " + std::to_string(vm.symbolsOffset) + " " + std::to_string(vm.codeOffset) + " " +
			std::to_string(vm.dataOffset) + " " + std::to_string(vm.endOffset));
		to_send << ":MemoryWords 0 " << (vm.endOffset / 8 + 128) << std::hex;
		for (Word i = 0; i < vm.endOffset + 128 * 8; i += 8)
			to_send << " " << vm.getWord(i, Endianness::Little);
		server.send(client, to_send.str());
		server.send(client, ":Done GetMain");
		for (Word address: writtenAddresses)
			server.send(client, ":MemoryWord " + std::to_string(address) + " " +
				std::to_string(vm.getWord(address, Endianness::Little)));
	}

	bool ServerMode::tick() {
#ifdef CATCH_TICK
		const Word pc = vm.programCounter;
		try {
#endif
			return vm.tick();
#ifdef CATCH_TICK
		} catch (std::exception &err) {
			std::cerr << "Execution failed: " << err.what() << "\n";
			std::cerr << "Offending address: " << pc << "\n";
			try {
				Word last = vm.programCounter;
				for (int i = 0; i < 16; ++i) {
					vm.undo();
					if (last != vm.programCounter) {
						std::cerr << "Previous address: " << vm.programCounter << "\n";
						last = vm.programCounter;
					}
				}
			} catch (std::exception &undo_err) {
				std::cerr << "Couldn't rewind.\n";
			}

			throw;
		}
#endif
	}
}
