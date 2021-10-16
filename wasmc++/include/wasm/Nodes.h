#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>

#include "compiler/Immediate.h"
#include "parser/ASTNode.h"
#include "parser/Enums.h"
#include "parser/StringSet.h"
#include "wasm/Args.h"
#include "wasm/Instructions.h"
#include "wasm/Types.h"

namespace Wasmc {
	enum class WASMNodeType {
		Immediate, RType, IType, Copy, Load, Store, Set, Li, Si, Lni, Ch, Lh, Sh, Cmp, Cmpi, Sel, J, Jc, Jr, Jrc, Mv,
		SizedStack, MultR, MultI, DiviI, Lui, Stack, Nop, IntI, RitI, TimeI, TimeR, RingI, RingR, Print, Halt, SleepR,
		Page, SetptI, Label, SetptR, Svpg, Query, PseudoPrint, Statement, Call, StringPrint, Jeq, JeqI, Cs, Ls, Ss
	};

	Condition getCondition(const std::string &);
	Condition getCondition(const ASTNode *);

	struct WASMBaseNode: ASTNode {
		WASMBaseNode(int sym);
		virtual WASMNodeType nodeType() const = 0;
		virtual operator std::string() const = 0;
	};

	struct HasImmediate {
		Immediate imm;
		HasImmediate(const ASTNode *imm_): imm(getImmediate(imm_)) {}
		HasImmediate(const Immediate &imm_): imm(imm_) {}
	};

	struct HasSecondImmediate {
		Immediate imm2;
		HasSecondImmediate(const Immediate &imm2_): imm2(imm2_) {}
	};

	struct HasRS {
		const std::string *rs;
		HasRS(const ASTNode *node): HasRS(node? node->lexerInfo : nullptr) {}
		HasRS(const std::string *rs_): rs(rs_? rs_ : StringSet::intern("$0")) {}
	};

	struct HasRT {
		const std::string *rt;
		HasRT(const ASTNode *node): HasRT(node? node->lexerInfo : nullptr) {}
		HasRT(const std::string *rt_): rt(rt_? rt_ : StringSet::intern("$0")) {}
	};

	struct HasRD {
		const std::string *rd;
		HasRD(const ASTNode *node): HasRD(node? node->lexerInfo : nullptr) {}
		HasRD(const std::string *rd_): rd(rd_? rd_ : StringSet::intern("$0")) {}
	};

	struct HasOper {
		const std::string *oper;
		int operToken;
		HasOper(): HasOper(0, nullptr) {}
		HasOper(const ASTNode *node): HasOper(node->symbol, node->lexerInfo) {}
		HasOper(int oper_token, const std::string *oper_ = nullptr): oper(oper_), operToken(oper_token) {}
	};

	struct HasUnsigned {
		bool isUnsigned;
		HasUnsigned(const ASTNode *node): HasUnsigned(!!node) {}
		HasUnsigned(bool is_unsigned): isUnsigned(is_unsigned) {}
	};

	struct HasCondition {
		Condition condition;
		HasCondition(const ASTNode *node): condition(getCondition(node)) {}
		HasCondition(Condition condition_): condition(condition_) {}
	};

	struct HasLink {
		bool link;
		HasLink(const ASTNode *colons): link(!colons->empty()) {}
		HasLink(bool link_): link(link_) {}
	};

	struct ThreeRegs: HasRS, HasRT, HasRD {
		ThreeRegs():
			HasRS((const std::string *) nullptr), HasRT((const std::string *) nullptr),
			HasRD((const std::string *) nullptr) {}
		ThreeRegs(const ASTNode *rs_, const ASTNode *rt_, const ASTNode *rd_): HasRS(rs_), HasRT(rt_), HasRD(rd_) {}
		ThreeRegs(const std::string *rs_, const std::string *rt_, const std::string *rd_):
			HasRS(rs_), HasRT(rt_), HasRD(rd_) {}
	};

	struct TwoRegs: HasRS, HasRD {
		TwoRegs(const ASTNode *rs_, const ASTNode *rd_): HasRS(rs_), HasRD(rd_) {}
		TwoRegs(const std::string *rs_, const std::string *rd_): HasRS(rs_), HasRD(rd_) {}
	};

