#include <iostream>
#include <sstream>

#include "parser/Lexer.h"
#include "parser/Parser.h"
#include "parser/StringSet.h"
#include "util/Color.h"
#include "wasm/Nodes.h"
#include "wasm/Registers.h"

namespace Wasmc {
	static Condition getCondition(const std::string &str) {
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

	WASMInstructionNode * WASMInstructionNode::setInSubroutine(bool value) {
		inSubroutine = value;
		return this;
	}

	WASMInstructionNode * WASMInstructionNode::setBang(int value) {
		bang = value;
		return this;
	}

	WASMInstructionNode * WASMInstructionNode::setSymbolType(SymbolType value) {
		symbolType = value;
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
		return new WASMLabelNode(label);
	}

	std::string WASMLabelNode::debugExtra() const {
		return cyan("@") + "\e[38;5;202m" + *label + "\e[39m";
	}

	WASMLabelNode::operator std::string() const {
		return "@" + *label;
	}

	RNode::RNode(ASTNode *rs_, ASTNode *oper_, ASTNode *rt_, ASTNode *rd_, ASTNode *unsigned_):
	WASMInstructionNode(WASM_RNODE), rs(rs_->lexerInfo), oper(oper_->lexerInfo), rt(rt_->lexerInfo), rd(rd_->lexerInfo),
	operToken(oper_->symbol), isUnsigned(!!unsigned_) {
		delete rs_;
		delete oper_;
		if (oper_ != rt_)
			delete rt_;
		delete rd_;
		if (unsigned_)
			delete unsigned_;
	}

	RNode::RNode(const std::string *rs_, const std::string *oper_, const std::string *rt_, const std::string *rd_,
	             int oper_token, bool is_unsigned):
		WASMInstructionNode(WASM_RNODE), rs(rs_), oper(oper_), rt(rt_), rd(rd_), operToken(oper_token),
		isUnsigned(is_unsigned) {}

	WASMInstructionNode * RNode::copy() const {
		return new RNode(rs, oper, rt, rd, operToken, isUnsigned);
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
	WASMInstructionNode(WASM_INODE), HasImmediate(getImmediate(imm_)), rs(rs_->lexerInfo), oper(oper_->lexerInfo),
	rd(rd_->lexerInfo), operToken(oper_->symbol), isUnsigned(!!unsigned_) {
		delete rs_;
		delete oper_;
		delete imm_;
		delete rd_;
		if (unsigned_)
			delete unsigned_;
	}

	INode::INode(const std::string *rs_, const std::string *oper_, const Immediate &imm_, const std::string *rd_,
	             int oper_token, bool is_unsigned):
		WASMInstructionNode(WASM_INODE), HasImmediate(imm_), rs(rs_), oper(oper_), rd(rd_), operToken(oper_token),
		isUnsigned(is_unsigned) {}

	WASMInstructionNode * INode::copy() const {
		return new INode(rs, oper, imm, rd, operToken, isUnsigned);
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
	WASMInstructionNode(sym), rs(rs_->lexerInfo), rd(rd_->lexerInfo), isByte(!!byte_) {
		delete rs_;
		delete rd_;
		if (byte_)
			delete byte_;
	}

	WASMMemoryNode::WASMMemoryNode(int sym, const std::string *rs_, const std::string *rd_, bool is_byte):
		WASMInstructionNode(sym), rs(rs_), rd(rd_), isByte(is_byte) {}

	WASMCopyNode::WASMCopyNode(ASTNode *rs_, ASTNode *rd_, ASTNode *byte_):
		WASMMemoryNode(WASM_COPYNODE, rs_, rd_, byte_) {}

	WASMCopyNode::WASMCopyNode(const std::string *rs_, const std::string *rd_, bool is_byte):
		WASMMemoryNode(WASM_COPYNODE, rs_, rd_, is_byte) {}

	WASMInstructionNode * WASMCopyNode::copy() const {
		return new WASMCopyNode(rs, rd, isByte);
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
		return new WASMLoadNode(rs, rd, isByte);
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
		return new WASMStoreNode(rs, rd, isByte);
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
		WASMInstructionNode(WASM_SETNODE), HasImmediate(imm_), rd(rd_) {}

	WASMInstructionNode * WASMSetNode::copy() const {
		return new WASMSetNode(imm, rd);
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
		WASMInstructionNode(WASM_LINODE), HasImmediate(imm_), rd(rd_), isByte(is_byte) {}

	WASMInstructionNode * WASMLiNode::copy() const {
		return new WASMLiNode(imm, rd, isByte);
	}

	std::string WASMLiNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim("[") + colorize(imm) + dim("] -> ") + cyan(*rd)
			+ (isByte? " /b" : "");
	}

	WASMLiNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "[" + toString(imm) + "] -> " + *rd + (isByte? " /b" : "");
	}

	WASMSiNode::WASMSiNode(ASTNode *rs_, ASTNode *imm_, ASTNode *byte_):
	WASMInstructionNode(WASM_SINODE), HasImmediate(getImmediate(imm_)), rs(rs_->lexerInfo), isByte(!!byte_) {
		delete rs_;
		delete imm_;
		if (byte_)
			delete byte_;
	}

	WASMSiNode::WASMSiNode(const Immediate &imm_, const std::string *rs_, bool is_byte):
		WASMInstructionNode(WASM_SINODE), HasImmediate(imm_), rs(rs_), isByte(is_byte) {}

	WASMInstructionNode * WASMSiNode::copy() const {
		return new WASMSiNode(imm, rs, isByte);
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
		return new WASMLniNode(imm, rd, isByte);
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
	WASMInstructionNode(sym), rs(rs_->lexerInfo), rd(rd_->lexerInfo) {
		delete rs_;
		delete rd_;
	}

	WASMHalfMemoryNode::WASMHalfMemoryNode(int sym, const std::string *rs_, const std::string *rd_):
		WASMInstructionNode(sym), rs(rs_), rd(rd_) {}

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

	WASMCmpNode::WASMCmpNode(ASTNode *rs_, ASTNode *rt_):
	WASMInstructionNode(WASM_CMPNODE), rs(rs_->lexerInfo), rt(rt_->lexerInfo) {
		delete rs_;
		delete rt_;
	}

	WASMCmpNode::WASMCmpNode(const std::string *rs_, const std::string *rt_):
		WASMInstructionNode(WASM_CMPNODE), rs(rs_), rt(rt_) {}

	std::string WASMCmpNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(*rs) + dim(" ~ ") + cyan(*rt);
	}

	WASMCmpNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + *rs + " ~ " + *rt;
	}

	WASMCmpiNode::WASMCmpiNode(ASTNode *rs_, ASTNode *imm_):
	WASMInstructionNode(WASM_CMPINODE), HasImmediate(getImmediate(imm_)), rs(rs_->lexerInfo) {
		delete rs_;
		delete imm_;
	}

	WASMCmpiNode::WASMCmpiNode(const std::string *rs_, const Immediate &imm_):
		WASMInstructionNode(WASM_CMPINODE), HasImmediate(imm_), rs(rs_) {}

	std::string WASMCmpiNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(*rs) + dim(" ~ ") + colorize(imm);
	}

	WASMCmpiNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + *rs + " ~ " + toString(imm);
	}

