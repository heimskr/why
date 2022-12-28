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

	RNode::RNode(ASTNode *rs_, ASTNode *oper_, ASTNode *rt_, ASTNode *rd_):
	WASMInstructionNode(WASM_RNODE), RType(rs_, rt_, rd_), HasOper(oper_) {
		locate(oper_);
		if (rs_ != rd_)
			delete rs_;
		if (oper_ != rt_)
			delete rt_;
		delete oper_;
		delete rd_;
	}

	RNode::RNode(const TypedReg &rs_, const std::string *oper_, const TypedReg &rt_, const TypedReg &rd_,
	             int oper_token):
		WASMInstructionNode(WASM_RNODE), RType(rs_, rt_, rd_), HasOper(oper_token, oper_) {}

	WASMInstructionNode * RNode::copy() const {
		return (new RNode(rs, oper, rt, rd, operToken))->absorb(*this);
	}

	Opcode RNode::getOpcode() const {
		try {
			return OPCODES.at(TOKEN_INSTRUCTIONS_R.at(operToken));
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

		try {
			return FUNCTS.at(*instruction);
		} catch (const std::out_of_range &) {
			debug();
			error() << "Couldn't find function for token " << operToken << " (" << *oper << ")\n";
			throw;
		}
	}

	std::string RNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(rs) + " " + dim(*oper) + " " + cyan(rt) + dim(" -> ")
			+ cyan(rd);
	}

	RNode::operator std::string() const {
		const std::string base = WASMInstructionNode::operator std::string();
		if (*oper == "!" || *oper == "~")
			return base + *oper + std::string(rs) + " -> " + std::string(rd);
		return base + std::string(rs) + " " + *oper + " " + std::string(rt) + " -> " + std::string(rd);
	}

	INode::INode(ASTNode *rs_, ASTNode *oper_, ASTNode *imm_, ASTNode *rd_):
	WASMInstructionNode(WASM_INODE), IType(rs_, rd_, imm_), HasOper(oper_) {
		if (rs_ != rd_)
			delete rs_;
		delete oper_;
		delete imm_;
		delete rd_;
	}

	INode::INode(const TypedReg &rs_, const std::string *oper_, const TypedImmediate &imm_, const TypedReg &rd_,
	             int oper_token):
		WASMInstructionNode(WASM_INODE), IType(rs_, rd_, imm_), HasOper(oper_token, oper_) {}

	Opcode INode::getOpcode() const {
		try {
			return OPCODES.at(TOKEN_INSTRUCTIONS_I.at(operToken));
		} catch (const std::out_of_range &) {
			debug();
			std::cerr << "Couldn't find opcode for token " << operToken << " (" << *oper << ")\n";
			throw;
		}
	}

	WASMInstructionNode * INode::copy() const {
		return (new INode(rs, oper, imm, rd, operToken))->absorb(*this);
	}

	std::string INode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(rs) + " " + dim(*oper) + " " + colorize(imm) + dim(" -> ")
			+ cyan(rd);
	}

	INode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(rs) + " " + *oper + " " + toString(imm) +
			" -> " + std::string(rd);
	}

	WASMMemoryNode::WASMMemoryNode(int sym, ASTNode *rs_, ASTNode *rd_):
	WASMInstructionNode(sym), RType(rs_, {}, rd_) {
		delete rs_;
		delete rd_;
	}

	WASMMemoryNode::WASMMemoryNode(int sym, const TypedReg &rs_, const TypedReg &rd_):
		WASMInstructionNode(sym), RType(rs_, {}, rd_) {}

	WASMCopyNode::WASMCopyNode(ASTNode *rs_, ASTNode *rd_):
		WASMMemoryNode(WASM_COPYNODE, rs_, rd_) {}

	WASMCopyNode::WASMCopyNode(const TypedReg &rs_, const TypedReg &rd_):
		WASMMemoryNode(WASM_COPYNODE, rs_, rd_) {}

	WASMInstructionNode * WASMCopyNode::copy() const {
		return (new WASMCopyNode(rs, rd))->absorb(*this);
	}

	std::string WASMCopyNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim("[") + cyan(rs) + dim("] -> [") + cyan(rd) + dim("]");
	}

	WASMCopyNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "[" + std::string(rs) + "] -> [" + std::string(rd) + "]";
	}

	WASMLoadNode::WASMLoadNode(ASTNode *rs_, ASTNode *rd_):
		WASMMemoryNode(WASM_LOADNODE, rs_, rd_) {}

	WASMLoadNode::WASMLoadNode(const TypedReg &rs_, const TypedReg &rd_):
		WASMMemoryNode(WASM_LOADNODE, rs_, rd_) {}

	WASMInstructionNode * WASMLoadNode::copy() const {
		return (new WASMLoadNode(rs, rd))->absorb(*this);
	}

	std::string WASMLoadNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim("[") + cyan(rs) + dim("] -> ") + cyan(rd);
	}

	WASMLoadNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "[" + std::string(rs) + "] -> " + std::string(rd);
	}

	WASMStoreNode::WASMStoreNode(ASTNode *rs_, ASTNode *rd_):
		WASMMemoryNode(WASM_STORENODE, rs_, rd_) {}

	WASMStoreNode::WASMStoreNode(const TypedReg &rs_, const TypedReg &rd_):
		WASMMemoryNode(WASM_STORENODE, rs_, rd_) {}

	WASMInstructionNode * WASMStoreNode::copy() const {
		return (new WASMStoreNode(rs, rd))->absorb(*this);
	}

	std::string WASMStoreNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(rs) + dim(" -> [") + cyan(rd) + dim("]");
	}

	WASMStoreNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(rs) + " -> [" + std::string(rd) + "]";
	}

	WASMSetNode::WASMSetNode(ASTNode *imm_, ASTNode *rd_):
	WASMSetNode(getImmediate(imm_), TypedReg(rd_)) {
		delete imm_;
		delete rd_;
	}

	WASMSetNode::WASMSetNode(const TypedImmediate &imm_, const TypedReg &rd_):
		WASMInstructionNode(WASM_SETNODE), IType({}, rd_, imm_) {}

	WASMInstructionNode * WASMSetNode::copy() const {
		return (new WASMSetNode(imm, rd))->absorb(*this);
	}

	std::string WASMSetNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + colorize(imm) + dim(" -> ") + cyan(rd);
	}

	WASMSetNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + toString(imm) + " -> " + std::string(rd);
	}

	WASMLiNode::WASMLiNode(ASTNode *imm_, ASTNode *rd_):
	WASMLiNode(getImmediate(imm_), TypedReg(rd_)) {
		delete imm_;
		delete rd_;
	}

	WASMLiNode::WASMLiNode(const TypedImmediate &imm_, const TypedReg &rd_):
		WASMInstructionNode(WASM_LINODE), IType({}, rd_, imm_) {}

	WASMInstructionNode * WASMLiNode::copy() const {
		return (new WASMLiNode(imm, rd))->absorb(*this);
	}

	std::string WASMLiNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim("[") + colorize(imm) + dim("] -> ") + cyan(rd);
	}

	WASMLiNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "[" + toString(imm) + "] -> " + std::string(rd);
	}

	WASMSiNode::WASMSiNode(ASTNode *rs_, ASTNode *imm_):
	WASMInstructionNode(WASM_SINODE), IType(rs_, {}, imm_) {
		delete rs_;
		delete imm_;
	}

	WASMSiNode::WASMSiNode(const TypedImmediate &imm_, const TypedReg &rs_):
		WASMInstructionNode(WASM_SINODE), IType(rs_, {}, imm_) {}

	WASMInstructionNode * WASMSiNode::copy() const {
		return (new WASMSiNode(imm, rs))->absorb(*this);
	}

	std::string WASMSiNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(rs) + dim(" -> [") + colorize(imm) + dim("]");
	}

	WASMSiNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(rs) + " -> [" + toString(imm) + "]";
	}

	WASMLniNode::WASMLniNode(ASTNode *imm_, ASTNode *rd_): WASMLiNode(imm_, rd_) {
		symbol = WASM_LNINODE;
	}

	WASMLniNode::WASMLniNode(const TypedImmediate &imm_, const TypedReg &rd_):
	WASMLiNode(imm_, rd_) {
		symbol = WASM_LNINODE;
	}

	WASMInstructionNode * WASMLniNode::copy() const {
		return (new WASMLniNode(imm, rd))->absorb(*this);
	}

	std::string WASMLniNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim("[") + colorize(imm) + dim("] -> [") + cyan(rd) + dim("]");
	}

	WASMLniNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "[" + toString(imm) + "] -> [" + std::string(rd) + "]";
	}

	WASMCmpNode::WASMCmpNode(ASTNode *rs_, ASTNode *rt_): WASMInstructionNode(WASM_CMPNODE), RType(rs_, rt_, nullptr) {
		delete rs_;
		delete rt_;
	}

	WASMCmpNode::WASMCmpNode(const TypedReg &rs_, const TypedReg &rt_):
		WASMInstructionNode(WASM_CMPNODE), RType(rs_, rt_, {}) {}

	std::string WASMCmpNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(rs) + dim(" ~ ") + cyan(rt);
	}

	WASMCmpNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(rs) + " ~ " + std::string(rt);
	}

	WASMCmpiNode::WASMCmpiNode(ASTNode *rs_, ASTNode *imm_):
	WASMInstructionNode(WASM_CMPINODE), IType(rs_, nullptr, imm_) {
		delete rs_;
		delete imm_;
	}

	WASMCmpiNode::WASMCmpiNode(const TypedReg &rs_, const TypedImmediate &imm_):
		WASMInstructionNode(WASM_CMPINODE), IType(rs_, {}, imm_) {}

	std::string WASMCmpiNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(rs) + dim(" ~ ") + colorize(imm);
	}

	WASMCmpiNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(rs) + " ~ " + toString(imm);
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

	WASMSelNode::WASMSelNode(const TypedReg &rs_, Condition cond, const TypedReg &rt_, const TypedReg &rd_):
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
		return WASMInstructionNode::debugExtra() + dim("[") + cyan(rs) + " " + dim(oper_) + " " + cyan(rt)
			+ dim("] -> ") + cyan(rd);
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
		return WASMInstructionNode::operator std::string() + "[" + std::string(rs) + " " + oper_ + " " + std::string(rt)
			+ "] -> " + std::string(rd);
	}

	WASMJNode::WASMJNode(ASTNode *cond, ASTNode *colons, ASTNode *addr):
	WASMInstructionNode(WASM_JNODE), JType(cond, colons, addr, nullptr) {
		delete addr;
		delete colons;
		delete cond;
	}

	WASMJNode::WASMJNode(const TypedImmediate &addr, bool link_, Condition cond):
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
	JType(j? j->condition : Condition::None, j? j->link : false, j? j->imm : TypedImmediate(OperandType::VOID_PTR, 0),
	TypedReg(rs_)) {
		if (j == nullptr) {
			wasmerror("No WASMJNode found in jc instruction");
		} else {
			if (j->condition != Condition::None)
				wasmerror("Conditions specified for jc instruction will be ignored");
			delete j;
		}
		delete rs_;
	}

	WASMJcNode::WASMJcNode(const TypedImmediate &imm_, bool link_, const TypedReg &rs_):
		WASMInstructionNode(WASM_JCNODE), JType(Condition::None, link_, imm_, rs_) {}

	std::string WASMJcNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim(link? "::" : ":") + " " + colorize(imm) + red(" if ")
			+ cyan(rs);
	}

	WASMJcNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(link? "::" : ":") + " " + toString(imm)
			+ " if " + std::string(rs);
	}

	WASMJrNode::WASMJrNode(ASTNode *cond, ASTNode *colons, ASTNode *rd_):
	WASMInstructionNode(WASM_JRNODE), RType({}, {}, TypedReg(rd_)),
	HasCondition(cond), HasLink(colons) {
		delete colons;
		delete rd_;
		delete cond;
	}

	WASMJrNode::WASMJrNode(Condition condition_, bool link_, const TypedReg &rd_):
		WASMInstructionNode(WASM_JRNODE), RType({}, {}, rd_), HasCondition(condition_), HasLink(link_) {}

	std::string WASMJrNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim(conditionString(condition) + std::string(link? "::" : ":")) + " "
			+ cyan(rd);
	}

	WASMJrNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + conditionString(condition) + std::string(link? "::" : ":")
			+ " " + std::string(rd);
	}

	WASMJrcNode::WASMJrcNode(WASMJrNode *jr, ASTNode *rs_):
	WASMInstructionNode(WASM_JRCNODE), RType(TypedReg(rs_), {}, jr? jr->rd : TypedReg()),
	HasLink(jr? jr->link : false) {
		if (jr == nullptr) {
			wasmerror("No WASMJrNode found in jr(l)c instruction");
		} else {
			if (jr->condition != Condition::None)
				wasmerror("Conditions specified for jr(l)c instruction will be ignored");
			delete jr;
		}
		delete rs_;
	}

	WASMJrcNode::WASMJrcNode(bool link_, const TypedReg &rs_, const TypedReg &rd_):
		WASMInstructionNode(WASM_JRCNODE), RType(rs_, {}, rd_), HasLink(link_) {}

	std::string WASMJrcNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim(link? "::" : ":") + " " + cyan(rd) + red(" if ") + cyan(rs);
	}

	WASMJrcNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(link? "::" : ":") + " " + std::string(rd) +
			" if " + std::string(rs);
	}

	WASMJeqNode::WASMJeqNode(WASMJNode *j, ASTNode *rs_, ASTNode *rt_):
	WASMInstructionNode(WASM_JEQNODE), link(j? j->link : false), addr(j? j->imm : TypedImmediate(OperandType::VOID_PTR,
	0)), rt(getEither(rt_)), rs(rs_) {
		if (j == nullptr) {
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
	rs(rs_) {
		if (!jr) {
			wasmerror("No WASMJrNode found in jeq instruction");
		} else {
			if (jr->condition != Condition::None)
				wasmerror("Conditions specified for jeq instruction will be ignored");
			delete jr;
		}
		delete rs_;
	}

	WASMJeqNode::WASMJeqNode(const Either &addr_, bool link_, const TypedReg &rs_, const Either &rt_):
		WASMInstructionNode(WASM_JEQNODE), link(link_), addr(addr_), rt(rt_), rs(rs_) {}

	size_t WASMJeqNode::expandedSize() const {
		return !std::holds_alternative<TypedReg>(addr) || std::holds_alternative<TypedReg>(rt)? 2 : 3;
	}

	std::string WASMJeqNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim(link? "::" : ":") + " " + colorize(addr) + red(" if ")
			+ cyan(rs) + " == " + colorize(rt);
	}

	WASMJeqNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(link? "::" : ":") + " " + toString(addr)
			+ " if " + std::string(rs) + " == " + toString(rt);
	}

	WASMSizedStackNode::WASMSizedStackNode(ASTNode *size, ASTNode *rs_, bool is_push):
	WASMInstructionNode(WASM_SIZEDSTACKNODE), IType(rs_, nullptr, size), isPush(is_push) {
		delete size;
		delete rs_;
	}

	WASMSizedStackNode::WASMSizedStackNode(int size, const TypedReg &rs_, bool is_push):
		WASMInstructionNode(WASM_SIZEDSTACKNODE), IType(rs_, {}, TypedImmediate(OperandType::ULONG, size)),
		isPush(is_push) {}

	WASMInstructionNode * WASMSizedStackNode::copy() const {
		return (new WASMSizedStackNode(imm.get<int>(), rs, isPush))->absorb(*this);
	}

	std::string WASMSizedStackNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim(std::string(isPush? "[" : "]") + ":"
			+ std::to_string(imm.get<int>())) + " " + cyan(rs);
	}

	WASMSizedStackNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(isPush? "[" : "]") + ":"
			+ std::to_string(imm.get<int>()) + " " + std::string(rs);
	}

	WASMMultRNode::WASMMultRNode(ASTNode *rs_, ASTNode *rt_):
	WASMInstructionNode(WASM_MULTRNODE), RType(rs_, rt_, nullptr) {
		delete rs_;
		delete rt_;
	}

	WASMMultRNode::WASMMultRNode(const TypedReg &rs_, const TypedReg &rt_):
		WASMInstructionNode(WASM_MULTRNODE), RType(rs_, rt_, {}) {}

	std::string WASMMultRNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(rs) + dim(" * ") + cyan(rt);
	}

	WASMMultRNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(rs) + " * " + std::string(rt);
	}

	WASMMultINode::WASMMultINode(ASTNode *rs_, ASTNode *imm_):
	WASMInstructionNode(WASM_MULTINODE), IType(rs_, nullptr, imm_) {
		delete rs_;
		delete imm_;
	}

	WASMMultINode::WASMMultINode(const TypedReg &rs_, const TypedImmediate &imm_):
		WASMInstructionNode(WASM_MULTINODE), IType(rs_, {}, imm_) {}

	std::string WASMMultINode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(rs) + dim(" * ") + colorize(imm);
	};

	WASMMultINode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(rs) + " * " + toString(imm);
	}

	WASMDiviINode::WASMDiviINode(ASTNode *imm_, ASTNode *rs_, ASTNode *rd_):
	WASMInstructionNode(WASM_DIVIINODE), IType(rs_, rd_, imm_) {
		delete rs_;
		delete rd_;
		delete imm_;
	}

	WASMDiviINode::WASMDiviINode(const TypedImmediate &imm_, const TypedReg &rs_, const TypedReg &rd_):
		WASMInstructionNode(WASM_DIVIINODE), IType(rs_, rd_, imm_) {}

	WASMInstructionNode * WASMDiviINode::copy() const {
		return (new WASMDiviINode(imm, rs, rd))->absorb(*this);
	}

	std::string WASMDiviINode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + colorize(imm) + dim(" / ") + cyan(rs) + dim(" -> ") + cyan(rd);
	}

	WASMDiviINode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + toString(imm) + " / " + std::string(rs) + " -> " +
			std::string(rd);
	}

	WASMLuiNode::WASMLuiNode(ASTNode *imm_, ASTNode *rd_):
	WASMInstructionNode(WASM_LUINODE), IType(nullptr, rd_, imm_) {
		delete imm_;
		delete rd_;
	}

	WASMLuiNode::WASMLuiNode(const TypedImmediate &imm_, const TypedReg &rd_):
		WASMInstructionNode(WASM_LUINODE), IType({}, rd_, imm_) {}

	std::string WASMLuiNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + "lui" + dim(": ") + colorize(imm) + dim(" -> ") + cyan(rd);
	}

	WASMLuiNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "lui: " + toString(imm) + " -> " + std::string(rd);
	}

	WASMStackNode::WASMStackNode(ASTNode *reg, bool is_push):
	WASMInstructionNode(WASM_STACKNODE), RType(reg, nullptr, reg), isPush(is_push) {
		delete reg;
	}

	WASMStackNode::WASMStackNode(const TypedReg &reg, bool is_push):
		WASMInstructionNode(WASM_STACKNODE), RType(reg, {}, reg), isPush(is_push) {}

	std::string WASMStackNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim(isPush? "[" : "]") + " " + cyan(rs);
	}

	WASMStackNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(isPush? "[" : "]") + " " + std::string(rs);
	}

	WASMTypedStackNode::WASMTypedStackNode(ASTNode *reg, bool is_push):
	WASMInstructionNode(WASM_TYPEDSTACKNODE), RType(reg, nullptr, reg), isPush(is_push) {
		delete reg;
	}

	WASMTypedStackNode::WASMTypedStackNode(const TypedReg &reg, bool is_push):
		WASMInstructionNode(WASM_TYPEDSTACKNODE), RType(reg, {}, reg), isPush(is_push) {}

	std::string WASMTypedStackNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim(isPush? "#[" : "#]") + " " + cyan(rs);
	}

	WASMTypedStackNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(isPush? "#[" : "#]") + " " + std::string(rs);
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

	WASMIntINode::WASMIntINode(const TypedImmediate &imm_): WASMInstructionNode(WASM_INTINODE), IType(imm_) {}

	std::string WASMIntINode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%int") + " " + colorize(imm);
	}

	WASMIntINode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%int " + toString(imm);
	}

	WASMRitINode::WASMRitINode(ASTNode *imm_): WASMInstructionNode(WASM_RITINODE), IType(imm_) {
		delete imm_;
	}

	WASMRitINode::WASMRitINode(const TypedImmediate &imm_):
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

	WASMTimeINode::WASMTimeINode(const TypedImmediate &imm_): WASMInstructionNode(WASM_TIMEINODE), IType(imm_) {}

	std::string WASMTimeINode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%time") + " " + colorize(imm);
	}

	WASMTimeINode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%time " + toString(imm);
	}

	WASMTimeRNode::WASMTimeRNode(ASTNode *rs_): WASMInstructionNode(WASM_TIMERNODE), RType(rs_, nullptr, nullptr) {
		delete rs_;
	}

	WASMTimeRNode::WASMTimeRNode(const TypedReg &rs_):
		WASMInstructionNode(WASM_TIMERNODE), RType(rs_, {}, {}) {}

	std::string WASMTimeRNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%time") + " " + cyan(rs);
	}

	WASMTimeRNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%time " + std::string(rs);
	}

	WASMRingINode::WASMRingINode(ASTNode *imm_): WASMInstructionNode(WASM_RINGINODE), IType(imm_) {
		delete imm_;
	}

	WASMRingINode::WASMRingINode(const TypedImmediate &imm_): WASMInstructionNode(WASM_RINGINODE), IType(imm_) {}

	std::string WASMRingINode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%ring") + " " + colorize(imm);
	}

	WASMRingINode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%ring " + toString(imm);
	}

	WASMRingRNode::WASMRingRNode(ASTNode *rs_): WASMInstructionNode(WASM_RINGRNODE), RType(rs_, nullptr, nullptr) {
		delete rs_;
	}

	WASMRingRNode::WASMRingRNode(const TypedReg &rs_):
		WASMInstructionNode(WASM_RINGRNODE), RType(rs_, {}, {}) {}

	std::string WASMRingRNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%ring") + " " + cyan(rs);
	}

	WASMRingRNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%ring " + std::string(rs);
	}

	WASMSvringNode::WASMSvringNode(ASTNode *rd_): WASMInstructionNode(WASM_SVRINGNODE), RType(nullptr, nullptr, rd_) {
		delete rd_;
	}

	WASMSvringNode::WASMSvringNode(const TypedReg &rd_):
		WASMInstructionNode(WASM_SVRINGNODE), RType({}, {}, rd_) {}

	std::string WASMSvringNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%ring") + dim(" -> ") + cyan(rd);
	}

	WASMSvringNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%ring -> " + std::string(rd);
	}

	WASMSvtimeNode::WASMSvtimeNode(ASTNode *rd_): WASMInstructionNode(WASM_SVTIMENODE), RType(nullptr, nullptr, rd_) {
		delete rd_;
	}

	WASMSvtimeNode::WASMSvtimeNode(const TypedReg &rd_):
		WASMInstructionNode(WASM_SVTIMENODE), RType({}, {}, rd_) {}

	std::string WASMSvtimeNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%time") + dim(" -> ") + cyan(rd);
	}

	WASMSvtimeNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%time -> " + std::string(rd);
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

	WASMPrintNode::WASMPrintNode(const TypedReg &rs_, PrintType type_):
		WASMInstructionNode(WASM_PRINTNODE), RType(rs_, {}, {}), type(type_) {}

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
			case PrintType::Hex:  return base + "<" + blue("prx")   + " " + cyan(rs) + ">";
			case PrintType::Dec:  return base + "<" + blue("prd")   + " " + cyan(rs) + ">";
			case PrintType::Char: return base + "<" + blue("prc")   + " " + cyan(rs) + ">";
			case PrintType::Full: return base + "<" + blue("print") + " " + cyan(rs) + ">";
			case PrintType::Bin:  return base + "<" + blue("prb")   + " " + cyan(rs) + ">";
			default:
				return base + red("???");
		}
	}

	WASMPrintNode::operator std::string() const {
		const std::string base = WASMInstructionNode::operator std::string();
		switch (type) {
			case PrintType::Hex:  return  base + "<prx "   + std::string(rs) + ">";
			case PrintType::Dec:  return  base + "<prd "   + std::string(rs) + ">";
			case PrintType::Char: return  base + "<prc "   + std::string(rs) + ">";
			case PrintType::Full: return  base + "<print " + std::string(rs) + ">";
			case PrintType::Bin:  return  base + "<prb "   + std::string(rs) + ">";
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

	WASMSleepRNode::WASMSleepRNode(const TypedReg &rs_):
		WASMInstructionNode(WASM_SLEEPRNODE), RType(rs_, {}, {}) {}

	std::string WASMSleepRNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + "<" + blue("sleep") + " " + cyan(rs) + ">";
	}

	WASMSleepRNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "<sleep " + std::string(rs) + ">";
	}

	WASMPageNode::WASMPageNode(bool on_): WASMInstructionNode(WASM_PAGENODE), on(on_) {}

	std::string WASMPageNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%page") + " " + (on? "on" : "off");
	}

	WASMPageNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%page " + std::string(on? "on" : "off");
	}

	WASMSetptRNode::WASMSetptRNode(ASTNode *rs_, ASTNode *rt_):
	WASMInstructionNode(WASM_SETPTRNODE), RType(rs_, rt_, nullptr) {
		delete rs_;
		delete rt_;
	}

	WASMSetptRNode::WASMSetptRNode(const TypedReg &rs_, const TypedReg &rt_):
		WASMInstructionNode(WASM_SETPTRNODE), RType(rs_, rt_, {}) {}

	std::string WASMSetptRNode::debugExtra() const {
		if (!rt)
			return WASMInstructionNode::debugExtra() + blue("%setpt") + " " + cyan(rs);
		return WASMInstructionNode::debugExtra() + dim(":") + " " + blue("%setpt") + " " + cyan(rs) + " " + cyan(rt);
	}

	WASMSetptRNode::operator std::string() const {
		if (!rt)
			return WASMInstructionNode::operator std::string() + "%setpt " + std::string(rs);
		return WASMInstructionNode::operator std::string() + ": %setpt " + std::string(rs) + " " + std::string(rd);
	}

	WASMMvNode::WASMMvNode(ASTNode *rs_, ASTNode *rd_): WASMMvNode(TypedReg(rs_), TypedReg(rd_)) {
		delete rs_;
		delete rd_;
	}

	WASMMvNode::WASMMvNode(const TypedReg &rs_, const TypedReg &rd_):
		WASMInstructionNode(WASM_MVNODE), rs(rs_), rd(rd_) {}

	std::string WASMMvNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(rs) + dim(" -> ") + cyan(rd);
	}

	WASMMvNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(rs) + " -> " + std::string(rd);
	}

	WASMSvpgNode::WASMSvpgNode(ASTNode *rd_): WASMInstructionNode(WASM_SVPGNODE), RType(nullptr, nullptr, rd_) {
		delete rd_;
	}

	WASMSvpgNode::WASMSvpgNode(const TypedReg &rd_):
		WASMInstructionNode(WASM_SVPGNODE), RType({}, {}, rd_) {}

	std::string WASMSvpgNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%page") + dim(" -> ") + cyan(rd);
	}

	WASMSvpgNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%page -> " + std::string(rd);
	}

	WASMQueryNode::WASMQueryNode(QueryType type_, ASTNode *rd_):
	WASMInstructionNode(WASM_QUERYNODE), RType(nullptr, nullptr, rd_), type(type_) {
		delete rd_;
	}

	WASMQueryNode::WASMQueryNode(QueryType type_, const TypedReg &rd_):
		WASMInstructionNode(WASM_QUERYNODE), RType({}, {}, rd_), type(type_) {}

	Funct WASMQueryNode::getFunct() const {
		switch (type) {
			case QueryType::Memory: return FUNCTS.at("qm");
			default: throw std::runtime_error("Invalid query type: " + std::to_string(static_cast<int>(type)));
		}
	}

	std::string WASMQueryNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + "? " + blue(query_map.at(type)) + dim(" -> ") + cyan(rd);
	}

	WASMQueryNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "? mem -> " + std::string(rd);
	}

	WASMPseudoPrintNode::WASMPseudoPrintNode(ASTNode *imm_):
	WASMInstructionNode(WASM_PSEUDOPRINTNODE), HasImmediate({OperandType::UCHAR, getUntypedImmediate(imm_)}) {
		delete imm_;
	}

	WASMPseudoPrintNode::WASMPseudoPrintNode(const TypedImmediate &imm_):
		WASMInstructionNode(WASM_PSEUDOPRINTNODE), HasImmediate(imm_) {}

	size_t WASMPseudoPrintNode::expandedSize() const {
		if (imm.is<char>())
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

	WASMInverseNode::WASMInverseNode(ASTNode *imm_, ASTNode *rs_, ASTNode *rd_, Type type_):
		WASMInstructionNode(WASM_INVERSENODE), IType(rs_, rd_, imm_), type(type_) {}

	WASMInverseNode::WASMInverseNode(const TypedImmediate &imm_, const TypedReg &rs_, const TypedReg &rd_, Type type_):
		WASMInstructionNode(WASM_INVERSENODE), IType(rs_, rd_, imm_), type(type_) {}

	std::string WASMInverseNode::getOper() const {
		switch (type) {
			case Type::Sllii: return "<<";
			case Type::Srlii: return ">>>";
			case Type::Sraii: return ">>";
			default: throw std::runtime_error("Invalid WASMInverseNode::Type: " + std::to_string(int(type)));
		}
	}

	Opcode WASMInverseNode::getOpcode() const {
		switch (type) {
			case Type::Sllii: return OPCODES.at("sllii");
			case Type::Srlii: return OPCODES.at("srlii");
			case Type::Sraii: return OPCODES.at("sraii");
			default: throw std::runtime_error("Invalid WASMInverseNode::Type: " + std::to_string(int(type)));
		}
	}

	std::string WASMInverseNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + colorize(imm) + " " + dim(getOper()) + " " + cyan(rs) + dim(" -> ")
			+ cyan(rd);
	}

	WASMInverseNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + toString(imm) + " " + getOper() + " " + std::string(rs) +
			" -> " + std::string(rd);
	}

	RNode * makeSeq(const TypedReg &rs, const TypedReg &rt, const TypedReg &rd, int bang) {
		static const auto deq = StringSet::intern("==");
		RNode *out = new RNode(rs, deq, rt, rd, WASMTOK_DEQ);
		out->setBang(bang);
		return out;
	}

	WASMTransNode::WASMTransNode(const ASTNode *rs_, const ASTNode *rd_): WASMTransNode(TypedReg(rs_), TypedReg(rd_)) {
		delete rs_;
		delete rd_;
	}

	WASMTransNode::WASMTransNode(const TypedReg &rs_, const TypedReg &rd_):
		WASMInstructionNode(WASM_TRANSNODE), RType(rs_, {}, rd_) {}

	std::string WASMTransNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + "translate " + cyan(rs) + dim(" -> ") + cyan(rd);
	}

	WASMTransNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "translate " + std::string(rs) + " -> " + std::string(rd);
	}

	WASMPageStackNode::WASMPageStackNode(bool is_push, const ASTNode *rs_):
	WASMInstructionNode(WASM_PAGESTACKNODE), RType(TypedReg(rs_), {}, {}), isPush(is_push) {
		delete rs_;
	}

	WASMPageStackNode::WASMPageStackNode(bool is_push, const TypedReg &rs_):
		WASMInstructionNode(WASM_PAGESTACKNODE), RType(rs_, {}, {}), isPush(is_push) {}

	std::string WASMPageStackNode::debugExtra() const {
		if (!rs)
			return WASMInstructionNode::debugExtra() + dim(isPush? "[" : "]") + " " + blue("%page");
		return WASMInstructionNode::debugExtra() + dim(isPush? ": [" : ": ]") + " " + blue("%page") + " " + cyan(rs);
	}

	WASMPageStackNode::operator std::string() const {
		if (!rs)
			return WASMInstructionNode::operator std::string() + (isPush? "[" : "]") + " %page";
		return WASMInstructionNode::operator std::string() + (isPush? ": [" : ": ]") + " %page " + std::string(rs);
	}

	WASMCtlbNode::WASMCtlbNode():
		WASMInstructionNode(WASM_CTLBNODE), RType(nullptr, nullptr, nullptr) {}

	std::string WASMCtlbNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("ctlb");
	}

	WASMCtlbNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "ctlb";
	}
}
