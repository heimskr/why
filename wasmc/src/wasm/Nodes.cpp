#include <iostream>
#include <sstream>

#include "parser/Lexer.h"
#include "parser/Parser.h"
#include "parser/StringSet.h"
#include "util/Color.h"
#include "util/Util.h"
#include "wasm/Nodes.h"
#include "wasm/Registers.h"

namespace Wasmc {
	Condition getCondition(const std::string &str) {
		if (str == "0")
			return Condition::Zero;
		if (str == "+")
			return Condition::Positive;
		if (str == "-")
			return Condition::Negative;
		if (str == "*")
			return Condition::Nonzero;
		if (!str.empty())
			wasmerror("Invalid condition: " + str);
		return Condition::None;
	}

	Condition getCondition(const ASTNode *node) {
		return node? getCondition(*node->lexerInfo) : Condition::None;
	}

	static const char * conditionString(Condition condition) {
		switch (condition) {
			case Condition::None:     return "";
			case Condition::Zero:     return "0";
			case Condition::Positive: return "+";
			case Condition::Negative: return "-";
			case Condition::Nonzero:  return "*";
			default:
				throw std::runtime_error("Invalid condition in WASMJNode: "
					+ std::to_string(static_cast<int>(condition)));
		}
	}

	WASMBaseNode::WASMBaseNode(int sym): ASTNode(wasmParser, sym) {}

	WASMInstructionNode * WASMInstructionNode::absorbIntbang(ASTNode *intbang) {
		if (intbang) {
			bang = intbang->children.front()->atoi();
			delete intbang;
		}

		return this;
	}

	WASMInstructionNode * WASMInstructionNode::absorbLabel(ASTNode *label) {
		if (label) {
			labels.push_back(dynamic_cast<WASMLabelNode *>(label)->label);
			delete label;
		}

		return this;
	}

	WASMInstructionNode * WASMInstructionNode::absorb(const WASMInstructionNode &other) {
		bang = other.bang;
		labels = other.labels;
		flags = other.flags;
		return this;
	}

	WASMInstructionNode * WASMInstructionNode::setInSubroutine(bool value) {
		inSubroutine = value;
		return this;
	}

	WASMInstructionNode * WASMInstructionNode::setBang(int value) {
		bang = value;
		return this;
	}

	std::string WASMInstructionNode::debugExtra() const {
		std::string out = "";
		for (const std::string *label: labels)
			out += blue("@") + orange(*label) + " ";
		return out;
	}

	WASMInstructionNode::operator std::string() const {
		std::string out;
		for (const std::string *label: labels)
			out += "@" + *label + " ";
		return out;
	}

	WASMImmediateNode::WASMImmediateNode(ASTNode *node):
	WASMBaseNode(WASM_IMMEDIATE), HasImmediate(getImmediate(node)) {
		delete node;
	}

	WASMImmediateNode::WASMImmediateNode(const Immediate &imm_): WASMBaseNode(WASM_IMMEDIATE), HasImmediate(imm_) {}

	std::string WASMImmediateNode::debugExtra() const {
		return colorize(imm);
	}

	WASMImmediateNode::operator std::string() const {
		return toString(imm);
	}

	WASMLabelNode::WASMLabelNode(ASTNode *label_): WASMInstructionNode(WASM_LABEL), label(label_->lexerInfo) {
		delete label_;
	}

	WASMLabelNode::WASMLabelNode(const std::string *label_): WASMInstructionNode(WASM_LABEL), label(label_) {}

	WASMInstructionNode * WASMLabelNode::copy() const {
		return (new WASMLabelNode(label))->absorb(*this);
	}

	std::string WASMLabelNode::debugExtra() const {
		return cyan("@") + "\e[38;5;202m" + *label + "\e[39m";
	}

	WASMLabelNode::operator std::string() const {
		return "@" + *label;
	}

	RNode::RNode(ASTNode *rs_, ASTNode *oper_, ASTNode *rt_, ASTNode *rd_, ASTNode *unsigned_):
	WASMInstructionNode(WASM_RNODE), RType(rs_, rt_, rd_), HasOper(oper_), HasUnsigned(!!unsigned_) {
		locate(oper_);
		if (rs_ != rd_)
			delete rs_;
		if (oper_ != rt_)
			delete rt_;
		delete oper_;
		delete rd_;
		if (unsigned_)
			delete unsigned_;
	}

	RNode::RNode(const std::string *rs_, const std::string *oper_, const std::string *rt_, const std::string *rd_,
	             int oper_token, bool is_unsigned):
		WASMInstructionNode(WASM_RNODE), RType(rs_, rt_, rd_), HasOper(oper_token, oper_), HasUnsigned(is_unsigned) {}

	WASMInstructionNode * RNode::copy() const {
		return (new RNode(rs, oper, rt, rd, operToken, isUnsigned))->absorb(*this);
	}

	Opcode RNode::getOpcode() const {
		try {
			const std::string &instruction = TOKEN_INSTRUCTIONS_R.at(operToken);
			return OPCODES.at(isUnsigned? UNSIGNED_EQUIVALENTS.at(instruction) : instruction);
		} catch (const std::out_of_range &) {
			debug();
			std::cerr << "Couldn't find opcode for token " << operToken << " (" << *oper << ")\n";
			throw;
		}
	}

	Funct RNode::getFunct() const {
		const std::string *instruction;
		try {
			instruction = &TOKEN_INSTRUCTIONS_R.at(operToken);
		} catch (const std::out_of_range &) {
			debug();
			error() << "Couldn't find instruction for token " << operToken << " (" << *oper << ")\n";
			throw;
		}

		std::string function;

		try {
			function = isUnsigned? UNSIGNED_EQUIVALENTS.at(*instruction) : *instruction;
		} catch (const std::out_of_range &) {
			debug();
			error() << "Couldn't find unsigned equivalent for " << *instruction << "\n";
			throw;
		}

		try {
			return FUNCTS.at(isUnsigned? UNSIGNED_EQUIVALENTS.at(*instruction) : *instruction);
		} catch (const std::out_of_range &) {
			debug();
			error() << "Couldn't find function for token " << operToken << " (" << *oper << ")\n";
			throw;
		}
	}

