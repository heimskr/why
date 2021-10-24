#pragma once

#include <map>
#include <set>

#include "Defs.h"

namespace WVM::Operations {

	extern std::set<int> RSet;
	extern std::set<int> ISet;
	extern std::set<int> JSet;

	void execute(VM &, UWord);
	void executeRType(int opcode, VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags, int funct);
	void executeIType(int opcode, VM &, Word &rs, Word &rd,  Conditions, int flags, HWord immediate);
	void executeJType(int opcode, VM &, Word &rs, bool link, Conditions, int flags, HWord address);

	void decodeRType(UWord instruction, int &rs, int &rt, int &rd, Conditions &, int &flags, int &funct);
	void decodeIType(UWord instruction, int &rs, int &rd,  Conditions &, int &flags, HWord &immediate);
	void decodeJType(UWord instruction, int &rs, bool &link, Conditions &, int &flags, HWord &address);

	void addOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 1   R 0
	void subOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 1   R 1
	void multOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);          // 1   R 2
	// 1.3
	// 1.4
	void multuOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);         // 1   R 5
	void sllOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 1   R 6
	void srlOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 1   R 7
	void sraOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 1   R 8
	void modOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 1   R 9
	void divOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 1   R 10
	void divuOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);          // 1   R 11
	void moduOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);          // 1   R 12

	void andOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 2   R 0
	void nandOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);          // 2   R 1
	void norOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 2   R 2
	void notOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 2   R 3
	void orOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);            // 2   R 4
	void xnorOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);          // 2   R 5
	void xorOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 2   R 6
	void landOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);          // 2   R 8
	void lnandOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);         // 2   R 9
	void lnorOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);          // 2   R 10
	void lnotOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);          // 2   R 11
	void lorOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 2   R 12
	void lxnorOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);         // 2   R 13
	void lxorOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);          // 2   R 14

	void addiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);   // 3   I
	void subiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);   // 4   I
	void multiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);  // 5   I
	// 22
	// 23
	void multuiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate); // 24  I
	void slliOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);   // 34  I
	void srliOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);   // 35  I
	void sraiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);   // 36  I
	void modiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);   // 30  I
	void diviOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);   // 52  I
	void divuiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);  // 53  I
	void diviiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);  // 54  I
	void divuiiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate); // 55  I
	void moduiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);  // 67  I

	void andiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);   // 6   I
	void nandiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);  // 7   I
	void noriOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);   // 8   I
	void oriOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);    // 9   I
	void xnoriOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);  // 10  I
	void xoriOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);   // 11  I

	void luiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);    // 13  I
	void slOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);            // 14  R 0
	void sleOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 14  R 1
	void seqOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 14  R 2
	void sluOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 14  R 3
	void sleuOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);          // 14  R 4
	void cmpOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 14  R 5

	void sliOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);    // 25  I
	void sleiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);   // 26  I
	void seqiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);   // 27  I
	void sluiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);   // 28  I
	void sleuiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);  // 29  I
	void sgiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);    // 41  I
	void sgeiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);   // 42  I
	void sgeuiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);  // 59  I
	void sguiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);   // 60  I
	void cmpiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);   // 43  I

	void jOp(VM &, Word &rs, bool link, Conditions, int flags, HWord address);       // 15  J
	void jcOp(VM &, Word &rs, bool link, Conditions, int flags, HWord address);      // 16  J

	void jrOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);            // 17  R 0
	void jrcOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 17  R 1
	void jrlOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 17  R 2
	void jrlcOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);          // 17  R 3

	void cOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);             // 18  R 0
	void lOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);             // 18  R 1
	void sOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);             // 18  R 2
	void cbOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);            // 18  R 3
	void lbOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);            // 18  R 4
	void sbOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);            // 18  R 5
	void spushOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);         // 18  R 6
	void spopOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);          // 18  R 7
	void sspushOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate); // 57  I
	void sspopOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);  // 58  I
	void chOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);            // 18  R 8
	void lhOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);            // 18  R 9
	void shOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);            // 18  R 10
	void msOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);            // 18  R 11
	void csOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);            // 18  R 12
	void lsOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);            // 18  R 13
	void ssOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);            // 18  R 14

	void liOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);     // 19  I
	void siOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);     // 20  I
	void setOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);    // 21  I
	void lbiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);    // 37  I
	void sbiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);    // 38  I
	void lniOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);    // 39  I
	void lbniOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);   // 40  I

	void intOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);    // 32  I
	void ritOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);    // 33  I
	void timeOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);          // 48  R
	void timeiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);  // 49  I
	void ringOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);          // 50  R
	void ringiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);  // 51  I

	void prOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);            // 31 ?R 1
	void haltOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);          // 31 ?R 2
	void evalOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);          // 31 ?R 3
	void prcOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 31 ?R 4
	void prdOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 31 ?R 5
	void prxOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 31 ?R 6
	void sleepOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);         // 31 ?R 7
	void prbOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 31 ?R 8
	void restOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);          // 31 ?R 9
	void ioOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);            // 31 ?R 10

	void selOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);           // 56  R
	void pgoffOp(VM &, Word &, Word &, Word &, Conditions, int flags);               // 61  R 0
	void pgonOp(VM &, Word &, Word &, Word &, Conditions, int flags);                // 61  R 1
	void setptOp(VM &, Word &rs, Word &, Word &, Conditions, int flags);             // 61  R 2
	void svpgOp(VM &, Word &, Word &, Word &rd, Conditions, int flags);              // 61  R 3
	void qmOp(VM &, Word &, Word &, Word &rd, Conditions, int flags);                // 65  R 0
	void diOp(VM &, Word &, Word &, Word &rd, Conditions, int flags);                // 66  R 0
	void eiOp(VM &, Word &, Word &, Word &rd, Conditions, int flags);                // 66  R 1

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
#define OP_MULTU OP_RMATH
#define FN_MULTU 5
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
#define OP_DIVU OP_RMATH
#define FN_DIVU 11
#define OP_MODU OP_RMATH
#define FN_MODU 12

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
#define OP_SLU OP_RCOMP
#define FN_SLU 3
#define OP_SLEU OP_RCOMP
#define FN_SLEU 4
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

#define OP_MULTUI 24
#define OP_SLI 25
#define OP_SLEI 26
#define OP_SEQI 27
#define OP_SLUI 28
#define OP_SLEUI 29
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
#define OP_LBI 37
#define OP_SBI 38
#define OP_LNI 39
#define OP_LBNI 40
#define OP_SGI 41
#define OP_SGEI 42
#define OP_CMPI 43
#define OP_TIME 48
#define OP_TIMEI 49
#define OP_RING 50
#define OP_RINGI 51
#define OP_DIVI 52
#define OP_DIVUI 53
#define OP_DIVII 54
#define OP_DIVUII 55
#define OP_SEL 56
#define OP_SSPUSH 57
#define OP_SSPOP 58
#define OP_SGEUI 59
#define OP_SGUI 60

#define OP_PAGE  61
#define FN_PGOFF 0
#define FN_PGON  1
#define FN_SETPT 2
#define FN_SVPG  3

#define OP_QUERY 65
#define OP_QM OP_QUERY
#define FN_QM 0

#define OP_INTERRUPTS 66
#define FN_DI 0
#define FN_EI 1

#define OP_MODUI 67

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