	WASMSelNode::WASMSelNode(ASTNode *rs_, ASTNode *oper_, ASTNode *rt_, ASTNode *rd_):
	WASMInstructionNode(WASM_SELNODE), rs(rs_->lexerInfo), rt(rt_->lexerInfo), rd(rd_->lexerInfo) {
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
		WASMInstructionNode(WASM_SELNODE), rs(rs_), rt(rt_), rd(rd_), condition(cond) {}

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

	WASMJNode::WASMJNode(ASTNode *cond, ASTNode *colons, ASTNode *addr_):
	WASMInstructionNode(WASM_JNODE), HasImmediate(getImmediate(addr_)), link(!colons->empty()) {
		delete addr_;
		delete colons;
		if (!cond) {
			condition = Condition::None;
		} else {
			condition = getCondition(*cond->lexerInfo);
			delete cond;
		}
	}

	WASMJNode::WASMJNode(const Immediate &addr, bool link_, Condition cond):
		WASMInstructionNode(WASM_JNODE), HasImmediate(addr), condition(cond), link(link_) {}

	std::string WASMJNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim(conditionString(condition) + std::string(link? "::" : ":")) + " "
			+ colorize(imm);
	}

	WASMJNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + conditionString(condition) + std::string(link? "::" : ":")
			+ " " + toString(imm);
	}

	WASMJeqNode::WASMJeqNode(WASMJNode *j, ASTNode *rs_, ASTNode *rt_):
	WASMInstructionNode(WASM_JEQNODE), link(j? j->link : false), addr(j? j->imm : 0), rt(getEither(rt_)),
	rs(rs_->lexerInfo) {
		if (!j) {
			wasmerror("No WASMCJNode found in jeq instruction");
		} else {
			if (j->condition != Condition::None)
				wasmerror("Conditions specified for jeq instruction will be ignored");
			delete j;
		}
		delete rs_;
	}

	WASMJeqNode::WASMJeqNode(const Either &addr_, bool link_, const std::string *rs_, const Either &rt_):
		WASMInstructionNode(WASM_JEQNODE), link(link_), addr(addr_), rt(rt_), rs(rs_) {}

	std::string WASMJeqNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim(link? "::" : ":") + " " + colorize(addr) + red(" if ")
			+ cyan(*rs) + " == " + colorize(rt);
	}

	WASMJeqNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(link? "::" : ":") + " " + toString(addr)
			+ " if " + *rs + " == " + toString(rt);
	}

	WASMJcNode::WASMJcNode(WASMJNode *j, ASTNode *rs_):
	WASMInstructionNode(WASM_JCNODE), HasImmediate(j? j->imm : 0), link(j? j->link : false), rs(rs_->lexerInfo) {
		if (!j) {
			wasmerror("No WASMCJNode found in jc instruction");
		} else {
			if (j->condition != Condition::None)
				wasmerror("Conditions specified for jc instruction will be ignored");
			delete j;
		}
		delete rs_;
	}

	WASMJcNode::WASMJcNode(const Immediate &imm_, bool link_, const std::string *rs_):
		WASMInstructionNode(WASM_JCNODE), HasImmediate(imm_), link(link_), rs(rs_) {}

	std::string WASMJcNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim(link? "::" : ":") + " " + colorize(imm) + red(" if ")
			+ cyan(*rs);
	}

	WASMJcNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(link? "::" : ":") + " " + toString(imm)
			+ " if " + *rs;
	}

	WASMJrNode::WASMJrNode(ASTNode *cond, ASTNode *colons, ASTNode *rd_):
	WASMInstructionNode(WASM_JRNODE), link(!colons->empty()), rd(rd_->lexerInfo) {
		delete colons;
		delete rd_;
		if (!cond) {
			condition = Condition::None;
		} else {
			condition = getCondition(*cond->lexerInfo);
			delete cond;
		}
	}

	WASMJrNode::WASMJrNode(Condition condition_, bool link_, const std::string &rd_):
		WASMInstructionNode(WASM_JRNODE), condition(condition_), link(link_), rd(StringSet::intern(rd_)) {}

	WASMJrNode::WASMJrNode(Condition condition_, bool link_, const std::string *rd_):
		WASMInstructionNode(WASM_JRNODE), condition(condition_), link(link_), rd(rd_) {}

	std::string WASMJrNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim(conditionString(condition) + std::string(link? "::" : ":")) + " "
			+ cyan(*rd);
	}

	WASMJrNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + conditionString(condition) + std::string(link? "::" : ":")
			+ " " + *rd;
	}

	WASMJrcNode::WASMJrcNode(WASMJrNode *jr, ASTNode *rs_):
	WASMInstructionNode(WASM_JRCNODE), link(jr? jr->link : false), rs(rs_->lexerInfo), rd(jr? jr->rd : nullptr) {
		if (!jr) {
			wasmerror("No WASMCJrNode found in jr(l)c instruction");
		} else {
			if (jr->condition != Condition::None)
				wasmerror("Conditions specified for jr(l)c instruction will be ignored");
			delete jr;
		}
		delete rs_;
	}

	WASMJrcNode::WASMJrcNode(bool link_, const std::string *rs_, const std::string *rd_):
		WASMInstructionNode(WASM_JRCNODE), link(link_), rs(rs_), rd(rd_) {}

	std::string WASMJrcNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim(link? "::" : ":") + " " + cyan(*rd) + red(" if ") + cyan(*rs);
	}

	WASMJrcNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(link? "::" : ":") + " " + *rd + " if " + *rs;
	}

	WASMSizedStackNode::WASMSizedStackNode(ASTNode *size_, ASTNode *rs_, bool is_push):
	WASMInstructionNode(WASM_SSNODE), size(size_->atoi()), rs(rs_->lexerInfo), isPush(is_push) {
		delete size_;
		delete rs_;
	}

	WASMSizedStackNode::WASMSizedStackNode(long size_, const std::string *rs_, bool is_push):
		WASMInstructionNode(WASM_SSNODE), size(size_), rs(rs_), isPush(is_push) {}

	std::string WASMSizedStackNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim(std::string(isPush? "[" : "]") + ":" + std::to_string(size))
			+ " " + cyan(*rs);
	}

	WASMSizedStackNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(isPush? "[" : "]") + ":" + std::to_string(size)
			+ " " + *rs;
	}

	WASMMultRNode::WASMMultRNode(ASTNode *rs_, ASTNode *rt_, ASTNode *unsigned_):
	WASMInstructionNode(WASM_MULTRNODE), rs(rs_->lexerInfo), rt(rt_->lexerInfo), isUnsigned(!!unsigned_) {
		delete rs_;
		delete rt_;
		if (unsigned_)
			delete unsigned_;
	}

	WASMMultRNode::WASMMultRNode(const std::string *rs_, const std::string *rt_, bool is_unsigned):
		WASMInstructionNode(WASM_MULTRNODE), rs(rs_), rt(rt_), isUnsigned(is_unsigned) {}

	std::string WASMMultRNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(*rs) + dim(" * ") + cyan(*rt) + (isUnsigned? " /u" : "");
	}

	WASMMultRNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + *rs + " * " + *rt + (isUnsigned? " /u" : "");
	}

	WASMMultINode::WASMMultINode(ASTNode *rs_, ASTNode *imm_, ASTNode *unsigned_):
	WASMInstructionNode(WASM_MULTINODE), HasImmediate(getImmediate(imm_)), rs(rs_->lexerInfo), isUnsigned(!!unsigned_) {
		delete rs_;
		delete imm_;
		if (unsigned_)
			delete unsigned_;
	}

	WASMMultINode::WASMMultINode(const std::string *rs_, const Immediate &imm_, bool is_unsigned):
		WASMInstructionNode(WASM_MULTINODE), HasImmediate(imm_), rs(rs_), isUnsigned(is_unsigned) {}

	std::string WASMMultINode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + cyan(*rs) + dim(" * ") + colorize(imm) + (isUnsigned? " /u" : "");
	};

	WASMMultINode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + *rs + " * " + toString(imm) + (isUnsigned? " /u" : "");
	}

	WASMDiviINode::WASMDiviINode(ASTNode *imm_, ASTNode *rs_, ASTNode *rd_, ASTNode *unsigned_):
	WASMInstructionNode(WASM_DIVIINODE), HasImmediate(getImmediate(imm_)), rs(rs_->lexerInfo), rd(rd_->lexerInfo),
	isUnsigned(!!unsigned_) {
		delete rs_;
		delete rd_;
		delete imm_;
		if (unsigned_)
			delete unsigned_;
	}

	WASMDiviINode::WASMDiviINode(const Immediate &imm_, const std::string *rs_, const std::string *rd_,
	                             bool is_unsigned):
		WASMInstructionNode(WASM_DIVIINODE), HasImmediate(imm_), rs(rs_), rd(rd_), isUnsigned(is_unsigned) {}

	std::string WASMDiviINode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + colorize(imm) + dim(" / ") + cyan(*rs) + dim(" -> ") + cyan(*rd)
			+ (isUnsigned? " /u" : "");
	}

	WASMDiviINode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + toString(imm) + " / " + *rs + " -> " + *rd
			+ (isUnsigned? " /u" : "");
	}

	WASMLuiNode::WASMLuiNode(ASTNode *imm_, ASTNode *rd_):
	WASMInstructionNode(WASM_LUINODE), HasImmediate(getImmediate(imm_)), rd(rd_->lexerInfo) {
		delete imm_;
		delete rd_;
	}

	WASMLuiNode::WASMLuiNode(const Immediate &imm_, const std::string *rd_):
		WASMInstructionNode(WASM_LUINODE), HasImmediate(imm_), rd(rd_) {}

	std::string WASMLuiNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + "lui" + dim(": ") + colorize(imm) + dim(" -> ") + cyan(*rd);
	}

	WASMLuiNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "lui: " + toString(imm) + " -> " + *rd;
	}

	WASMStackNode::WASMStackNode(ASTNode *reg_, bool is_push):
	WASMInstructionNode(WASM_STACKNODE), reg(reg_->lexerInfo), isPush(is_push) {
		delete reg_;
	}

	WASMStackNode::WASMStackNode(const std::string *reg_, bool is_push):
		WASMInstructionNode(WASM_STACKNODE), reg(reg_), isPush(is_push) {}

	std::string WASMStackNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim(isPush? "[" : "]") + " " + cyan(*reg);
	}

	WASMStackNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + std::string(isPush? "[" : "]") + " " + *reg;
	}

	WASMNopNode::WASMNopNode(): WASMInstructionNode(WASM_NOPNODE) {}

	std::string WASMNopNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + dim("<>");
	}

	WASMNopNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "<>";
	}

	WASMIntINode::WASMIntINode(ASTNode *imm_): WASMInstructionNode(WASM_INTINODE), HasImmediate(getImmediate(imm_)) {
		delete imm_;
	}

	WASMIntINode::WASMIntINode(const Immediate &imm_): WASMInstructionNode(WASM_INTINODE), HasImmediate(imm_) {}

	std::string WASMIntINode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%int") + " " + colorize(imm);
	}

	WASMIntINode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%int " + toString(imm);
	}

	WASMRitINode::WASMRitINode(ASTNode *imm_): WASMInstructionNode(WASM_RITINODE), HasImmediate(getImmediate(imm_)) {
		delete imm_;
	}

	WASMRitINode::WASMRitINode(const Immediate &imm_): WASMInstructionNode(WASM_RITINODE), HasImmediate(imm_) {}

	std::string WASMRitINode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%rit") + " " + colorize(imm);
	}

	WASMRitINode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%rit " + toString(imm);
	}

	WASMTimeINode::WASMTimeINode(ASTNode *imm_): WASMInstructionNode(WASM_TIMEINODE), HasImmediate(getImmediate(imm_)) {
		delete imm_;
	}

	WASMTimeINode::WASMTimeINode(const Immediate &imm_): WASMInstructionNode(WASM_TIMEINODE), HasImmediate(imm_) {}

	std::string WASMTimeINode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%time") + " " + colorize(imm);
	}

	WASMTimeINode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%time " + toString(imm);
	}

	WASMTimeRNode::WASMTimeRNode(ASTNode *rs_): WASMInstructionNode(WASM_TIMERNODE), rs(rs_->lexerInfo) {
		delete rs_;
	}

	WASMTimeRNode::WASMTimeRNode(const std::string *rs_): WASMInstructionNode(WASM_TIMERNODE), rs(rs_) {}

	std::string WASMTimeRNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%time") + " " + cyan(*rs);
	}

	WASMTimeRNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%time " + *rs;
	}

	WASMRingINode::WASMRingINode(ASTNode *imm_): WASMInstructionNode(WASM_RINGINODE), HasImmediate(getImmediate(imm_)) {
		delete imm_;
	}

	WASMRingINode::WASMRingINode(const Immediate &imm_): WASMInstructionNode(WASM_RINGINODE), HasImmediate(imm_) {}

	std::string WASMRingINode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%ring") + " " + colorize(imm);
	}

	WASMRingINode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%ring " + toString(imm);
	}

	WASMRingRNode::WASMRingRNode(ASTNode *rs_): WASMInstructionNode(WASM_RINGRNODE), rs(rs_->lexerInfo) {
		delete rs_;
	}

	WASMRingRNode::WASMRingRNode(const std::string *rs_): WASMInstructionNode(WASM_RINGRNODE), rs(rs_) {}

	std::string WASMRingRNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%ring") + " " + cyan(*rs);
	}

	WASMRingRNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%ring " + *rs;
	}

	WASMPrintNode::WASMPrintNode(ASTNode *rs_, ASTNode *type_):
	WASMInstructionNode(WASM_PRINTNODE), rs(rs_->lexerInfo) {
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
		WASMInstructionNode(WASM_PRINTNODE), rs(rs_), type(type_) {}

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

	WASMSleepRNode::WASMSleepRNode(ASTNode *rs_): WASMInstructionNode(WASM_SLEEPRNODE), rs(rs_->lexerInfo) {
		delete rs_;
	}

	WASMSleepRNode::WASMSleepRNode(const std::string *rs_): WASMInstructionNode(WASM_SLEEPRNODE), rs(rs_) {}

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

	WASMSetptINode::WASMSetptINode(ASTNode *imm_):
	WASMInstructionNode(WASM_SETPTINODE), HasImmediate(getImmediate(imm_)) {
		delete imm_;
	}

	WASMSetptINode::WASMSetptINode(const Immediate &imm_): WASMInstructionNode(WASM_SETPTINODE), HasImmediate(imm_) {}

	std::string WASMSetptINode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%setpt") + " " + colorize(imm);
	}

	WASMSetptINode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%setpt " + toString(imm);
	}

	WASMSetptRNode::WASMSetptRNode(ASTNode *rs_): WASMInstructionNode(WASM_SETPTRNODE), rs(rs_->lexerInfo) {
		delete rs_;
	}

	WASMSetptRNode::WASMSetptRNode(const std::string *rs_): WASMInstructionNode(WASM_SETPTRNODE), rs(rs_) {}

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

	WASMSvpgNode::WASMSvpgNode(ASTNode *rd_): WASMInstructionNode(WASM_SVPGNODE), rd(rd_->lexerInfo) {
		delete rd_;
	}

	WASMSvpgNode::WASMSvpgNode(const std::string *rd_): WASMInstructionNode(WASM_SVPGNODE), rd(rd_) {}

	std::string WASMSvpgNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + blue("%page") + dim(" -> ") + cyan(*rd);
	}

	WASMSvpgNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "%page -> " + *rd;
	}

	WASMQueryNode::WASMQueryNode(QueryType type_, ASTNode *rd_):
	WASMInstructionNode(WASM_QUERYNODE), type(type_), rd(rd_->lexerInfo) {
		delete rd_;
	}

	WASMQueryNode::WASMQueryNode(QueryType type_, const std::string *rd_):
		WASMInstructionNode(WASM_QUERYNODE), type(type_), rd(rd_) {}

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

	std::string WASMStringPrintNode::debugExtra() const {
		return WASMInstructionNode::debugExtra() + "<" + blue("p") + " \"" + *string + "\">";
	}

	WASMStringPrintNode::operator std::string() const {
		return WASMInstructionNode::operator std::string() + "<p \"" + *string + "\">";
	}

	WASMCallNode::WASMCallNode(ASTNode *function_, ASTNode *args_):
		WASMInstructionNode(WASM_CALLNODE), function(function_->lexerInfo), args(args_) {}

	WASMCallNode::WASMCallNode(const std::string *function_, const Args &args_):
		WASMInstructionNode(WASM_CALLNODE), function(function_), args(args_) {}

	std::string WASMCallNode::debugExtra() const {
		std::stringstream ss;
		ss << bold(*function) << dim("(");
		bool first = true;
		for (const auto &arg: args) {
			if (first)
				first = false;
			else
				ss << dim(", ");
			ss << std::string(*arg);
		}
		ss << dim(")");
		return WASMInstructionNode::debugExtra() + ss.str();
	}

	WASMCallNode::operator std::string() const {
		std::stringstream ss;
		ss << *function << "(";
		bool first = true;
		for (const auto &arg: args) {
			if (first)
				first = false;
			else
				ss << ", ";
			ss << std::string(*arg);
		}
		ss << ")";
		return ss.str();
	}

	RNode * makeSeq(const std::string *rs, const std::string *rt, const std::string *rd) {
		static const auto deq = StringSet::intern("==");
		return new RNode(rs, deq, rt, rd, WASMTOK_DEQ, false);
	}
}
