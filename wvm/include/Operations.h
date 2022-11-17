#pragma once

#include <map>
#include <set>

#include "Defs.h"
#include "Register.h"
#include "wasm/Types.h"

namespace WVM::Operations {

	extern std::set<int> RSet;
	extern std::set<int> ISet;
	extern std::set<int> JSet;

	struct RArgs {
		VM &vm;
		Register &rs;
		Register &rt;
		Register &rd;
		Conditions conditions;
		int flags;
		int function;
		int rsType;
		int rtType;
		int rdType;
		RArgs(VM &vm_, Register &rs_, Register &rt_, Register &rd_, Conditions conditions_, int flags_, int function_,
		int rs_type, int rt_type, int rd_type):
			vm(vm_), rs(rs_), rt(rt_), rd(rd_), conditions(conditions_), flags(flags_), function(function_),
			rsType(rs_type), rtType(rt_type), rdType(rd_type) {}
	};

	struct IArgs {
		VM &vm;
		Register &rs;
		Register &rd;
		Conditions conditions;
		int flags;
		HWord immediate;
		int rsType;
		int rdType;
		int immType;
		IArgs(VM &vm_, Register &rs_, Register &rd_, Conditions conditions_, int flags_, HWord immediate_, int rs_type,
		      int rd_type, int imm_type):
			vm(vm_), rs(rs_), rd(rd_), conditions(conditions_), flags(flags_), immediate(immediate_),
			rsType(rs_type), rdType(rd_type), immType(imm_type) {}
	};

	struct JArgs {
		VM &vm;
		Register &rs;
		bool link;
		Conditions conditions;
		int flags;
		HWord address;
		int rsType;
		JArgs(VM &vm_, Register &rs_, bool link_, Conditions conditions_, int flags_, HWord address_, int rs_type):
			vm(vm_), rs(rs_), link(link_), conditions(conditions_), flags(flags_), address(address_), rsType(rs_type) {}
	};

	void execute(VM &, Wasmc::TypedInstruction);
	void executeRType(int opcode, RArgs &);
	void executeIType(int opcode, IArgs &);
	void executeJType(int opcode, JArgs &);

	void decodeRType(Wasmc::TypedInstruction, int &rs, int &rt, int &rd, Conditions &, int &flags, int &funct,
	                 int &st, int &tt, int &dt);
	void decodeIType(Wasmc::TypedInstruction, int &rs, int &rd,  Conditions &, int &flags, HWord &immediate,
	                 int &st, int &rt, int &it);
	void decodeJType(Wasmc::TypedInstruction, int &rs, bool &link, Conditions &, int &flags, HWord &address, int &st);

	void addOp(RArgs &);    // 1   R 0
	void subOp(RArgs &);    // 1   R 1
	void multOp(RArgs &);   // 1   R 2
	// 1.3
	// 1.4
	void sllOp(RArgs &);    // 1   R 6
	void srlOp(RArgs &);    // 1   R 7
	void sraOp(RArgs &);    // 1   R 8
	void modOp(RArgs &);    // 1   R 9
	void divOp(RArgs &);    // 1   R 10
	void sextOp(RArgs &);   // 1   R 13

	void andOp(RArgs &);    // 2   R 0
	void nandOp(RArgs &);   // 2   R 1
	void norOp(RArgs &);    // 2   R 2
	void notOp(RArgs &);    // 2   R 3
	void orOp(RArgs &);     // 2   R 4
	void xnorOp(RArgs &);   // 2   R 5
	void xorOp(RArgs &);    // 2   R 6
	void landOp(RArgs &);   // 2   R 8
	void lnandOp(RArgs &);  // 2   R 9
	void lnorOp(RArgs &);   // 2   R 10
	void lnotOp(RArgs &);   // 2   R 11
	void lorOp(RArgs &);    // 2   R 12
	void lxnorOp(RArgs &);  // 2   R 13
	void lxorOp(RArgs &);   // 2   R 14

	void addiOp(IArgs &);   // 3   I
	void subiOp(IArgs &);   // 4   I
	void multiOp(IArgs &);  // 5   I
	// 22
	// 23
	void slliOp(IArgs &);   // 34  I
	void srliOp(IArgs &);   // 35  I
	void sraiOp(IArgs &);   // 36  I
	void modiOp(IArgs &);   // 30  I
	void diviOp(IArgs &);   // 52  I
	void diviiOp(IArgs &);  // 54  I
	void slliiOp(IArgs &);  // 62  I
	void srliiOp(IArgs &);  // 63  I
	void sraiiOp(IArgs &);  // 64  I

	void andiOp(IArgs &);   // 6   I
	void nandiOp(IArgs &);  // 7   I
	void noriOp(IArgs &);   // 8   I
	void oriOp(IArgs &);    // 9   I
	void xnoriOp(IArgs &);  // 10  I
	void xoriOp(IArgs &);   // 11  I

