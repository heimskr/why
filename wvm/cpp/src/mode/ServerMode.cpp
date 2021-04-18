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

void sigint_handler(int) {
	if (WVM::Mode::ServerMode::instance)
		WVM::Mode::ServerMode::instance->stop();
}

namespace WVM::Mode {
	ServerMode * ServerMode::instance = nullptr;
	void ServerMode::run(const std::string &path) {
		instance = this;
		server.messageHandler = [&](int client, const std::string &message) { handleMessage(client, message); };
		std::cerr << "ServerMode running on port " << server.getPort() << "\n";
		std::ofstream port_stream;
		port_stream.open(".port");
		port_stream << server.getPort();
		port_stream.close();
		vm.load(path);
		initVM();
		signal(SIGINT, sigint_handler);
		server.onEnd = [&](int client, int) { cleanupClient(client); };
		server.run();
	}

	void ServerMode::initVM() {
		vm.onUpdateMemory = [&](Word address, Word unadjusted, Size size) {
			if (logMemoryWrites)
				DBG("[" << unadjusted << "] <- " << vm.get(unadjusted, size));
			writtenAddresses.insert(address);
			const std::string message = ":MemoryWord " + std::to_string(address) + " " +
				std::to_string(vm.getWord(address));
			for (int client: memorySubscribers)
				server.send(client, message);
		};

		vm.onRegisterChange = [&](unsigned char id) {
			if (logRegisters)
				DBG(Why::coloredRegister(id) << " <- " << vm.registers[id]);
			const std::string message = ":Register " + std::to_string(id) + " " + std::to_string(vm.registers[id]);
			for (int client: registerSubscribers)
				server.send(client, message);
		};

		vm.onJump = [&](Word, Word to) {
			const std::string message = ":PC " + std::to_string(to);
			for (int client: pcSubscribers)
				server.send(client, message);
		};

		vm.onPrint = [&](const std::string &str) {
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

			for (int client: outputSubscribers) {
				for (const std::string &ch: hex)
					server.send(client, ch);
			}
		};

		vm.onAddBreakpoint = [&](Word breakpoint) {
			for (int client: bpSubscribers)
				server.send(client, ":AddBP " + std::to_string(breakpoint));
		};

		vm.onRemoveBreakpoint = [&](Word breakpoint) {
			for (int client: bpSubscribers)
				server.send(client, ":RemoveBP " + std::to_string(breakpoint));
		};

		vm.onPagingChange = [&](bool enabled) {
			for (int client: pagingSubscribers)
				server.send(client, ":Paging " + std::string(enabled? "enabled" : "disabled"));
		};

		vm.onP0Change = [&](Word addr) {
			for (int client: p0Subscribers)
				server.send(client, ":P0 " + std::to_string(addr));
		};
	}

	void ServerMode::cleanupClient(int client) {
		memorySubscribers.erase(client);
		registerSubscribers.erase(client);
		pcSubscribers.erase(client);
		ffSubscribers.erase(client);
		bpSubscribers.erase(client);
		pagingSubscribers.erase(client);
		p0Subscribers.erase(client);
	}

