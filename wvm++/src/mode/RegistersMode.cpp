#include <algorithm>
#include <bitset>
#include <sstream>

#include "lib/ansi.h"
#include "mode/RegistersMode.h"
#include "Unparser.h"
#include "Util.h"

namespace WVM::Mode {
	void RegistersMode::run(const std::string &hostname, int port) {
		ClientMode::run(hostname, port);

		networkThread = std::thread([&]() {
			loop();
		});

		terminal.onInterrupt = [this]() {
			stop();
			terminal.mouse(Haunted::MouseMode::None);
			return true;
		};

		textbox = new Haunted::UI::VectorBox(&terminal);

		expando = new Haunted::UI::Boxes::ExpandoBox(&terminal, Haunted::UI::Boxes::BoxOrientation::Vertical,
			std::initializer_list<Haunted::UI::Boxes::ExpandoBox::ChildPair> {{textbox, -1}});

		textbox->keyFunction = [&](const Haunted::Key &key) {
			if (key == Haunted::KeyMod::Ctrl) {
				if (key == 't') {
					*buffer << ":Tick\n";
				} else if (key == 'g') {
					*buffer << ":GetMain\n";
				} else if (key == 'p') {
					remakeList();
				} else if (key == 'r') {
					*buffer << ":Reset\n:Registers raw\n";
				} else if (key == 'b') {
					base = Base::Binary;
					remakeList();
				} else if (key == 'o') {
					base = Base::Octal;
					remakeList();
				} else if (key == 'd') {
					base = Base::Decimal;
					remakeList();
				} else if (key == 'h') {
					base = Base::Hex;
					remakeList();
				} else if (key == 'l') {
					terminal.redraw();
				} else return false;
			} else if (key == '.') {
				*buffer << ":Tick\n";
			} else return false;
			return true;
		};

		for (int reg = 0; reg < Why::totalRegisters; ++reg)
			registers[reg] = 0;

		terminal.startInput();
		terminal.setRoot(&*expando);
		textbox->setAutoscroll(false);
		terminal.mouse(Haunted::MouseMode::Motion);
		textbox->focus();
		expando->draw();
		terminal.watchSize();
		*buffer << ":Registers raw\n";
		*buffer << ":Subscribe registers\n";
		terminal.join();
		networkThread.join();
	}

	void RegistersMode::remakeList() {
		textbox->clearLines();
		std::stringstream ss;
		textbox->draw();
		textbox->suppressDraw = true;

		for (int reg = 0; reg < Why::totalRegisters; ++reg) {
			stringify(reg, ss);
			*textbox += ss.str();
			ss.clear();
			ss.str("");
		}

		textbox->suppressDraw = false;
		terminal.redraw();
	}

	void RegistersMode::stringify(int reg, std::stringstream &ss) {
		ss << Why::coloredRegister(reg) << std::string(5 - Why::registerName(reg).size(), ' ');
		if (base == Base::Binary) {
			ss << std::bitset<64>(registers[reg]);
		} else if (base == Base::Octal) {
			ss << std::oct << registers[reg];
		} else if (base == Base::Decimal) {
			ss << std::dec << registers[reg];
		} else if (base == Base::Hex) {
			ss << std::hex << registers[reg];
		} else throw std::runtime_error("Invalid base: " + std::to_string(static_cast<int>(base)));
	}

	std::string RegistersMode::stringify(int reg) {
		std::stringstream ss;
		stringify(reg, ss);
		return ss.str();
	}

	void RegistersMode::updateLine(int reg) {
		auto &simple = getLine(reg);
		simple.text = stringify(reg);
		textbox->redrawLine(simple);
	}

	void RegistersMode::stop() {
		ClientMode::stop();
	}

	void RegistersMode::setFastForward(bool to) {
		terminal.suppressOutput = fastForward = to;
		if (!to)
			remakeList();
	}

	void RegistersMode::handleMessage(const std::string &message) {
		if (message.front() != ':') {
			DBG("Not sure how to handle [" << message << "]");
			return;
		}

		const size_t space = message.find(' ');
		const std::string verb = message.substr(1, space - 1);
		const std::string rest = space == std::string::npos? "" : message.substr(space + 1);
		const std::vector<std::string> split = Util::split(rest, " ", false);
		const size_t size = split.size();

		if (verb == "Register") {
			Word reg, value;
			if (size != 2 || !Util::parseLong(split[0], reg) || !Util::parseLong(split[1], value) || reg < 0
			    || 128 <= reg) {
				DBG("Invalid: Registers[" << rest << "]");
				return;
			}
			
			registers[reg] = value;

			if (!ready && reg == 127) {
				ready = true;
				remakeList();
			} else if (ready) {
				updateLine(reg);
			}
		} else if (verb == "FastForward") {
			if (size != 1) {
				DBG("Invalid: FastForward[" << rest << "]");
				return;
			}

			if (split[0] == "on")
				setFastForward(true);
			else if (split[0] == "off")
				setFastForward(false);
			else
				DBG("Invalid: FastForward[" << rest << "]");
		} else if (verb == "Quit") {
			stop();
			std::terminate();
		}
	}

	Haunted::UI::SimpleLine<std::vector> & RegistersMode::getLine(int reg) {
		auto &line = *std::next(textbox->getLines().begin(), reg);
		if (auto *simple = dynamic_cast<Haunted::UI::SimpleLine<std::vector> *>(line.get())) {
			return *simple;
		} else throw std::runtime_error("Unable to cast line to Haunted::UI::simpleline");
	}
}