	void luiOp(IArgs &);    // 13  I
	void slOp(RArgs &);     // 14  R 0
	void sleOp(RArgs &);    // 14  R 1
	void seqOp(RArgs &);    // 14  R 2
	void cmpOp(RArgs &);    // 14  R 5

	void sliOp(IArgs &);    // 25  I
	void sleiOp(IArgs &);   // 26  I
	void seqiOp(IArgs &);   // 27  I
	void sgiOp(IArgs &);    // 41  I
	void sgeiOp(IArgs &);   // 42  I
	void cmpiOp(IArgs &);   // 43  I

	void jOp(JArgs &);      // 15  J
	void jcOp(JArgs &);     // 16  J

	void jrOp(RArgs &);     // 17  R 0
	void jrcOp(RArgs &);    // 17  R 1
	void jrlOp(RArgs &);    // 17  R 2
	void jrlcOp(RArgs &);   // 17  R 3

	void cOp(RArgs &);      // 18  R 0
	void lOp(RArgs &);      // 18  R 1
	void sOp(RArgs &);      // 18  R 2
	void spushOp(RArgs &);  // 18  R 6
	void spopOp(RArgs &);   // 18  R 7
	void transOp(RArgs &);  // 68  R 0

	void liOp(IArgs &);     // 19  I
	void siOp(IArgs &);     // 20  I
	void setOp(IArgs &);    // 21  I
	void lniOp(IArgs &);    // 39  I

	void intOp(IArgs &);    // 32  I
	void ritOp(IArgs &);    // 33  I
	void timeOp(RArgs &);   // 48  R 0
	void svtimeOp(RArgs &); // 48  R 1
	void timeiOp(IArgs &);  // 49  I
	void ringOp(RArgs &);   // 50  R 0
	void svringOp(RArgs &); // 50  R 1
	void ringiOp(IArgs &);  // 51  I

	void prOp(RArgs &);     // 31 ?R 1
	void haltOp(RArgs &);   // 31 ?R 2
	void evalOp(RArgs &);   // 31 ?R 3
	void prcOp(RArgs &);    // 31 ?R 4
	void prdOp(RArgs &);    // 31 ?R 5
	void prxOp(RArgs &);    // 31 ?R 6
	void sleepOp(RArgs &);  // 31 ?R 7
	void prbOp(RArgs &);    // 31 ?R 8
	void restOp(RArgs &);   // 31 ?R 9
	void ioOp(RArgs &);     // 31 ?R 10

