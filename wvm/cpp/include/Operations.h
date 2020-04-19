#ifndef WVM_OPERATIONS_H_
#define WVM_OPERATIONS_H_

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

	void decodeRType(VM &, UWord instruction, Word *&rs, Word *&rt, Word *&rd, Conditions &, int &flags, int &funct);
	void decodeIType(VM &, UWord instruction, Word *&rs, Word *&rd,  Conditions &, int &flags, HWord &immediate);
	void decodeJType(VM &, UWord instruction, Word *&rs, bool &link, Conditions &, int &flags, HWord &address);

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

	void sliOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);    // 25  I
	void sleiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);   // 26  I
	void seqiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);   // 27  I
	void sluiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);   // 28  I
	void sleuiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);  // 29  I
	void sgiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);    // 41  I
	void sgeiOp(VM &, Word &rs, Word &rd, Conditions, int flags, HWord immediate);   // 42  I

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
	void chOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);            // 18  R 8
	void lhOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);            // 18  R 9
	void shOp(VM &, Word &rs, Word &rt, Word &rd, Conditions, int flags);            // 18  R 10

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

#define OP_NOP 0
#define OP_RMATH 1
#define OP_RLOGIC 2
#define OP_RCOMP 14
#define OP_RJUMP 17
#define OP_RMEM 18
#define OP_REXT 31

#define OP_ADD 1
#define FN_ADD 0
#define OP_SUB 1
#define FN_SUB 1
#define OP_MULT 1
#define FN_MULT 2
#define OP_MULTU 1
#define FN_MULTU 5
#define OP_SLL 1
#define FN_SLL 6
#define OP_SRL 1
#define FN_SRL 7
#define OP_SRA 1
#define FN_SRA 8
#define OP_MOD 1
#define FN_MOD 9
#define OP_DIV 1
#define FN_DIV 10
#define OP_DIVU 1
#define FN_DIVU 11

#define OP_AND 2
#define FN_AND 0
#define OP_NAND 2
#define FN_NAND 1
#define OP_NOR 2
#define FN_NOR 2
#define OP_NOT 2
#define FN_NOT 3
#define OP_OR 2
#define FN_OR 4
#define OP_XNOR 2
#define FN_XNOR 5
#define OP_XOR 2
#define FN_XOR 6
#define OP_LAND 2
#define FN_LAND 8
#define OP_LNAND 2
#define FN_LNAND 9
#define OP_LNOR 2
#define FN_LNOR 10
#define OP_LNOT 2
#define FN_LNOT 11
#define OP_LOR 2
#define FN_LOR 12
#define OP_LXNOR 2
#define FN_LXNOR 13
#define OP_LXOR 2
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

#define OP_SL 14
#define FN_SL 0
#define OP_SLE 14
#define FN_SLE 1
#define OP_SEQ 14
#define FN_SEQ 2
#define OP_SLU 14
#define FN_SLU 3
#define OP_SLEU 14
#define FN_SLEU 4

#define OP_J 15
#define OP_JC 16

#define OP_JR 17
#define FN_JR 0
#define OP_JRC 17
#define FN_JRC 1
#define OP_JRL 17
#define FN_JRL 2
#define OP_JRLC 17
#define FN_JRLC 3

#define OP_C 18
#define FN_C 0
#define OP_L 18
#define FN_L 1
#define OP_S 18
#define FN_S 2
#define OP_CB 18
#define FN_CB 3
#define OP_LB 18
#define FN_LB 4
#define OP_SB 18
#define FN_SB 5
#define OP_SPUSH 18
#define FN_SPUSH 6
#define OP_SPOP 18
#define FN_SPOP 7
#define OP_CH 18
#define FN_CH 8
#define OP_LH 18
#define FN_LH 9
#define OP_SH 18
#define FN_SH 10

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

#define OP_PR 31
#define FN_PR 1
#define OP_HALT 31
#define FN_HALT 2
#define OP_EVAL 31
#define FN_EVAL 3
#define OP_PRC 31
#define FN_PRC 4
#define OP_PRD 31
#define FN_PRD 5
#define OP_PRX 31
#define FN_PRX 6
#define OP_SLEEP 31
#define FN_SLEEP 7

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
#define OP_TIME 48
#define OP_TIMEI 49
#define OP_RING 50
#define OP_RINGI 51
#define OP_DIVI 52
#define OP_DIVUI 53
#define OP_DIVII 54
#define OP_DIVUII 55
}

#endif