	void ServerMode::stop() {
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

		auto invalid = [&]() { server.send(client, ":InvalidMessage " + message); };

		if (verb == "Stop") {
			stop();
		} else if (verb == "Close") {
			cleanupClient(client);
			server.removeClient(client);
		} else if (verb == "Subscribe") {
			if (size != 2) {
				invalid();
				return;
			}

			const std::string &to = split[1];
			if (to == "memory") {
				memorySubscribers.insert(client);
				ffSubscribers.insert(client);
				server.send(client, ":MemorySize " + std::to_string(vm.getMemorySize()));
			} else if (to == "registers") {
				registerSubscribers.insert(client);
				ffSubscribers.insert(client);
			} else if (to == "pc") {
				pcSubscribers.insert(client);
				server.send(client, ":PC " + std::to_string(vm.programCounter));
				ffSubscribers.insert(client);
			} else if (to == "output") {
				outputSubscribers.insert(client);
			} else if (to == "bp" || to == "breakpoints") {
				bpSubscribers.insert(client);
				for (int breakpoint: vm.getBreakpoints())
					server.send(client, ":AddBP " + std::to_string(breakpoint));
			} else if (to == "paging") {
				pagingSubscribers.insert(client);
				server.send(client, ":Paging " + std::string(vm.pagingOn? "enabled" : "disabled"));
			} else if (to == "p0") {
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

				vm.registers[reg] = new_value;
			}

			server.send(client, ":Register " + std::to_string(reg) + " " + std::to_string(vm.registers[reg]));
		} else if (verb == "PrintOps") {
			for (Word i = vm.codeOffset; i < vm.dataOffset; i += 8)
				std::cout << "\e[2m[" << std::setw(5) << i << "]\e[22m "
				          << Unparser::stringify(vm.getInstruction(i), &vm) << "\n";
		} else if (verb == "Symbols") {
			for (const std::pair<std::string, Symbol> pair: vm.symbolTable) {
				std::cout << "\e[1m" << pair.first << "\e[22m: " << pair.second.location << " \e[22;2m[" << std::hex
				          << pair.second.hash << std::dec << "]\e[22m\n";
			}
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
			if (size != 1) {
				invalid();
				return;
			}

			server.send(client, ":PC " + std::to_string(vm.programCounter));
		} else if (verb == "SetPC") {
			Word address;
			if (size != 2 || !Util::parseLong(split[1], address)) {
				invalid();
				return;
			}

			vm.jump(address);
		} else if (verb == "GetString") {
			if (size != 2) {
				invalid();
				return;
			}

			Word address;
			if (!Util::parseLong(split[1], address)) {
				// Look up from the symbol table.
				if (vm.symbolTable.count(split[1]) == 0) {
					server.send(client, ":Error GetString: unknown symbol: " + split[1]);
					return;
				}

				address = vm.symbolTable.at(split[1]).location;
			}

			// std::replace
		} else if (verb == "Registers") {
			if (size == 2 && split[1] == "raw") {
				for (int i = 0; i < Why::totalRegisters; ++i)
					server.send(client, ":Register " + std::to_string(i) + " " + std::to_string(vm.registers[i]));
			} else {
				for (int i = 0; i < Why::totalRegisters; ++i)
					server.send(client, ":Register $" + Why::registerName(i) + " " + std::to_string(vm.registers[i]));
			}
		} else if (verb == "Reset") {
			vm.reset(false);
			sendMemory(client);
			server.send(client, ":PC " + std::to_string(vm.programCounter));
			server.send(client, ":ResetComplete");
		} else if (verb == "AddBP") {
			Word breakpoint;
			if (size != 2 || !Util::parseLong(split[1], breakpoint)) {
				invalid();
				return;
			}

			vm.addBreakpoint(breakpoint);
		} else if (verb == "RemoveBP") {
			Word breakpoint;
			if (size != 2 || !Util::parseLong(split[1], breakpoint)) {
				invalid();
				return;
			}

			vm.removeBreakpoint(breakpoint);
		} else if (verb == "AskAbout") {
			Word address;
			if (size != 2 || !Util::parseLong(split[1], address)) {
				invalid();
				return;
			}

			broadcast(":Log " + std::to_string(address) + ": " + std::to_string(vm.getHalfword(address)) + ", " +
				std::to_string(vm.getHalfword(address + 4)) + ", " + std::to_string(vm.getWord(address)));
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
		} else {
			server.send(client, ":UnknownVerb " + verb);
		}
	}

	void ServerMode::setFastForward(bool to) {
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
		for (int client: server.getClients())
			server.send(client, message);
	}

	void ServerMode::sendMemory(int client) {
		std::stringstream to_send;
		server.send(client, ":Offsets " + std::to_string(vm.symbolsOffset) + " " + std::to_string(vm.codeOffset) + " "
			+ std::to_string(vm.dataOffset) + " " + std::to_string(vm.endOffset));
		to_send << ":MemoryWords 0 " << (vm.endOffset / 8 + 128) << std::hex;
		for (Word i = 0; i < vm.endOffset + 128 * 8; i += 8)
			to_send << " " << vm.getWord(i, Endianness::Little);
		server.send(client, to_send.str());
		server.send(client, ":Done GetMain");
		for (Word address: writtenAddresses) {
			server.send(client, ":MemoryWord " + std::to_string(address) + " " +
				std::to_string(vm.getWord(address, Endianness::Little)));
		}
	}

	bool ServerMode::tick() {
		Word pc = vm.programCounter;
		try {
			return vm.tick();
		} catch (std::exception &err) {
			std::cerr << "Execution failed: " << err.what() << "\n";
			std::cerr << "Offending address: " << pc << "\n";
			try {
				for (int i = 0; i < 16; ++i) {
					vm.undo();
					std::cerr << "Previous address: " << vm.programCounter << "\n";
				}
			} catch(std::exception &undo_err) {
				std::cerr << "Couldn't rewind.\n";
			}

			throw;
		}
	}
}