	void selOp(RArgs &);    // 56  R
	void pgoffOp(RArgs &);  // 61  R 0
	void pgonOp(RArgs &);   // 61  R 1
	void setptOp(RArgs &);  // 61  R 2
	void svpgOp(RArgs &);   // 61  R 3
	void ppushOp(RArgs &);  // 61  R 4
	void ppopOp(RArgs &);   // 61  R 5
	void qmOp(RArgs &);     // 65  R 0
	void diOp(RArgs &);     // 66  R 0
	void eiOp(RArgs &);     // 66  R 1

#define OP_NOP 0
#define OP_RMATH 1
#define OP_RLOGIC 2
#define OP_RCOMP 14
#define OP_RJUMP 17
#define OP_RMEM 18
#define OP_REXT 31

#define OP_ADD OP_RMATH
#define FN_ADD 0
#define OP_SUB OP_RMATH
#define FN_SUB 1
#define OP_MULT OP_RMATH
#define FN_MULT 2
#define OP_SLL OP_RMATH
#define FN_SLL 6
#define OP_SRL OP_RMATH
#define FN_SRL 7
#define OP_SRA OP_RMATH
#define FN_SRA 8
#define OP_MOD OP_RMATH
#define FN_MOD 9
#define OP_DIV OP_RMATH
#define FN_DIV 10
#define OP_SEXT OP_RMATH
#define FN_SEXT 13

#define OP_AND OP_RLOGIC
#define FN_AND 0
#define OP_NAND OP_RLOGIC
#define FN_NAND 1
#define OP_NOR OP_RLOGIC
#define FN_NOR 2
#define OP_NOT OP_RLOGIC
#define FN_NOT 3
#define OP_OR OP_RLOGIC
#define FN_OR 4
#define OP_XNOR OP_RLOGIC
#define FN_XNOR 5
#define OP_XOR OP_RLOGIC
#define FN_XOR 6
#define OP_LAND OP_RLOGIC
#define FN_LAND 8
#define OP_LNAND OP_RLOGIC
#define FN_LNAND 9
#define OP_LNOR OP_RLOGIC
#define FN_LNOR 10
#define OP_LNOT OP_RLOGIC
#define FN_LNOT 11
#define OP_LOR OP_RLOGIC
#define FN_LOR 12
#define OP_LXNOR OP_RLOGIC
#define FN_LXNOR 13
#define OP_LXOR OP_RLOGIC
#define FN_LXOR 14

#define OP_ADDI 3
#define OP_SUBI 4
#define OP_MULTI 5

#define OP_ANDI 6
#define OP_NANDI 7
#define OP_NORI 8
#define OP_ORI 9
#define OP_XNORI 10
#define OP_XORI 11

#define OP_LUI 13

#define OP_SL OP_RCOMP
#define FN_SL 0
#define OP_SLE OP_RCOMP
#define FN_SLE 1
#define OP_SEQ OP_RCOMP
#define FN_SEQ 2
#define OP_CMP OP_RCOMP
#define FN_CMP 5

#define OP_J 15
#define OP_JC 16

#define OP_JR OP_RJUMP
#define FN_JR 0
#define OP_JRC OP_RJUMP
#define FN_JRC 1
#define OP_JRL OP_RJUMP
#define FN_JRL 2
#define OP_JRLC OP_RJUMP
#define FN_JRLC 3

#define OP_C OP_RMEM
#define FN_C 0
#define OP_L OP_RMEM
#define FN_L 1
#define OP_S OP_RMEM
#define FN_S 2
#define OP_CB OP_RMEM
#define FN_CB 3
#define OP_LB OP_RMEM
#define FN_LB 4
#define OP_SB OP_RMEM
#define FN_SB 5
#define OP_SPUSH OP_RMEM
#define FN_SPUSH 6
#define OP_SPOP OP_RMEM
#define FN_SPOP 7
#define OP_CH OP_RMEM
#define FN_CH 8
#define OP_LH OP_RMEM
#define FN_LH 9
#define OP_SH OP_RMEM
#define FN_SH 10
#define OP_MS OP_RMEM
#define FN_MS 11
#define OP_CS OP_RMEM
#define FN_CS 12
#define OP_LS OP_RMEM
#define FN_LS 13
#define OP_SS OP_RMEM
#define FN_SS 14

#define OP_LI 19
#define OP_SI 20
#define OP_SET 21

#define OP_SLI 25
#define OP_SLEI 26
#define OP_SEQI 27
#define OP_MODI 30

#define OP_PR OP_REXT
#define FN_PR 1
#define OP_HALT OP_REXT
#define FN_HALT 2
#define OP_EVAL OP_REXT
#define FN_EVAL 3
#define OP_PRC OP_REXT
#define FN_PRC 4
#define OP_PRD OP_REXT
#define FN_PRD 5
#define OP_PRX OP_REXT
#define FN_PRX 6
#define OP_SLEEP OP_REXT
#define FN_SLEEP 7
#define OP_PRB OP_REXT
#define FN_PRB 8
#define OP_REST OP_REXT
#define FN_REST 9
#define OP_IO OP_REXT
#define FN_IO 10

#define OP_INT 32
#define OP_RIT 33
#define OP_SLLI 34
#define OP_SRLI 35
#define OP_SRAI 36
#define OP_LNI 39
#define OP_SGI 41
#define OP_SGEI 42
#define OP_CMPI 43
#define OP_TIME 48
#define FN_TIME 0
#define FN_SVTIME 1
#define OP_TIMEI 49
#define OP_RING 50
#define FN_RING 0
#define FN_SVRING 1
#define OP_RINGI 51
#define OP_DIVI 52
#define OP_DIVII 54
#define OP_SEL 56
#define OP_SLLII 62
#define OP_SRLII 63
#define OP_SRAII 64

#define OP_PAGE  61
#define FN_PGOFF 0
#define FN_PGON  1
#define FN_SETPT 2
#define FN_SVPG  3
#define FN_PPUSH 4
#define FN_PPOP  5

#define OP_QUERY 65
#define OP_QM OP_QUERY
#define FN_QM 0

#define OP_INTERRUPTS 66
#define FN_DI 0
#define FN_EI 1

#define OP_TRANS 68

#define INTERRUPT_MAX	3

#define INT_NULL		0
#define INT_NULL_FROM	-1
#define INT_NULL_TO		-1

#define INT_SYSTEM		1
#define INT_SYSTEM_FROM	0
#define INT_SYSTEM_TO	-1

#define INT_TIMER		2
#define INT_TIMER_FROM	-1
#define INT_TIMER_TO	0

#define INT_PROTEC		3
#define INT_PROTEC_FROM	0
#define INT_PROTEC_TO	2

#define IO_DEVCOUNT  0
#define IO_SEEKABS   1
#define IO_SEEKREL   2
#define IO_READ      3
#define IO_WRITE     4
#define IO_GETSIZE   5
#define IO_GETCURSOR 6
#define IO_GETNAME   7
}