	std::string RNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(*rs) + " " + dim(*oper) + " " + cyan(*rt) + dim(" -> ")
			+ cyan(*rd) + (isUnsigned? " /u" : "");
	}

	RNode::operator std::string() const {
		const std::string base = WASMInstructionNode::operator std::string();
		if (*oper == "!" || *oper == "~")
			return base + *oper + *rs + " -> " + *rd + (isUnsigned? " /u" : "");
		return base + *rs + " " + *oper + " " + *rt + " -> " + *rd + (isUnsigned? " /u" : "");
	}

	INode::INode(ASTNode *rs_, ASTNode *oper_, ASTNode *imm_, ASTNode *rd_, ASTNode *unsigned_):
	WASMInstructionNode(WASM_INODE), IType(rs_, rd_, imm_), HasOper(oper_), HasUnsigned(unsigned_) {
		if (rs_ != rd_)
			delete rs_;
		delete oper_;
		delete imm_;
		delete rd_;
		if (unsigned_)
			delete unsigned_;
	}

	INode::INode(const std::string *rs_, const std::string *oper_, const Immediate &imm_, const std::string *rd_,
	             int oper_token, bool is_unsigned):
		WASMInstructionNode(WASM_INODE), IType(rs_, rd_, imm_), HasOper(oper_token, oper_), HasUnsigned(is_unsigned) {}

	Opcode INode::getOpcode() const {
		try {
			const std::string &instruction = TOKEN_INSTRUCTIONS_I.at(operToken);
			return OPCODES.at(isUnsigned? UNSIGNED_EQUIVALENTS.at(instruction) : instruction);
		} catch (const std::out_of_range &) {
			debug();
			std::cerr << "Couldn't find opcode for token " << operToken << " (" << *oper << ")\n";
			throw;
		}
	}

	WASMInstructionNode * INode::copy() const {
		return (new INode(rs, oper, imm, rd, operToken, isUnsigned))->absorb(*this);
	}

	std::string INode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(*rs) + " " + dim(*oper) + " " + colorize(imm) + dim(" -> ")
			+ cyan(*rd) + (isUnsigned? " /u" : "");
	}

	INode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + *rs + " " + *oper + " " + toString(imm) + " -> " + *rd
			+ (isUnsigned? " /u" : "");
	}

	WASMMemoryNode::WASMMemoryNode(int sym, ASTNode *rs_, ASTNode *rd_, ASTNode *byte_):
	WASMInstructionNode(sym), RType(rs_, nullptr, rd_), isByte(!!byte_) {
		delete rs_;
		delete rd_;
		if (byte_)
			delete byte_;
	}

	WASMMemoryNode::WASMMemoryNode(int sym, const std::string *rs_, const std::string *rd_, bool is_byte):
		WASMInstructionNode(sym), RType(rs_, nullptr, rd_), isByte(is_byte) {}

	WASMCopyNode::WASMCopyNode(ASTNode *rs_, ASTNode *rd_, ASTNode *byte_):
		WASMMemoryNode(WASM_COPYNODE, rs_, rd_, byte_) {}

	WASMCopyNode::WASMCopyNode(const std::string *rs_, const std::string *rd_, bool is_byte):
		WASMMemoryNode(WASM_COPYNODE, rs_, rd_, is_byte) {}

	WASMInstructionNode * WASMCopyNode::copy() const {
		return (new WASMCopyNode(rs, rd, isByte))->absorb(*this);
	}

	std::string WASMCopyNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim("[") + cyan(*rs) + dim("] -> [") + cyan(*rd) + dim("]")
			+ (isByte? " /b" : "");
	}

	WASMCopyNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "[" + *rs + "] -> [" + *rd + "]" + (isByte? " /b" : "");
	}

	WASMLoadNode::WASMLoadNode(ASTNode *rs_, ASTNode *rd_, ASTNode *byte_):
		WASMMemoryNode(WASM_LOADNODE, rs_, rd_, byte_) {}

	WASMLoadNode::WASMLoadNode(const std::string *rs_, const std::string *rd_, bool is_byte):
		WASMMemoryNode(WASM_LOADNODE, rs_, rd_, is_byte) {}

	WASMInstructionNode * WASMLoadNode::copy() const {
		return (new WASMLoadNode(rs, rd, isByte))->absorb(*this);
	}

	std::string WASMLoadNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim("[") + cyan(*rs) + dim("] -> ") + cyan(*rd)
			+ (isByte? " /b" : "");
	}

	WASMLoadNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "[" + *rs + "] -> " + *rd + (isByte? " /b" : "");
	}

	WASMStoreNode::WASMStoreNode(ASTNode *rs_, ASTNode *rd_, ASTNode *byte_):
		WASMMemoryNode(WASM_STORENODE, rs_, rd_, byte_) {}

	WASMStoreNode::WASMStoreNode(const std::string *rs_, const std::string *rd_, bool is_byte):
		WASMMemoryNode(WASM_STORENODE, rs_, rd_, is_byte) {}

	WASMInstructionNode * WASMStoreNode::copy() const {
		return (new WASMStoreNode(rs, rd, isByte))->absorb(*this);
	}

	std::string WASMStoreNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(*rs) + dim(" -> [") + cyan(*rd) + dim("]")
			+ (isByte? " /b" : "");
	}

	WASMStoreNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + *rs + " -> [" + *rd + "]" + (isByte? " /b" : "");
	}

	WASMSetNode::WASMSetNode(ASTNode *imm_, ASTNode *rd_):
	WASMSetNode(getImmediate(imm_), rd_->lexerInfo) {
		delete imm_;
		delete rd_;
	}

	WASMSetNode::WASMSetNode(const Immediate &imm_, const std::string *rd_):
		WASMInstructionNode(WASM_SETNODE), IType(nullptr, rd_, imm_) {}

	WASMInstructionNode * WASMSetNode::copy() const {
		return (new WASMSetNode(imm, rd))->absorb(*this);
	}

	std::string WASMSetNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + colorize(imm) + dim(" -> ") + cyan(*rd);
	}

	WASMSetNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + toString(imm) + " -> " + *rd;
	}

	WASMLiNode::WASMLiNode(ASTNode *imm_, ASTNode *rd_, ASTNode *byte_):
	WASMLiNode(getImmediate(imm_), rd_->lexerInfo, !!byte_) {
		delete imm_;
		delete rd_;
		if (byte_)
			delete byte_;
	}

	WASMLiNode::WASMLiNode(const Immediate &imm_, const std::string *rd_, bool is_byte):
		WASMInstructionNode(WASM_LINODE), IType(nullptr, rd_, imm_), isByte(is_byte) {}

	WASMInstructionNode * WASMLiNode::copy() const {
		return (new WASMLiNode(imm, rd, isByte))->absorb(*this);
	}

	std::string WASMLiNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim("[") + colorize(imm) + dim("] -> ") + cyan(*rd)
			+ (isByte? " /b" : "");
	}

	WASMLiNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "[" + toString(imm) + "] -> " + *rd + (isByte? " /b" : "");
	}

	WASMSiNode::WASMSiNode(ASTNode *rs_, ASTNode *imm_, ASTNode *byte_):
	WASMInstructionNode(WASM_SINODE), IType(rs_, nullptr, imm_), isByte(!!byte_) {
		delete rs_;
		delete imm_;
		if (byte_)
			delete byte_;
	}

	WASMSiNode::WASMSiNode(const Immediate &imm_, const std::string *rs_, bool is_byte):
		WASMInstructionNode(WASM_SINODE), IType(rs_, nullptr, imm_), isByte(is_byte) {}

	WASMInstructionNode * WASMSiNode::copy() const {
		return (new WASMSiNode(imm, rs, isByte))->absorb(*this);
	}

	std::string WASMSiNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(*rs) + dim(" -> [") + colorize(imm) + dim("]")
			+ (isByte? " /b" : "");
	}

	WASMSiNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + *rs + " -> [" + toString(imm) + "]" + (isByte? " /b" : "");
	}

	WASMLniNode::WASMLniNode(ASTNode *imm_, ASTNode *rd_, ASTNode *byte_): WASMLiNode(imm_, rd_, byte_) {
		symbol = WASM_LNINODE;
	}

	WASMLniNode::WASMLniNode(const Immediate &imm_, const std::string *rd_, bool is_byte):
	WASMLiNode(imm_, rd_, is_byte) {
		symbol = WASM_LNINODE;
	}

	WASMInstructionNode * WASMLniNode::copy() const {
		return (new WASMLniNode(imm, rd, isByte))->absorb(*this);
	}

	std::string WASMLniNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim("[") + colorize(imm) + dim("] -> [") + cyan(*rd) + dim("]")
			+ (isByte? " /b" : "");
	}

	WASMLniNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "[" + toString(imm) + "] -> [" + *rd + "]"
			+ (isByte? " /b" : "");
	}

	WASMHalfMemoryNode::WASMHalfMemoryNode(int sym, ASTNode *rs_, ASTNode *rd_):
	WASMInstructionNode(sym), RType(rs_, nullptr, rd_) {
		delete rs_;
		delete rd_;
	}

	WASMHalfMemoryNode::WASMHalfMemoryNode(int sym, const std::string *rs_, const std::string *rd_):
		WASMInstructionNode(sym), RType(rs_, nullptr, rd_) {}

	WASMChNode::WASMChNode(ASTNode *rs_, ASTNode *rd_):
		WASMHalfMemoryNode(WASM_CHNODE, rs_, rd_) {}

	WASMChNode::WASMChNode(const std::string *rs_, const std::string *rd_):
		WASMHalfMemoryNode(WASM_CHNODE, rs_, rd_) {}

	std::string WASMChNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim("[") + cyan(*rs) + dim("] -> [") + cyan(*rd) + dim("]") + " /h";
	}

	WASMChNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "[" + *rs + "] -> [" + *rd + "] /h";
	}

	WASMLhNode::WASMLhNode(ASTNode *rs_, ASTNode *rd_):
		WASMHalfMemoryNode(WASM_LHNODE, rs_, rd_) {}

	WASMLhNode::WASMLhNode(const std::string *rs_, const std::string *rd_):
		WASMHalfMemoryNode(WASM_LHNODE, rs_, rd_) {}

	std::string WASMLhNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim("[") + cyan(*rs) + dim("] -> ") + cyan(*rd) + " /h";
	}

	WASMLhNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "[" + *rs + "] -> " + *rd + " /h";
	}

	WASMShNode::WASMShNode(ASTNode *rs_, ASTNode *rd_):
		WASMHalfMemoryNode(WASM_SHNODE, rs_, rd_) {}

	WASMShNode::WASMShNode(const std::string *rs_, const std::string *rd_):
		WASMHalfMemoryNode(WASM_SHNODE, rs_, rd_) {}

	std::string WASMShNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(*rs) + dim(" -> [") + cyan(*rd) + dim("]") + " /h";
	}

	WASMShNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + *rs + " -> [" + *rd + "] /h";
	}

	WASMShortMemoryNode::WASMShortMemoryNode(int sym, ASTNode *rs_, ASTNode *rd_):
	WASMInstructionNode(sym), RType(rs_, nullptr, rd_) {
		delete rs_;
		delete rd_;
	}

	WASMShortMemoryNode::WASMShortMemoryNode(int sym, const std::string *rs_, const std::string *rd_):
		WASMInstructionNode(sym), RType(rs_, nullptr, rd_) {}

	WASMCsNode::WASMCsNode(ASTNode *rs_, ASTNode *rd_):
		WASMShortMemoryNode(WASM_CSNODE, rs_, rd_) {}

	WASMCsNode::WASMCsNode(const std::string *rs_, const std::string *rd_):
		WASMShortMemoryNode(WASM_CSNODE, rs_, rd_) {}

	std::string WASMCsNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim("[") + cyan(*rs) + dim("] -> [") + cyan(*rd) + dim("]") + " /s";
	}

	WASMCsNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "[" + *rs + "] -> [" + *rd + "] /s";
	}

	WASMLsNode::WASMLsNode(ASTNode *rs_, ASTNode *rd_):
		WASMShortMemoryNode(WASM_LSNODE, rs_, rd_) {}

	WASMLsNode::WASMLsNode(const std::string *rs_, const std::string *rd_):
		WASMShortMemoryNode(WASM_LSNODE, rs_, rd_) {}

	std::string WASMLsNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim("[") + cyan(*rs) + dim("] -> ") + cyan(*rd) + " /s";
	}

	WASMLsNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "[" + *rs + "] -> " + *rd + " /s";
	}

	WASMSsNode::WASMSsNode(ASTNode *rs_, ASTNode *rd_):
		WASMShortMemoryNode(WASM_SSNODE, rs_, rd_) {}

	WASMSsNode::WASMSsNode(const std::string *rs_, const std::string *rd_):
		WASMShortMemoryNode(WASM_SSNODE, rs_, rd_) {}

	std::string WASMSsNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(*rs) + dim(" -> [") + cyan(*rd) + dim("]") + " /s";
	}

	WASMSsNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + *rs + " -> [" + *rd + "] /s";
	}

	WASMCmpNode::WASMCmpNode(ASTNode *rs_, ASTNode *rt_): WASMInstructionNode(WASM_CMPNODE), RType(rs_, rt_, nullptr) {
		delete rs_;
		delete rt_;
	}

	WASMCmpNode::WASMCmpNode(const std::string *rs_, const std::string *rt_):
		WASMInstructionNode(WASM_CMPNODE), RType(rs_, rt_, nullptr) {}

	std::string WASMCmpNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(*rs) + dim(" ~ ") + cyan(*rt);
	}

	WASMCmpNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + *rs + " ~ " + *rt;
	}

	WASMCmpiNode::WASMCmpiNode(ASTNode *rs_, ASTNode *imm_):
	WASMInstructionNode(WASM_CMPINODE), IType(rs_, nullptr, imm_) {
		delete rs_;
		delete imm_;
	}

	WASMCmpiNode::WASMCmpiNode(const std::string *rs_, const Immediate &imm_):
		WASMInstructionNode(WASM_CMPINODE), IType(rs_, nullptr, imm_) {}

	std::string WASMCmpiNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(*rs) + dim(" ~ ") + colorize(imm);
	}

	WASMCmpiNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + *rs + " ~ " + toString(imm);
	}

	WASMSelNode::WASMSelNode(ASTNode *rs_, ASTNode *oper_, ASTNode *rt_, ASTNode *rd_):
	WASMInstructionNode(WASM_SELNODE), RType(rs_, rt_, rd_), HasCondition(Condition::None) {
		delete rs_;
		delete rt_;
		delete rd_;
		if (*oper_->lexerInfo == "=")
			condition = Condition::Zero;
		else if (*oper_->lexerInfo == "<")
			condition = Condition::Negative;
		else if (*oper_->lexerInfo == ">")
			condition = Condition::Positive;
		else if (*oper_->lexerInfo == "!=")
			condition = Condition::Nonzero;
		else
			wasmerror("Invalid operator: " + *oper_->lexerInfo);
		delete oper_;
	}

	WASMSelNode::WASMSelNode(const std::string *rs_, Condition cond, const std::string *rt_, const std::string *rd_):
		WASMInstructionNode(WASM_SELNODE), RType(rs_, rt_, rd_), HasCondition(cond) {}

	std::string WASMSelNode::debugExtra() const {
		const char *oper_;
		switch (condition) {
			case Condition::Zero:     oper_ = "=";  break;
			case Condition::Negative: oper_ = "<";  break;
			case Condition::Positive: oper_ = ">";  break;
			case Condition::Nonzero:  oper_ = "!="; break;
			default:
				throw std::runtime_error("Invalid operator in WASMSelNode: " +
					std::to_string(static_cast<int>(condition)));
		}
		return WASMInstructionNode::debugExtra() + dim("[") + cyan(*rs) + " " + dim(oper_) + " " + cyan(*rt)
			+ dim("] -> ") + cyan(*rd);
	}

	WASMSelNode::operator std::string() const {
		const char *oper_;
		switch (condition) {
			case Condition::Zero:     oper_ = "=";  break;
			case Condition::Negative: oper_ = "<";  break;
			case Condition::Positive: oper_ = ">";  break;
			case Condition::Nonzero:  oper_ = "!="; break;
			default:
				throw std::runtime_error("Invalid operator in WASMSelNode: " +
					std::to_string(static_cast<int>(condition)));
		}
		return WASMInstructionNode::operator std::string() + "[" + *rs + " " + oper_ + " " + *rt + "] -> " + *rd;
	}

	WASMJNode::WASMJNode(ASTNode *cond, ASTNode *colons, ASTNode *addr):
	WASMInstructionNode(WASM_JNODE), JType(cond, colons, addr, nullptr) {
		delete addr;
		delete colons;
		delete cond;
	}

	WASMJNode::WASMJNode(const Immediate &addr, bool link_, Condition cond):
		WASMInstructionNode(WASM_JNODE), JType(cond, link_, addr, nullptr) {}

	std::string WASMJNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim(conditionString(condition) + std::string(link? "::" : ":")) + " "
			+ colorize(imm);
	}

	WASMJNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + conditionString(condition) + std::string(link? "::" : ":")
			+ " " + toString(imm);
	}

	WASMJcNode::WASMJcNode(WASMJNode *j, ASTNode *rs_): WASMInstructionNode(WASM_JCNODE),
	JType(j? j->condition : Condition::None, j? j->link : false, j? j->imm : 0, rs_->lexerInfo) {
		if (!j) {
			wasmerror("No WASMJNode found in jc instruction");
		} else {
			if (j->condition != Condition::None)
				wasmerror("Conditions specified for jc instruction will be ignored");
			delete j;
		}
		delete rs_;
	}

	WASMJcNode::WASMJcNode(const Immediate &imm_, bool link_, const std::string *rs_):
		WASMInstructionNode(WASM_JCNODE), JType(Condition::None, link_, imm_, rs_) {}

	std::string WASMJcNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim(link? "::" : ":") + " " + colorize(imm) + red(" if ")
			+ cyan(*rs);
	}

	WASMJcNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(link? "::" : ":") + " " + toString(imm)
			+ " if " + *rs;
	}

	WASMJrNode::WASMJrNode(ASTNode *cond, ASTNode *colons, ASTNode *rd_):
	WASMInstructionNode(WASM_JRNODE), RType(nullptr, nullptr, rd_), HasCondition(cond), HasLink(colons) {
		delete colons;
		delete rd_;
		delete cond;
	}

	WASMJrNode::WASMJrNode(Condition condition_, bool link_, const std::string &rd_):
		WASMInstructionNode(WASM_JRNODE), RType(nullptr, nullptr, StringSet::intern(rd_)), HasCondition(condition_),
		HasLink(link_) {}


	WASMJrNode::WASMJrNode(Condition condition_, bool link_, const std::string *rd_):
		WASMInstructionNode(WASM_JRNODE), RType(nullptr, nullptr, rd_), HasCondition(condition_), HasLink(link_) {}

	std::string WASMJrNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim(conditionString(condition) + std::string(link? "::" : ":")) + " "
			+ cyan(*rd);
	}

	WASMJrNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + conditionString(condition) + std::string(link? "::" : ":")
			+ " " + *rd;
	}

	WASMJrcNode::WASMJrcNode(WASMJrNode *jr, ASTNode *rs_):
	WASMInstructionNode(WASM_JRCNODE), RType(rs_->lexerInfo, nullptr, jr? jr->rd : nullptr),
	HasLink(jr? jr->link : false) {
		if (!jr) {
			wasmerror("No WASMJrNode found in jr(l)c instruction");
		} else {
			if (jr->condition != Condition::None)
				wasmerror("Conditions specified for jr(l)c instruction will be ignored");
			delete jr;
		}
		delete rs_;
	}

	WASMJrcNode::WASMJrcNode(bool link_, const std::string *rs_, const std::string *rd_):
		WASMInstructionNode(WASM_JRCNODE), RType(rs_, nullptr, rd_), HasLink(link_) {}

	std::string WASMJrcNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim(link? "::" : ":") + " " + cyan(*rd) + red(" if ") + cyan(*rs);
	}

	WASMJrcNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(link? "::" : ":") + " " + *rd + " if " + *rs;
	}

	WASMJeqNode::WASMJeqNode(WASMJNode *j, ASTNode *rs_, ASTNode *rt_):
	WASMInstructionNode(WASM_JEQNODE), link(j? j->link : false), addr(j? j->imm : 0), rt(getEither(rt_)),
	rs(rs_->lexerInfo) {
		if (!j) {
			wasmerror("No WASMJNode found in jeq instruction");
		} else {
			if (j->condition != Condition::None)
				wasmerror("Conditions specified for jeq instruction will be ignored");
			delete j;
		}
		delete rs_;
	}

	WASMJeqNode::WASMJeqNode(WASMJrNode *jr, ASTNode *rs_, ASTNode *rt_):
	WASMInstructionNode(WASM_JEQNODE), link(jr? jr->link : false), addr(jr? jr->rd : nullptr), rt(getEither(rt_)),
	rs(rs_->lexerInfo) {
		if (!jr) {
			wasmerror("No WASMJrNode found in jeq instruction");
		} else {
			if (jr->condition != Condition::None)
				wasmerror("Conditions specified for jeq instruction will be ignored");
			delete jr;
		}
		delete rs_;
	}

	WASMJeqNode::WASMJeqNode(const Either &addr_, bool link_, const std::string *rs_, const Either &rt_):
		WASMInstructionNode(WASM_JEQNODE), link(link_), addr(addr_), rt(rt_), rs(rs_) {}

	size_t WASMJeqNode::expandedSize() const {
		return !std::holds_alternative<Register>(addr) || std::holds_alternative<Register>(rt)? 2 : 3;
	}

	std::string WASMJeqNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim(link? "::" : ":") + " " + colorize(addr) + red(" if ")
			+ cyan(*rs) + " == " + colorize(rt);
	}

	WASMJeqNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(link? "::" : ":") + " " + toString(addr)
			+ " if " + *rs + " == " + toString(rt);
	}

	WASMSizedStackNode::WASMSizedStackNode(ASTNode *size, ASTNode *rs_, bool is_push):
	WASMInstructionNode(WASM_SIZEDSTACKNODE), IType(rs_, nullptr, size), isPush(is_push) {
		delete size;
		delete rs_;
	}

	WASMSizedStackNode::WASMSizedStackNode(int size, const std::string *rs_, bool is_push):
		WASMInstructionNode(WASM_SIZEDSTACKNODE), IType(rs_, nullptr, size), isPush(is_push) {}

	WASMInstructionNode * WASMSizedStackNode::copy() const {
		return (new WASMSizedStackNode(std::get<int>(imm), rs, isPush))->absorb(*this);
	}

	std::string WASMSizedStackNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim(std::string(isPush? "[" : "]") + ":"
			+ std::to_string(std::get<int>(imm))) + " " + cyan(*rs);
	}

	WASMSizedStackNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(isPush? "[" : "]") + ":"
			+ std::to_string(std::get<int>(imm)) + " " + *rs;
	}

	WASMMultRNode::WASMMultRNode(ASTNode *rs_, ASTNode *rt_, ASTNode *unsigned_):
	WASMInstructionNode(WASM_MULTRNODE), RType(rs_, rt_, nullptr), HasUnsigned(unsigned_) {
		delete rs_;
		delete rt_;
		delete unsigned_;
	}

	WASMMultRNode::WASMMultRNode(const std::string *rs_, const std::string *rt_, bool is_unsigned):
		WASMInstructionNode(WASM_MULTRNODE), RType(rs_, rt_, nullptr), HasUnsigned(is_unsigned) {}

	std::string WASMMultRNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(*rs) + dim(" * ") + cyan(*rt) + (isUnsigned? " /u" : "");
	}

	WASMMultRNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + *rs + " * " + *rt + (isUnsigned? " /u" : "");
	}

	WASMMultINode::WASMMultINode(ASTNode *rs_, ASTNode *imm_, ASTNode *unsigned_):
	WASMInstructionNode(WASM_MULTINODE), IType(rs_, nullptr, imm_), HasUnsigned(unsigned_) {
		delete rs_;
		delete imm_;
		if (unsigned_)
			delete unsigned_;
	}

	WASMMultINode::WASMMultINode(const std::string *rs_, const Immediate &imm_, bool is_unsigned):
		WASMInstructionNode(WASM_MULTINODE), IType(rs_, nullptr, imm_), HasUnsigned(is_unsigned) {}

	std::string WASMMultINode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(*rs) + dim(" * ") + colorize(imm) + (isUnsigned? " /u" : "");
	};

	WASMMultINode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + *rs + " * " + toString(imm) + (isUnsigned? " /u" : "");
	}

	WASMDiviINode::WASMDiviINode(ASTNode *imm_, ASTNode *rs_, ASTNode *rd_, ASTNode *unsigned_):
	WASMInstructionNode(WASM_DIVIINODE), IType(rs_, rd_, imm_), HasUnsigned(unsigned_) {
		delete rs_;
		delete rd_;
		delete imm_;
		delete unsigned_;
	}

	WASMDiviINode::WASMDiviINode(const Immediate &imm_, const std::string *rs_, const std::string *rd_,
	                             bool is_unsigned):
		WASMInstructionNode(WASM_DIVIINODE), IType(rs_, rd_, imm_), HasUnsigned(is_unsigned) {}

	WASMInstructionNode * WASMDiviINode::copy() const {
		return (new WASMDiviINode(imm, rs, rd, isUnsigned))->absorb(*this);
	}

	std::string WASMDiviINode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + colorize(imm) + dim(" / ") + cyan(*rs) + dim(" -> ") + cyan(*rd)
			+ (isUnsigned? " /u" : "");
	}

	WASMDiviINode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + toString(imm) + " / " + *rs + " -> " + *rd
			+ (isUnsigned? " /u" : "");
	}

	WASMLuiNode::WASMLuiNode(ASTNode *imm_, ASTNode *rd_):
	WASMInstructionNode(WASM_LUINODE), IType(nullptr, rd_, imm_) {
		delete imm_;
		delete rd_;
	}

	WASMLuiNode::WASMLuiNode(const Immediate &imm_, const std::string *rd_):
		WASMInstructionNode(WASM_LUINODE), IType(nullptr, rd_, imm_) {}

	std::string WASMLuiNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + "lui" + dim(": ") + colorize(imm) + dim(" -> ") + cyan(*rd);
	}

	WASMLuiNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "lui: " + toString(imm) + " -> " + *rd;
	}

	WASMStackNode::WASMStackNode(ASTNode *reg, bool is_push):
	WASMInstructionNode(WASM_STACKNODE), RType(reg, nullptr, reg), isPush(is_push) {
		delete reg;
	}

	WASMStackNode::WASMStackNode(const std::string *reg, bool is_push):
		WASMInstructionNode(WASM_STACKNODE), RType(reg, nullptr, reg), isPush(is_push) {}

	std::string WASMStackNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim(isPush? "[" : "]") + " " + cyan(*rs);
	}

	WASMStackNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(isPush? "[" : "]") + " " + *rs;
	}

	WASMNopNode::WASMNopNode(): WASMInstructionNode(WASM_NOPNODE) {}

	std::string WASMNopNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim("<>");
	}

	WASMNopNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "<>";
	}

	WASMIntINode::WASMIntINode(ASTNode *imm_): WASMInstructionNode(WASM_INTINODE), IType(imm_) {
		delete imm_;
	}

	WASMIntINode::WASMIntINode(const Immediate &imm_): WASMInstructionNode(WASM_INTINODE), IType(imm_) {}

	std::string WASMIntINode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%int") + " " + colorize(imm);
	}

	WASMIntINode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%int " + toString(imm);
	}

	WASMRitINode::WASMRitINode(ASTNode *imm_): WASMInstructionNode(WASM_RITINODE), IType(imm_) {
		delete imm_;
	}

	WASMRitINode::WASMRitINode(const Immediate &imm_):
		WASMInstructionNode(WASM_RITINODE), IType(nullptr, nullptr, imm_) {}

	std::string WASMRitINode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%rit") + " " + colorize(imm);
	}

	WASMRitINode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%rit " + toString(imm);
	}

	WASMTimeINode::WASMTimeINode(ASTNode *imm_): WASMInstructionNode(WASM_TIMEINODE), IType(imm_) {
		delete imm_;
	}

	WASMTimeINode::WASMTimeINode(const Immediate &imm_): WASMInstructionNode(WASM_TIMEINODE), IType(imm_) {}

	std::string WASMTimeINode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%time") + " " + colorize(imm);
	}

	WASMTimeINode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%time " + toString(imm);
	}

	WASMTimeRNode::WASMTimeRNode(ASTNode *rs_): WASMInstructionNode(WASM_TIMERNODE), RType(rs_, nullptr, nullptr) {
		delete rs_;
	}

	WASMTimeRNode::WASMTimeRNode(const std::string *rs_):
		WASMInstructionNode(WASM_TIMERNODE), RType(rs_, nullptr, nullptr) {}

	std::string WASMTimeRNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%time") + " " + cyan(*rs);
	}

	WASMTimeRNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%time " + *rs;
	}

	WASMRingINode::WASMRingINode(ASTNode *imm_): WASMInstructionNode(WASM_RINGINODE), IType(imm_) {
		delete imm_;
	}

	WASMRingINode::WASMRingINode(const Immediate &imm_): WASMInstructionNode(WASM_RINGINODE), IType(imm_) {}

	std::string WASMRingINode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%ring") + " " + colorize(imm);
	}

	WASMRingINode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%ring " + toString(imm);
	}

	WASMRingRNode::WASMRingRNode(ASTNode *rs_): WASMInstructionNode(WASM_RINGRNODE), RType(rs_, nullptr, nullptr) {
		delete rs_;
	}

	WASMRingRNode::WASMRingRNode(const std::string *rs_):
		WASMInstructionNode(WASM_RINGRNODE), RType(rs_, nullptr, nullptr) {}

	std::string WASMRingRNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%ring") + " " + cyan(*rs);
	}

	WASMRingRNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%ring " + *rs;
	}

	WASMPrintNode::WASMPrintNode(ASTNode *rs_, ASTNode *type_):
	WASMInstructionNode(WASM_PRINTNODE), RType(rs_, nullptr, nullptr) {
		delete rs_;
		const std::string &typestr = *type_->lexerInfo;
		if (typestr == "prx") {
			type = PrintType::Hex;
		} else if (typestr == "prd" || typestr == "p") {
			type = PrintType::Dec;
		} else if (typestr == "prc") {
			type = PrintType::Char;
		} else if (typestr == "print") {
			type = PrintType::Full;
		} else if (typestr == "prb") {
			type = PrintType::Bin;
		} else {
			wasmerror("Invalid print type: " + typestr);
			type = PrintType::Full;
		}
		delete type_;
	}

	WASMPrintNode::WASMPrintNode(const std::string *rs_, PrintType type_):
		WASMInstructionNode(WASM_PRINTNODE), RType(rs_, nullptr, nullptr), type(type_) {}

	Funct WASMPrintNode::getFunct() const {
		switch (type) {
			case PrintType::Hex:  return FUNCTS.at("prx");
			case PrintType::Dec:  return FUNCTS.at("prd");
			case PrintType::Char: return FUNCTS.at("prc");
			case PrintType::Full: return FUNCTS.at("printr");
			case PrintType::Bin:  return FUNCTS.at("prb");
			default: throw std::runtime_error("Invalid print type: " + std::to_string(static_cast<int>(type)));
		}
	}

	std::string WASMPrintNode::debugExtra() const {
		const std::string base = WASMInstructionNode::debugExtra();
		switch (type) {
			case PrintType::Hex:  return base + "<" + blue("prx")   + " " + cyan(*rs) + ">";
			case PrintType::Dec:  return base + "<" + blue("prd")   + " " + cyan(*rs) + ">";
			case PrintType::Char: return base + "<" + blue("prc")   + " " + cyan(*rs) + ">";
			case PrintType::Full: return base + "<" + blue("print") + " " + cyan(*rs) + ">";
			case PrintType::Bin:  return base + "<" + blue("prb")   + " " + cyan(*rs) + ">";
			default:
				return base + red("???");
		}
	}

	WASMPrintNode::operator std::string() const {
		const std::string base = WASMInstructionNode::operator std::string();
		switch (type) {
			case PrintType::Hex:  return  base + "<prx "   + *rs + ">";
			case PrintType::Dec:  return  base + "<prd "   + *rs + ">";
			case PrintType::Char: return  base + "<prc "   + *rs + ">";
			case PrintType::Full: return  base + "<print " + *rs + ">";
			case PrintType::Bin:  return  base + "<prb "   + *rs + ">";
			default:
				return base + "???";
		}
	}

	WASMHaltNode::WASMHaltNode(): WASMInstructionNode(WASM_HALTNODE) {}

	std::string WASMHaltNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + "<" + blue("halt") + ">";
	}

	WASMHaltNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "<halt>";
	}

	WASMSleepRNode::WASMSleepRNode(ASTNode *rs_): WASMInstructionNode(WASM_SLEEPRNODE), RType(rs_, nullptr, nullptr) {
		delete rs_;
	}

	WASMSleepRNode::WASMSleepRNode(const std::string *rs_):
		WASMInstructionNode(WASM_SLEEPRNODE), RType(rs_, nullptr, nullptr) {}

	std::string WASMSleepRNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + "<" + blue("sleep") + " " + cyan(*rs) + ">";
	}

	WASMSleepRNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "<sleep " + *rs + ">";
	}

	WASMPageNode::WASMPageNode(bool on_): WASMInstructionNode(WASM_PAGENODE), on(on_) {}

	std::string WASMPageNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%page") + " " + (on? "on" : "off");
	}

	WASMPageNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%page " + std::string(on? "on" : "off");
	}

	WASMSetptRNode::WASMSetptRNode(ASTNode *rs_): WASMInstructionNode(WASM_SETPTRNODE), RType(rs_, nullptr, nullptr) {
		delete rs_;
	}

	WASMSetptRNode::WASMSetptRNode(const std::string *rs_):
		WASMInstructionNode(WASM_SETPTRNODE), RType(rs_, nullptr, nullptr) {}

	std::string WASMSetptRNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%setpt") + " " + cyan(*rs);
	}

	WASMSetptRNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%setpt " + *rs;
	}

	WASMMvNode::WASMMvNode(ASTNode *rs_, ASTNode *rd_): WASMMvNode(rs_->lexerInfo, rd_->lexerInfo) {
		delete rs_;
		delete rd_;
	}

	WASMMvNode::WASMMvNode(const std::string *rs_, const std::string *rd_):
		WASMInstructionNode(WASM_MVNODE), rs(rs_), rd(rd_) {}

	std::string WASMMvNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(*rs) + dim(" -> ") + cyan(*rd);
	}

	WASMMvNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + *rs + " -> " + *rd;
	}

	WASMSvpgNode::WASMSvpgNode(ASTNode *rd_): WASMInstructionNode(WASM_SVPGNODE), RType(nullptr, nullptr, rd_) {
		delete rd_;
	}

	WASMSvpgNode::WASMSvpgNode(const std::string *rd_):
		WASMInstructionNode(WASM_SVPGNODE), RType(nullptr, nullptr, rd_) {}

	std::string WASMSvpgNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%page") + dim(" -> ") + cyan(*rd);
	}

	WASMSvpgNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%page -> " + *rd;
	}

	WASMQueryNode::WASMQueryNode(QueryType type_, ASTNode *rd_):
	WASMInstructionNode(WASM_QUERYNODE), RType(nullptr, nullptr, rd_), type(type_) {
		delete rd_;
	}

	WASMQueryNode::WASMQueryNode(QueryType type_, const std::string *rd_):
		WASMInstructionNode(WASM_QUERYNODE), RType(nullptr, nullptr, rd_), type(type_) {}

	Funct WASMQueryNode::getFunct() const {
		switch (type) {
			case QueryType::Memory: return FUNCTS.at("qm");
			default: throw std::runtime_error("Invalid query type: " + std::to_string(static_cast<int>(type)));
		}
	}

	std::string WASMQueryNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + "? " + blue(query_map.at(type)) + dim(" -> ") + cyan(*rd);
	}

	WASMQueryNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "? mem -> " + *rd;
	}

	WASMPseudoPrintNode::WASMPseudoPrintNode(ASTNode *imm_):
	WASMInstructionNode(WASM_PSEUDOPRINTNODE), HasImmediate(getImmediate(imm_)) {
		delete imm_;
	}

	WASMPseudoPrintNode::WASMPseudoPrintNode(const Immediate &imm_):
		WASMInstructionNode(WASM_PSEUDOPRINTNODE), HasImmediate(imm_) {}

	size_t WASMPseudoPrintNode::expandedSize() const {
		if (std::holds_alternative<char>(imm))
			return 2;
		throw std::runtime_error("Invalid WASMPseudoPrintNode immediate type: expected char");
	}

	std::string WASMPseudoPrintNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + "<" + blue("p") + " " + colorize(imm) + ">";
	}

	WASMPseudoPrintNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "<p " + toString(imm) + ">";
	}

	WASMStringPrintNode::WASMStringPrintNode(ASTNode *string_):
	WASMInstructionNode(WASM_STRINGPRINTNODE), string(StringSet::intern(string_->unquote())) {
		delete string_;
	}

	WASMStringPrintNode::WASMStringPrintNode(const std::string *string_):
		WASMInstructionNode(WASM_STRINGPRINTNODE), string(string_) {}

	size_t WASMStringPrintNode::expandedSize() const {
		size_t out = 0;
		char last_char = string->front() - 1;
		for (char ch: *string) {
			if (ch != last_char) {
				++out;
				last_char = ch;
			}

			++out;
		}
		return out;
	}

	std::string WASMStringPrintNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + "<" + blue("p") + " \"" + *string + "\">";
	}

	WASMStringPrintNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "<p \"" + *string + "\">";
	}

	WASMRestNode::WASMRestNode(): WASMInstructionNode(WASM_RESTNODE) {}

	std::string WASMRestNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + "<" + blue("rest") + ">";
	}

	WASMRestNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "<rest>";
	}

	WASMIONode::WASMIONode(const std::string *ident_): WASMInstructionNode(WASM_IONODE), ident(ident_) {}

	std::string WASMIONode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + (ident? "<" + blue("io") + " " + *ident + ">" : "<" + blue("io") +
			">");
	}

	WASMIONode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + (ident? "<io " + *ident + ">" : "<io>");
	}

	WASMInterruptsNode::WASMInterruptsNode(bool enable_): WASMInstructionNode(WASM_INTERRUPTSNODE), enable(enable_) {}

	std::string WASMInterruptsNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue(enable? "%ei" : "%di");
	}

	WASMInterruptsNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + (enable? "%ei" : "%di");
	}

	RNode * makeSeq(const std::string *rs, const std::string *rt, const std::string *rd, int bang) {
		static const auto deq = StringSet::intern("==");
		RNode *out = new RNode(rs, deq, rt, rd, WASMTOK_DEQ, false);
		out->setBang(bang);
		return out;
	}
}