	struct WASMInstructionNode: WASMBaseNode {
		int bang = -1;
		std::vector<const std::string *> labels;
		bool inSubroutine = false;
		LinkerFlags flags = LinkerFlags::Ignore;

		using WASMBaseNode::WASMBaseNode;

		WASMInstructionNode * absorbIntbang(ASTNode *);
		WASMInstructionNode * absorbLabel(ASTNode *);
		WASMInstructionNode * absorb(const WASMInstructionNode &);
		WASMInstructionNode * setInSubroutine(bool);
		WASMInstructionNode * setBang(int);
		virtual WASMInstructionNode * copy() const = 0;
		WASMNodeType nodeType() const override { return WASMNodeType::Statement; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMImmediateNode: WASMBaseNode, HasImmediate {
		WASMImmediateNode(ASTNode *);
		WASMImmediateNode(const Immediate &);
		WASMNodeType nodeType() const override { return WASMNodeType::Immediate; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMLabelNode: WASMInstructionNode { // Not technically an instruction, but still.
		const std::string *label;

		WASMLabelNode(ASTNode *);
		WASMLabelNode(const std::string *);
		WASMInstructionNode * copy() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::Label; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct HasOpcode {
		virtual Opcode getOpcode() const = 0;
	};

	struct RType: HasOpcode, ThreeRegs {
		RType(): ThreeRegs() {}
		RType(const ASTNode *rs_, const ASTNode *rt_, const ASTNode *rd_): ThreeRegs(rs_, rt_, rd_) {}
		RType(const std::string *rs_, const std::string *rt_, const std::string *rd_): ThreeRegs(rs_, rt_, rd_) {}
		virtual Funct getFunct() const = 0;
	};

	struct IType: HasOpcode, HasImmediate, TwoRegs {
		IType(const ASTNode *rs_, const ASTNode *rd_, const ASTNode *imm_):
			HasImmediate(imm_), TwoRegs(rs_, rd_) {}
		IType(const ASTNode *imm_):
			HasImmediate(imm_), TwoRegs((const std::string *) nullptr, (const std::string *) nullptr) {}
		IType(const std::string *rs_, const std::string *rd_, const Immediate &imm_):
			HasImmediate(imm_), TwoRegs(rs_, rd_) {}
		IType(const Immediate &imm_):
			HasImmediate(imm_), TwoRegs((const std::string *) nullptr, (const std::string *) nullptr) {}
	};

	struct JType: HasOpcode, HasImmediate, HasRS, HasCondition, HasLink {
		JType(const ASTNode *cond, const ASTNode *colons, const ASTNode *addr, const ASTNode *rs_):
			HasImmediate(getImmediate(addr)), HasRS(rs_), HasCondition(cond), HasLink(colons) {}
		JType(Condition cond, bool link_, const Immediate &addr, const std::string *rs_):
			HasImmediate(addr), HasRS(rs_), HasCondition(cond), HasLink(link_) {}
	};

	/** For math and logical operations. */
	struct RNode: WASMInstructionNode, RType, HasOper, HasUnsigned {
		RNode(ASTNode *rs_, ASTNode *oper_, ASTNode *rt_, ASTNode *rd_, ASTNode *unsigned_);
		RNode(const std::string *rs_, const std::string *oper_, const std::string *rt_, const std::string *rd_,
		      int oper_token, bool is_unsigned);
		WASMInstructionNode * copy() const override;
		Opcode getOpcode() const override;
		Funct getFunct() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::RType; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	/** For math and logical operations. */
	struct INode: WASMInstructionNode, IType, HasOper, HasUnsigned {
		INode(ASTNode *rs_, ASTNode *oper_, ASTNode *imm, ASTNode *rd_, ASTNode *unsigned_);
		INode(const std::string *rs_, const std::string *oper_, const Immediate &imm_, const std::string *rd_,
		      int oper_token, bool is_unsigned);
		Opcode getOpcode() const override;
		WASMInstructionNode * copy() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::IType; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMMemoryNode: WASMInstructionNode, RType {
		bool isByte;
		WASMMemoryNode(int sym, ASTNode *rs_, ASTNode *rd_, ASTNode *byte_);
		WASMMemoryNode(int sym, const std::string *rs_, const std::string *rd_, bool is_byte);
	};

	struct WASMCopyNode: WASMMemoryNode {
		WASMCopyNode(ASTNode *rs_, ASTNode *rd_, ASTNode *byte_);
		WASMCopyNode(const std::string *rs_, const std::string *rd_, bool is_byte);
		Opcode getOpcode() const override { return OPCODES.at(isByte? "cb" : "c"); }
		Funct getFunct() const override { return FUNCTS.at(isByte? "cb" : "c"); }
		WASMInstructionNode * copy() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::Copy; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMLoadNode: WASMMemoryNode {
		WASMLoadNode(ASTNode *rs_, ASTNode *rd_, ASTNode *byte_);
		WASMLoadNode(const std::string *rs_, const std::string *rd_, bool is_byte);
		Opcode getOpcode() const override { return OPCODES.at(isByte? "lb" : "l"); }
		Funct getFunct() const override { return FUNCTS.at(isByte? "lb" : "l"); }
		WASMInstructionNode * copy() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::Load; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMStoreNode: WASMMemoryNode {
		WASMStoreNode(ASTNode *rs_, ASTNode *rd_, ASTNode *byte_);
		WASMStoreNode(const std::string *rs_, const std::string *rd_, bool is_byte);
		Opcode getOpcode() const override { return OPCODES.at(isByte? "sb" : "s"); }
		Funct getFunct() const override { return FUNCTS.at(isByte? "sb" : "s"); }
		WASMInstructionNode * copy() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::Store; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMSetNode: WASMInstructionNode, IType {
		WASMSetNode(ASTNode *imm_, ASTNode *rd_);
		WASMSetNode(const Immediate &imm_, const std::string *rd_);
		Opcode getOpcode() const override { return OPCODES.at("set"); }
		WASMInstructionNode * copy() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::Set; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMLiNode: WASMInstructionNode, IType {
		bool isByte;

		WASMLiNode(ASTNode *imm_, ASTNode *rd_, ASTNode *byte_);
		WASMLiNode(const Immediate &imm_, const std::string *rd_, bool is_byte);
		Opcode getOpcode() const override { return OPCODES.at(isByte? "lbi" : "li"); }
		WASMInstructionNode * copy() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::Li; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMSiNode: WASMInstructionNode, IType {
		bool isByte;

		WASMSiNode(ASTNode *rs_, ASTNode *imm_, ASTNode *byte_);
		WASMSiNode(const Immediate &imm_, const std::string *rd_, bool is_byte);
		Opcode getOpcode() const override { return OPCODES.at(isByte? "sbi" : "si"); }
		WASMInstructionNode * copy() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::Si; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMLniNode: WASMLiNode {
		WASMLniNode(ASTNode *imm_, ASTNode *rd_, ASTNode *byte_);
		WASMLniNode(const Immediate &imm_, const std::string *rd_, bool is_byte);
		Opcode getOpcode() const override { return OPCODES.at(isByte? "lbni" : "lni"); }
		WASMInstructionNode * copy() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::Lni; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMHalfMemoryNode: WASMInstructionNode, RType {
		WASMHalfMemoryNode(int sym, ASTNode *rs_, ASTNode *rd_);
		WASMHalfMemoryNode(int sym, const std::string *rs_, const std::string *rd_);
	};

	struct WASMChNode: WASMHalfMemoryNode {
		WASMChNode(ASTNode *rs_, ASTNode *rd_);
		WASMChNode(const std::string *rs_, const std::string *rd_);
		Opcode getOpcode() const override { return OPCODES.at("ch"); }
		Funct getFunct() const override { return FUNCTS.at("ch"); }
		WASMInstructionNode * copy() const override { return (new WASMChNode(rs, rd))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Ch; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMLhNode: WASMHalfMemoryNode {
		WASMLhNode(ASTNode *rs_, ASTNode *rd_);
		WASMLhNode(const std::string *rs_, const std::string *rd_);
		Opcode getOpcode() const override { return OPCODES.at("lh"); }
		Funct getFunct() const override { return FUNCTS.at("lh"); }
		WASMInstructionNode * copy() const override { return (new WASMLhNode(rs, rd))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Lh; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMShNode: WASMHalfMemoryNode {
		WASMShNode(ASTNode *rs_, ASTNode *rd_);
		WASMShNode(const std::string *rs_, const std::string *rd_);
		Opcode getOpcode() const override { return OPCODES.at("sh"); }
		Funct getFunct() const override { return FUNCTS.at("sh"); }
		WASMInstructionNode * copy() const override { return (new WASMShNode(rs, rd))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Sh; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMShortMemoryNode: WASMInstructionNode, RType {
		WASMShortMemoryNode(int sym, ASTNode *rs_, ASTNode *rd_);
		WASMShortMemoryNode(int sym, const std::string *rs_, const std::string *rd_);
	};

	struct WASMCsNode: WASMShortMemoryNode {
		WASMCsNode(ASTNode *rs_, ASTNode *rd_);
		WASMCsNode(const std::string *rs_, const std::string *rd_);
		Opcode getOpcode() const override { return OPCODES.at("cs"); }
		Funct getFunct() const override { return FUNCTS.at("cs"); }
		WASMInstructionNode * copy() const override { return (new WASMCsNode(rs, rd))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Cs; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMLsNode: WASMShortMemoryNode {
		WASMLsNode(ASTNode *rs_, ASTNode *rd_);
		WASMLsNode(const std::string *rs_, const std::string *rd_);
		Opcode getOpcode() const override { return OPCODES.at("ls"); }
		Funct getFunct() const override { return FUNCTS.at("ls"); }
		WASMInstructionNode * copy() const override { return (new WASMLsNode(rs, rd))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Ls; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMSsNode: WASMShortMemoryNode {
		WASMSsNode(ASTNode *rs_, ASTNode *rd_);
		WASMSsNode(const std::string *rs_, const std::string *rd_);
		Opcode getOpcode() const override { return OPCODES.at("ss"); }
		Funct getFunct() const override { return FUNCTS.at("ss"); }
		WASMInstructionNode * copy() const override { return (new WASMSsNode(rs, rd))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Ss; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMCmpNode: WASMInstructionNode, RType {
		WASMCmpNode(ASTNode *rs_, ASTNode *rt_);
		WASMCmpNode(const std::string *rs_, const std::string *rt_);
		WASMInstructionNode * copy() const override { return (new WASMCmpNode(rs, rt))->absorb(*this); }
		Opcode getOpcode() const override { return OPCODES.at("cmp"); }
		Funct getFunct() const override { return FUNCTS.at("cmp"); }
		WASMNodeType nodeType() const override { return WASMNodeType::Cmp; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMCmpiNode: WASMInstructionNode, IType {
		WASMCmpiNode(ASTNode *rs_, ASTNode *imm_);
		WASMCmpiNode(const std::string *rs_, const Immediate &imm_);
		WASMInstructionNode * copy() const override { return (new WASMCmpiNode(rs, imm))->absorb(*this); }
		Opcode getOpcode() const override { return OPCODES.at("cmpi"); }
		WASMNodeType nodeType() const override { return WASMNodeType::Cmpi; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMSelNode: WASMInstructionNode, RType, HasCondition {
		WASMSelNode(ASTNode *rs_, ASTNode *oper_, ASTNode *rt_, ASTNode *rd_);
		WASMSelNode(const std::string *rs_, Condition cond, const std::string *rt_, const std::string *rd_);
		Opcode getOpcode() const override { return OPCODES.at("sel"); }
		Funct getFunct() const override { return FUNCTS.at("sel"); }
		WASMInstructionNode * copy() const override { return (new WASMSelNode(rs, condition, rt, rd))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Sel; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMJNode: WASMInstructionNode, JType {
		WASMJNode(ASTNode *cond, ASTNode *colons, ASTNode *addr_);
		WASMJNode(const Immediate &addr, bool link_ = false, Condition cond = Condition::None);
		Opcode getOpcode() const override { return OPCODES.at("j"); }
		WASMInstructionNode * copy() const override { return (new WASMJNode(imm, link, condition))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::J; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMJcNode: WASMInstructionNode, JType {
		WASMJcNode(WASMJNode *, ASTNode *rs_);
		WASMJcNode(const Immediate &imm_, bool link_, const std::string *rs_);
		Opcode getOpcode() const override { return OPCODES.at("jc"); }
		WASMInstructionNode * copy() const override { return (new WASMJcNode(imm, link, rs))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Jc; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	// Used for both jr and jrl.
	struct WASMJrNode: WASMInstructionNode, RType, HasCondition, HasLink {
		WASMJrNode(ASTNode *cond, ASTNode *colons, ASTNode *rd_);
		WASMJrNode(Condition condition_, bool link_, const std::string &rd_);
		WASMJrNode(Condition condition_, bool link_, const std::string *rd_);
		Opcode getOpcode() const override { return OPCODES.at(link? "jrl" : "jr"); }
		Funct getFunct() const override { return FUNCTS.at(link? "jrl" : "jr"); }
		WASMInstructionNode * copy() const override { return (new WASMJrNode(condition, link, rd))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Jr; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	// Used for both jrc and jrlc.
	struct WASMJrcNode: WASMInstructionNode, RType, HasLink {
		WASMJrcNode(WASMJrNode *, ASTNode *rs_);
		WASMJrcNode(bool link_, const std::string *rs_, const std::string *rd_);
		Opcode getOpcode() const override { return OPCODES.at(link? "jrlc" : "jrc"); }
		Funct getFunct() const override { return FUNCTS.at(link? "jrlc" : "jrc"); }
		WASMInstructionNode * copy() const override { return (new WASMJrcNode(link, rs, rd))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Jrc; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMJeqNode: WASMInstructionNode { // Pseudoinstruction; not handled by Assembler::compileInstruction
		bool link;
		Either addr, rt;
		const std::string *rs;

		WASMJeqNode(WASMJNode *, ASTNode *rs_, ASTNode *rt_);
		WASMJeqNode(WASMJrNode *, ASTNode *rs_, ASTNode *rt_);
		WASMJeqNode(const Either &addr_, bool link_, const std::string *rs_, const Either &rt_);
		WASMInstructionNode * copy() const override { return (new WASMJeqNode(addr, link, rs, rt))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Jeq; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	// Used for both sspush and sspop.
	struct WASMSizedStackNode: WASMInstructionNode, IType {
		bool isPush;
		WASMSizedStackNode(ASTNode *size, ASTNode *rs_, bool is_push);
		WASMSizedStackNode(int size, const std::string *rs_, bool is_push);
		Opcode getOpcode() const override { return OPCODES.at(isPush? "sspush" : "sspop"); }
		WASMInstructionNode * copy() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::SizedStack; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMMultRNode: WASMInstructionNode, RType, HasUnsigned {
		WASMMultRNode(ASTNode *rs_, ASTNode *rt_, ASTNode *unsigned_ = nullptr);
		WASMMultRNode(const std::string *rs_, const std::string *rt_, bool is_unsigned);
		Opcode getOpcode() const override { return OPCODES.at(isUnsigned? "multu" : "mult"); }
		Funct getFunct() const override { return FUNCTS.at(isUnsigned? "multu" : "mult"); }
		WASMInstructionNode * copy() const override { return (new WASMMultRNode(rs, rt, isUnsigned))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::MultR; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMMultINode: WASMInstructionNode, IType, HasUnsigned {
		WASMMultINode(ASTNode *rs_, ASTNode *imm_, ASTNode *unsigned_ = nullptr);
		WASMMultINode(const std::string *rs_, const Immediate &imm_, bool is_unsigned);
		Opcode getOpcode() const override { return OPCODES.at(isUnsigned? "multui" : "multi"); }
		WASMInstructionNode * copy() const override { return (new WASMMultINode(rs, imm, isUnsigned))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::MultI; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMDiviINode: WASMInstructionNode, IType, HasUnsigned {
		WASMDiviINode(ASTNode *imm_, ASTNode *rs_, ASTNode *rd_, ASTNode *unsigned_ = nullptr);
		WASMDiviINode(const Immediate &imm_, const std::string *rs_, const std::string *rd_, bool is_unsigned);
		Opcode getOpcode() const override { return OPCODES.at(isUnsigned? "divuii" : "divii"); }
		WASMInstructionNode * copy() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::DiviI; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMLuiNode: WASMInstructionNode, IType {
		WASMLuiNode(ASTNode *imm_, ASTNode *rd_);
		WASMLuiNode(const Immediate &imm_, const std::string *rd_);
		Opcode getOpcode() const override { return OPCODES.at("lui"); }
		WASMInstructionNode * copy() const override { return (new WASMLuiNode(imm, rd))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Lui; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	/** Used for both spush and spop. Because I'm lazy, rs and rd are set to the same register. */
	struct WASMStackNode: WASMInstructionNode, RType {
		bool isPush;
		WASMStackNode(ASTNode *reg, bool is_push);
		WASMStackNode(const std::string *reg, bool is_push);
		Opcode getOpcode() const override { return OPCODES.at(isPush? "spush" : "spop"); }
		Funct getFunct() const override { return FUNCTS.at(isPush? "spush" : "spop"); }
		WASMInstructionNode * copy() const override { return (new WASMStackNode(rs, isPush))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Stack; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMNopNode: WASMInstructionNode, HasOpcode {
		WASMNopNode();
		Opcode getOpcode() const override { return OPCODES.at("nop"); }
		WASMInstructionNode * copy() const override { return (new WASMNopNode())->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Nop; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMIntINode: WASMInstructionNode, IType {
		WASMIntINode(ASTNode *imm_);
		WASMIntINode(const Immediate &imm_);
		Opcode getOpcode() const override { return OPCODES.at("int"); }
		WASMInstructionNode * copy() const override { return (new WASMIntINode(imm))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::IntI; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMRitINode: WASMInstructionNode, IType {
		WASMRitINode(ASTNode *imm_);
		WASMRitINode(const Immediate &imm_);
		Opcode getOpcode() const override { return OPCODES.at("rit"); }
		WASMInstructionNode * copy() const override { return (new WASMRitINode(imm))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::RitI; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMTimeINode: WASMInstructionNode, IType {
		WASMTimeINode(ASTNode *imm_);
		WASMTimeINode(const Immediate &imm_);
		Opcode getOpcode() const override { return OPCODES.at("timei"); }
		WASMInstructionNode * copy() const override { return (new WASMTimeINode(imm))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::TimeI; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMTimeRNode: WASMInstructionNode, RType {
		WASMTimeRNode(ASTNode *rs_);
		WASMTimeRNode(const std::string *rs_);
		Opcode getOpcode() const override { return OPCODES.at("time"); }
		Funct getFunct() const override { return FUNCTS.at("time"); }
		WASMInstructionNode * copy() const override { return (new WASMTimeRNode(rs))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::TimeR; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMRingINode: WASMInstructionNode, IType {
		WASMRingINode(ASTNode *imm_);
		WASMRingINode(const Immediate &imm_);
		Opcode getOpcode() const override { return OPCODES.at("ringi"); }
		WASMInstructionNode * copy() const override { return (new WASMRingINode(imm))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::RingI; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMRingRNode: WASMInstructionNode, RType {
		WASMRingRNode(ASTNode *rs_);
		WASMRingRNode(const std::string *rs_);
		Opcode getOpcode() const override { return OPCODES.at("ring"); }
		Funct getFunct() const override { return FUNCTS.at("ring"); }
		WASMInstructionNode * copy() const override { return (new WASMRingRNode(rs))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::RingR; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMPrintNode: WASMInstructionNode, RType {
		PrintType type;

		WASMPrintNode(ASTNode *rs_, ASTNode *type_);
		WASMPrintNode(const std::string *rs_, PrintType type_);
		Opcode getOpcode() const override { return OPCODES.at("ext"); }
		Funct getFunct() const override;
		WASMInstructionNode * copy() const override { return (new WASMPrintNode(rs, type))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Print; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMHaltNode: WASMInstructionNode, RType {
		WASMHaltNode();
		Opcode getOpcode() const override { return OPCODES.at("ext"); }
		Funct getFunct() const override { return FUNCTS.at("halt"); }
		WASMInstructionNode * copy() const override { return (new WASMHaltNode())->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Halt; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMSleepRNode: WASMInstructionNode, RType {
		WASMSleepRNode(ASTNode *rs_);
		WASMSleepRNode(const std::string *rs_);
		Opcode getOpcode() const override { return OPCODES.at("ext"); }
		Funct getFunct() const override { return FUNCTS.at("sleep"); }
		WASMInstructionNode * copy() const override { return (new WASMSleepRNode(rs))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::SleepR; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMPageNode: WASMInstructionNode, RType {
		bool on;

		WASMPageNode(bool on_);
		Opcode getOpcode() const override { return OPCODES.at("pgon"); } // pgoff and pgon are the same
		Funct getFunct() const override { return FUNCTS.at(on? "pgon" : "pgoff"); }
		WASMInstructionNode * copy() const override { return (new WASMPageNode(on))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Page; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMSetptRNode: WASMInstructionNode, RType {
		WASMSetptRNode(ASTNode *rs_);
		WASMSetptRNode(const std::string *rs_);
		Opcode getOpcode() const override { return OPCODES.at("setpt"); }
		Funct getFunct() const override { return FUNCTS.at("setpt"); }
		WASMInstructionNode * copy() const override { return (new WASMSetptRNode(rs))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::SetptR; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	// Pseudoinstruction; not handled by Assembler::compileInstruction
	struct WASMMvNode: WASMInstructionNode {
		const std::string *rs, *rd;
		WASMMvNode(ASTNode *rs_, ASTNode *rd_);
		WASMMvNode(const std::string *rs_, const std::string *rd_);
		WASMInstructionNode * copy() const override { return (new WASMMvNode(rs, rd))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Mv; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMSvpgNode: WASMInstructionNode, RType {
		WASMSvpgNode(ASTNode *rd_);
		WASMSvpgNode(const std::string *rd_);
		Opcode getOpcode() const override { return OPCODES.at("svpg"); }
		Funct getFunct() const override { return FUNCTS.at("svpg"); }
		WASMInstructionNode * copy() const override { return (new WASMSvpgNode(rd))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Svpg; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMQueryNode: WASMInstructionNode, RType {
		QueryType type;

		WASMQueryNode(QueryType, ASTNode *rd_);
		WASMQueryNode(QueryType, const std::string *rd_);
		Opcode getOpcode() const override { return OPCODES.at("qm"); } // All query types share the same opcode
		Funct getFunct() const override;
		WASMInstructionNode * copy() const override { return (new WASMQueryNode(type, rd))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Query; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	// Pseudoinstruction; not handled by Assembler::compileInstruction
	struct WASMPseudoPrintNode: WASMInstructionNode, HasImmediate {
		WASMPseudoPrintNode(ASTNode *imm_);
		WASMPseudoPrintNode(const Immediate &imm_);
		WASMInstructionNode * copy() const override { return (new WASMPseudoPrintNode(imm))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::PseudoPrint; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	// Pseudoinstruction; not handled by Assembler::compileInstruction
	struct WASMStringPrintNode: WASMInstructionNode {
		const std::string *string;
		WASMStringPrintNode(ASTNode *string_);
		WASMStringPrintNode(const std::string *string_);
		WASMInstructionNode * copy() const override { return (new WASMStringPrintNode(string))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::StringPrint; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	// Pseudoinstruction; not handled by Assembler::compileInstruction
	struct WASMCallNode: WASMInstructionNode {
		const std::string *function;
		Args args;

		WASMCallNode(ASTNode *function_, ASTNode *args_ = nullptr);
		WASMCallNode(const std::string *function_, const Args &args_);
		WASMInstructionNode * copy() const override { return (new WASMCallNode(function, args))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Call; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	RNode * makeSeq(const std::string *rs, const std::string *rt, const std::string *rd, int bang = -1);
}
