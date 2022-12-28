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
		Page, SetptI, Label, SetptR, Svpg, Query, PseudoPrint, Statement, StringPrint, Jeq, JeqI, Cs, Ls, Ss, IO, Rest,
		Interrupts, Inverse, Svring, Svtime,
	};

	Condition getCondition(const std::string &);
	Condition getCondition(const ASTNode *);

	struct WASMBaseNode: ASTNode {
		WASMBaseNode(int sym);
		virtual WASMNodeType nodeType() const = 0;
		virtual operator std::string() const = 0;
	};

	struct HasImmediate {
		TypedImmediate imm;
		HasImmediate(const ASTNode *imm_): imm(getImmediate(imm_)) {}
		HasImmediate(const TypedImmediate &imm_): imm(imm_) {}
	};

	struct HasRS {
		TypedReg rs;
		HasRS(const ASTNode *node): HasRS(TypedReg(node)) {}
		HasRS(const TypedReg &rs_): rs(rs_) {}
	};

	struct HasRT {
		TypedReg rt;
		HasRT(const ASTNode *node): HasRT(TypedReg(node)) {}
		HasRT(const TypedReg &rt_): rt(rt_) {}
	};

	struct HasRD {
		TypedReg rd;
		HasRD(const ASTNode *node): HasRD(TypedReg(node)) {}
		HasRD(const TypedReg &rd_): rd(rd_) {}
	};

	struct HasOper {
		const std::string *oper;
		int operToken;
		HasOper(): HasOper(0, nullptr) {}
		HasOper(const ASTNode *node): HasOper(node->symbol, node->lexerInfo) {}
		HasOper(int oper_token, const std::string *oper_ = nullptr): oper(oper_), operToken(oper_token) {}
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
			HasRS(TypedReg()), HasRT(TypedReg()), HasRD(TypedReg()) {}
		ThreeRegs(const ASTNode *rs_, const ASTNode *rt_, const ASTNode *rd_):
			HasRS(rs_), HasRT(rt_), HasRD(rd_) {}
		ThreeRegs(const TypedReg &rs_, const TypedReg &rt_, const TypedReg &rd_):
			HasRS(rs_), HasRT(rt_), HasRD(rd_) {}
	};

	struct TwoRegs: HasRS, HasRD {
		TwoRegs(const ASTNode *rs_, const ASTNode *rd_): HasRS(rs_), HasRD(rd_) {}
		TwoRegs(const TypedReg &rs_, const TypedReg &rd_): HasRS(rs_), HasRD(rd_) {}
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
		/** Returns the number of instructions the instruction expands to. Will always be 1 except for certain
		 *  pseudoinstructions. */
		virtual size_t expandedSize() const { return 1; }
		WASMNodeType nodeType() const override { return WASMNodeType::Statement; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMLabelNode: WASMInstructionNode { // Not technically an instruction, but still.
		const std::string *label;

		WASMLabelNode(ASTNode *);
		WASMLabelNode(const std::string *);
		WASMInstructionNode * copy() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::Label; }
		size_t expandedSize() const override { return 0; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct HasOpcode {
		virtual Opcode getOpcode() const = 0;
	};

	struct RType: HasOpcode, ThreeRegs {
		RType(): ThreeRegs() {}
		RType(const ASTNode *rs_, const ASTNode *rt_, const ASTNode *rd_):
			ThreeRegs(rs_, rt_, rd_) {}
		RType(const TypedReg &rs_, const TypedReg &rt_, const TypedReg &rd_):
			ThreeRegs(rs_, rt_, rd_) {}
		virtual Funct getFunct() const = 0;
	};

	struct IType: HasOpcode, HasImmediate, TwoRegs {
		IType(const ASTNode *rs_, const ASTNode *rd_, const ASTNode *imm_):
			HasImmediate(imm_), TwoRegs(rs_, rd_) {}
		IType(const ASTNode *imm_):
			HasImmediate(imm_), TwoRegs(TypedReg(), TypedReg()) {}
		IType(const TypedReg &rs_, const TypedReg &rd_, const TypedImmediate &imm_):
			HasImmediate(imm_), TwoRegs(rs_, rd_) {}
		IType(const TypedImmediate &imm_):
			HasImmediate(imm_), TwoRegs(TypedReg(), TypedReg()) {}
	};

	struct JType: HasOpcode, HasImmediate, HasRS, HasCondition, HasLink {
		JType(const ASTNode *cond, const ASTNode *colons, const ASTNode *addr, const ASTNode *rs_):
			HasImmediate(getImmediate(addr)), HasRS(rs_), HasCondition(cond), HasLink(colons) {}
		JType(Condition cond, bool link_, const TypedImmediate &addr, const TypedReg &rs_):
			HasImmediate(addr), HasRS(rs_), HasCondition(cond), HasLink(link_) {}
	};

	/** For math and logical operations. */
	struct RNode: WASMInstructionNode, RType, HasOper {
		RNode(ASTNode *rs_, ASTNode *oper_, ASTNode *rt_, ASTNode *rd_);
		RNode(const TypedReg &rs_, const std::string *oper_, const TypedReg &rt_, const TypedReg &rd_, int oper_token);
		WASMInstructionNode * copy() const override;
		Opcode getOpcode() const override;
		Funct getFunct() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::RType; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	/** For math and logical operations. */
	struct INode: WASMInstructionNode, IType, HasOper {
		INode(ASTNode *rs_, ASTNode *oper_, ASTNode *imm, ASTNode *rd_);
		INode(const TypedReg &rs_, const std::string *oper_, const TypedImmediate &imm_, const TypedReg &rd_,
		      int oper_token);
		Opcode getOpcode() const override;
		WASMInstructionNode * copy() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::IType; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMMemoryNode: WASMInstructionNode, RType {
		WASMMemoryNode(int sym, ASTNode *rs_, ASTNode *rd_);
		WASMMemoryNode(int sym, const TypedReg &rs_, const TypedReg &rd_);
	};

	struct WASMCopyNode: WASMMemoryNode {
		WASMCopyNode(ASTNode *rs_, ASTNode *rd_);
		WASMCopyNode(const TypedReg &rs_, const TypedReg &PTHREAD_RWLOCK_DEFAULT_NP);
		Opcode getOpcode() const override { return OPCODES.at("c"); }
		Funct getFunct() const override { return FUNCTS.at("c"); }
		WASMInstructionNode * copy() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::Copy; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMLoadNode: WASMMemoryNode {
		WASMLoadNode(ASTNode *rs_, ASTNode *rd_);
		WASMLoadNode(const TypedReg &rs_, const TypedReg &rd_);
		Opcode getOpcode() const override { return OPCODES.at("l"); }
		Funct getFunct() const override { return FUNCTS.at("l"); }
		WASMInstructionNode * copy() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::Load; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMStoreNode: WASMMemoryNode {
		WASMStoreNode(ASTNode *rs_, ASTNode *rd_);
		WASMStoreNode(const TypedReg &rs_, const TypedReg &rd_);
		Opcode getOpcode() const override { return OPCODES.at("s"); }
		Funct getFunct() const override { return FUNCTS.at("s"); }
		WASMInstructionNode * copy() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::Store; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMSetNode: WASMInstructionNode, IType {
		WASMSetNode(ASTNode *imm_, ASTNode *rd_);
		WASMSetNode(const TypedImmediate &imm_, const TypedReg &rd_);
		Opcode getOpcode() const override { return OPCODES.at("set"); }
		WASMInstructionNode * copy() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::Set; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMLiNode: WASMInstructionNode, IType {
		bool isByte;

		WASMLiNode(ASTNode *imm_, ASTNode *rd_);
		WASMLiNode(const TypedImmediate &imm_, const TypedReg &rd_);
		Opcode getOpcode() const override { return OPCODES.at(isByte? "lbi" : "li"); }
		WASMInstructionNode * copy() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::Li; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMSiNode: WASMInstructionNode, IType {
		bool isByte;

		WASMSiNode(ASTNode *rs_, ASTNode *imm_);
		WASMSiNode(const TypedImmediate &imm_, const TypedReg &rd_);
		Opcode getOpcode() const override { return OPCODES.at(isByte? "sbi" : "si"); }
		WASMInstructionNode * copy() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::Si; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMLniNode: WASMLiNode {
		WASMLniNode(ASTNode *imm_, ASTNode *rd_);
		WASMLniNode(const TypedImmediate &imm_, const TypedReg &rd_);
		Opcode getOpcode() const override { return OPCODES.at(isByte? "lbni" : "lni"); }
		WASMInstructionNode * copy() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::Lni; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMCmpNode: WASMInstructionNode, RType {
		WASMCmpNode(ASTNode *rs_, ASTNode *rt_);
		WASMCmpNode(const TypedReg &rs_, const TypedReg &rt_);
		WASMInstructionNode * copy() const override { return (new WASMCmpNode(rs, rt))->absorb(*this); }
		Opcode getOpcode() const override { return OPCODES.at("cmp"); }
		Funct getFunct() const override { return FUNCTS.at("cmp"); }
		WASMNodeType nodeType() const override { return WASMNodeType::Cmp; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMCmpiNode: WASMInstructionNode, IType {
		WASMCmpiNode(ASTNode *rs_, ASTNode *imm_);
		WASMCmpiNode(const TypedReg &rs_, const TypedImmediate &imm_);
		WASMInstructionNode * copy() const override { return (new WASMCmpiNode(rs, imm))->absorb(*this); }
		Opcode getOpcode() const override { return OPCODES.at("cmpi"); }
		WASMNodeType nodeType() const override { return WASMNodeType::Cmpi; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMSelNode: WASMInstructionNode, RType, HasCondition {
		WASMSelNode(ASTNode *rs_, ASTNode *oper_, ASTNode *rt_, ASTNode *rd_);
		WASMSelNode(const TypedReg &rs_, Condition cond, const TypedReg &rt_, const TypedReg &rd_);
		Opcode getOpcode() const override { return OPCODES.at("sel"); }
		Funct getFunct() const override { return FUNCTS.at("sel"); }
		WASMInstructionNode * copy() const override { return (new WASMSelNode(rs, condition, rt, rd))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Sel; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMJNode: WASMInstructionNode, JType {
		WASMJNode(ASTNode *cond, ASTNode *colons, ASTNode *addr_);
		WASMJNode(const TypedImmediate &addr, bool link_ = false, Condition cond = Condition::None);
		Opcode getOpcode() const override { return OPCODES.at("j"); }
		WASMInstructionNode * copy() const override { return (new WASMJNode(imm, link, condition))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::J; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMJcNode: WASMInstructionNode, JType {
		WASMJcNode(WASMJNode *, ASTNode *rs_);
		WASMJcNode(const TypedImmediate &imm_, bool link_, const TypedReg &rs_);
		Opcode getOpcode() const override { return OPCODES.at("jc"); }
		WASMInstructionNode * copy() const override { return (new WASMJcNode(imm, link, rs))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Jc; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	// Used for both jr and jrl.
	struct WASMJrNode: WASMInstructionNode, RType, HasCondition, HasLink {
		WASMJrNode(ASTNode *cond, ASTNode *colons, ASTNode *rd_);
		WASMJrNode(Condition condition_, bool link_, const TypedReg &rd_);
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
		WASMJrcNode(bool link_, const TypedReg &rs_, const TypedReg &rd_);
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
		TypedReg rs;

		WASMJeqNode(WASMJNode *, ASTNode *rs_, ASTNode *rt_);
		WASMJeqNode(WASMJrNode *, ASTNode *rs_, ASTNode *rt_);
		WASMJeqNode(const Either &addr_, bool link_, const TypedReg &rs_, const Either &rt_);
		WASMInstructionNode * copy() const override { return (new WASMJeqNode(addr, link, rs, rt))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Jeq; }
		size_t expandedSize() const override;
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	// Used for both sspush and sspop.
	struct WASMSizedStackNode: WASMInstructionNode, IType {
		bool isPush;
		WASMSizedStackNode(ASTNode *size, ASTNode *rs_, bool is_push);
		WASMSizedStackNode(int size, const TypedReg &rs_, bool is_push);
		Opcode getOpcode() const override { return OPCODES.at(isPush? "sspush" : "sspop"); }
		WASMInstructionNode * copy() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::SizedStack; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMMultRNode: WASMInstructionNode, RType {
		WASMMultRNode(ASTNode *rs_, ASTNode *rt_);
		WASMMultRNode(const TypedReg &rs_, const TypedReg &rt_);
		Opcode getOpcode() const override { return OPCODES.at("mult"); }
		Funct getFunct() const override { return FUNCTS.at("mult"); }
		WASMInstructionNode * copy() const override { return (new WASMMultRNode(rs, rt))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::MultR; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMMultINode: WASMInstructionNode, IType {
		WASMMultINode(ASTNode *rs_, ASTNode *imm_);
		WASMMultINode(const TypedReg &rs_, const TypedImmediate &imm_);
		Opcode getOpcode() const override { return OPCODES.at("multi"); }
		WASMInstructionNode * copy() const override { return (new WASMMultINode(rs, imm))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::MultI; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMDiviINode: WASMInstructionNode, IType {
		WASMDiviINode(ASTNode *imm_, ASTNode *rs_, ASTNode *rd_);
		WASMDiviINode(const TypedImmediate &imm_, const TypedReg &rs_, const TypedReg &rd_);
		Opcode getOpcode() const override { return OPCODES.at("divii"); }
		WASMInstructionNode * copy() const override;
		WASMNodeType nodeType() const override { return WASMNodeType::DiviI; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMLuiNode: WASMInstructionNode, IType {
		WASMLuiNode(ASTNode *imm_, ASTNode *rd_);
		WASMLuiNode(const TypedImmediate &imm_, const TypedReg &rd_);
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
		WASMStackNode(const TypedReg &reg, bool is_push);
		Opcode getOpcode() const override { return OPCODES.at(isPush? "spush" : "spop"); }
		Funct getFunct() const override { return FUNCTS.at(isPush? "spush" : "spop"); }
		WASMInstructionNode * copy() const override { return (new WASMStackNode(rs, isPush))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Stack; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	/** Used for both tpush and tpop. Because I'm lazy, rs and rd are set to the same register. */
	struct WASMTypedStackNode: WASMInstructionNode, RType {
		bool isPush;
		WASMTypedStackNode(ASTNode *reg, bool is_push);
		WASMTypedStackNode(const TypedReg &reg, bool is_push);
		Opcode getOpcode() const override { return OPCODES.at(isPush? "tpush" : "tpop"); }
		Funct getFunct() const override { return FUNCTS.at(isPush? "tpush" : "tpop"); }
		WASMInstructionNode * copy() const override { return (new WASMTypedStackNode(rs, isPush))->absorb(*this); }
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
		WASMIntINode(const TypedImmediate &imm_);
		Opcode getOpcode() const override { return OPCODES.at("int"); }
		WASMInstructionNode * copy() const override { return (new WASMIntINode(imm))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::IntI; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMRitINode: WASMInstructionNode, IType {
		WASMRitINode(ASTNode *imm_);
		WASMRitINode(const TypedImmediate &imm_);
		Opcode getOpcode() const override { return OPCODES.at("rit"); }
		WASMInstructionNode * copy() const override { return (new WASMRitINode(imm))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::RitI; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMTimeINode: WASMInstructionNode, IType {
		WASMTimeINode(ASTNode *imm_);
		WASMTimeINode(const TypedImmediate &imm_);
		Opcode getOpcode() const override { return OPCODES.at("timei"); }
		WASMInstructionNode * copy() const override { return (new WASMTimeINode(imm))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::TimeI; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMTimeRNode: WASMInstructionNode, RType {
		WASMTimeRNode(ASTNode *rs_);
		WASMTimeRNode(const TypedReg &rs_);
		Opcode getOpcode() const override { return OPCODES.at("time"); }
		Funct getFunct() const override { return FUNCTS.at("time"); }
		WASMInstructionNode * copy() const override { return (new WASMTimeRNode(rs))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::TimeR; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMSvtimeNode: WASMInstructionNode, RType {
		WASMSvtimeNode(ASTNode *rd_);
		WASMSvtimeNode(const TypedReg &rd_);
		Opcode getOpcode() const override { return OPCODES.at("time"); }
		Funct getFunct() const override { return FUNCTS.at("svtime"); }
		WASMInstructionNode * copy() const override { return (new WASMSvtimeNode(rd))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Svtime; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMRingINode: WASMInstructionNode, IType {
		WASMRingINode(ASTNode *imm_);
		WASMRingINode(const TypedImmediate &imm_);
		Opcode getOpcode() const override { return OPCODES.at("ringi"); }
		WASMInstructionNode * copy() const override { return (new WASMRingINode(imm))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::RingI; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMRingRNode: WASMInstructionNode, RType {
		WASMRingRNode(ASTNode *rs_);
		WASMRingRNode(const TypedReg &rs_);
		Opcode getOpcode() const override { return OPCODES.at("ring"); }
		Funct getFunct() const override { return FUNCTS.at("ring"); }
		WASMInstructionNode * copy() const override { return (new WASMRingRNode(rs))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::RingR; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMSvringNode: WASMInstructionNode, RType {
		WASMSvringNode(ASTNode *rd_);
		WASMSvringNode(const TypedReg &rd_);
		Opcode getOpcode() const override { return OPCODES.at("ring"); }
		Funct getFunct() const override { return FUNCTS.at("svring"); }
		WASMInstructionNode * copy() const override { return (new WASMSvringNode(rd))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Svring; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMPrintNode: WASMInstructionNode, RType {
		PrintType type;

		WASMPrintNode(ASTNode *rs_, ASTNode *type_);
		WASMPrintNode(const TypedReg &rs_, PrintType type_);
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
		WASMSleepRNode(const TypedReg &rs_);
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
		WASMSetptRNode(ASTNode *rs_, ASTNode *rt_ = nullptr);
		WASMSetptRNode(const TypedReg &rs_, const TypedReg &rt_ = {});
		Opcode getOpcode() const override { return OPCODES.at("setpt"); }
		Funct getFunct() const override { return FUNCTS.at("setpt"); }
		WASMInstructionNode * copy() const override { return (new WASMSetptRNode(rs, rt))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::SetptR; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	// Pseudoinstruction; not handled by Assembler::compileInstruction
	struct WASMMvNode: WASMInstructionNode {
		TypedReg rs;
		TypedReg rd;
		WASMMvNode(ASTNode *rs_, ASTNode *rd_);
		WASMMvNode(const TypedReg &rs_, const TypedReg &rd_);
		WASMInstructionNode * copy() const override { return (new WASMMvNode(rs, rd))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Mv; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMSvpgNode: WASMInstructionNode, RType {
		WASMSvpgNode(ASTNode *rd_);
		WASMSvpgNode(const TypedReg &rd_);
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
		WASMQueryNode(QueryType, const TypedReg &rd_);
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
		WASMPseudoPrintNode(const TypedImmediate &imm_);
		WASMInstructionNode * copy() const override { return (new WASMPseudoPrintNode(imm))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::PseudoPrint; }
		size_t expandedSize() const override;
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
		size_t expandedSize() const override;
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMRestNode: WASMInstructionNode, RType {
		WASMRestNode();
		Opcode getOpcode() const override { return OPCODES.at("ext"); }
		Funct getFunct() const override { return FUNCTS.at("rest"); }
		WASMInstructionNode * copy() const override { return (new WASMRestNode())->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Rest; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMIONode: WASMInstructionNode, RType {
		const std::string *ident;

		WASMIONode(const std::string *ident_);
		Opcode getOpcode() const override { return OPCODES.at("ext"); }
		Funct getFunct() const override { return FUNCTS.at("io"); }
		WASMInstructionNode * copy() const override { return (new WASMIONode(ident))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::IO; }
		size_t expandedSize() const override { return ident? 2 : 1; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMInterruptsNode: WASMInstructionNode, RType {
		const bool enable;

		WASMInterruptsNode(bool enable_);
		Opcode getOpcode() const override { return OPCODES.at("di"); }
		Funct getFunct() const override { return FUNCTS.at(enable? "ei" : "di"); }
		WASMInstructionNode * copy() const override { return (new WASMInterruptsNode(enable))->absorb(*this); }
		WASMNodeType nodeType() const override { return WASMNodeType::Interrupts; }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	/** Covers a number of inverse immediate instructions (sllii, srlii, sraii). */
	class WASMInverseNode: public WASMInstructionNode, public IType {
		private:
			std::string getOper() const;

		public:
			enum class Type {Sllii, Srlii, Sraii};
			Type type;

			WASMInverseNode(ASTNode *imm_, ASTNode *rs_, ASTNode *rd_, Type);
			WASMInverseNode(const TypedImmediate &imm_, const TypedReg &rs_, const TypedReg &rd_, Type);
			Opcode getOpcode() const override;
			WASMInstructionNode * copy() const override { return (new WASMInverseNode(imm, rs, rd, type))->absorb(*this); }
			WASMNodeType nodeType() const override { return WASMNodeType::Inverse; }
			std::string debugExtra() const override;
			operator std::string() const override;
	};

	struct WASMTransNode: WASMInstructionNode, RType {
		WASMTransNode(const ASTNode *rs_, const ASTNode *rd_);
		WASMTransNode(const TypedReg &rs_, const TypedReg &rd_);
		Opcode getOpcode() const override { return OP_TRANS; }
		Funct getFunct() const override { return FUNCTS.at("trans"); }
		WASMInstructionNode * copy() const override { return (new WASMTransNode(rs, rd))->absorb(*this); }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMPageStackNode: WASMInstructionNode, RType {
		bool isPush;
		WASMPageStackNode(bool is_push, const ASTNode *rs_ = nullptr);
		WASMPageStackNode(bool is_push, const TypedReg &rs_);
		Opcode getOpcode() const override { return OP_PPUSH; }
		Funct getFunct() const override { return FUNCTS.at(isPush? "ppush" : "ppop"); }
		WASMInstructionNode * copy() const override { return (new WASMPageStackNode(isPush, rs))->absorb(*this); }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	struct WASMCtlbNode: WASMInstructionNode, RType {
		WASMCtlbNode();
		Opcode getOpcode() const override { return OP_CTLB; }
		Funct getFunct() const override { return FUNCTS.at("ctlb"); }
		WASMInstructionNode * copy() const override { return (new WASMCtlbNode)->absorb(*this); }
		std::string debugExtra() const override;
		operator std::string() const override;
	};

	RNode * makeSeq(const TypedReg &rs, const TypedReg &rt, const TypedReg &rd, int bang = -1);
}
