#ifndef WVM_OPERATIONS_H_
#define WVM_OPERATIONS_H_

#include <set>

#include "Defs.h"

namespace WVM::Operations {

	void execute(VM &, UWord &);

	void decodeRType(int &opcode, Word *&rs, Word *&rt, Word *&rd, Conditions &, int &flags, int &funct);
	void decodeIType(int &opcode, Word *&rs, Word *&rd,  Conditions &, int &flags, HWord &immediate);
	void decodeJType(int &opcode, Word *&rs, bool &link, Conditions &, int &flags, HWord &address);

	extern std::set<int> RSet;
	extern std::set<int> ISet;
	extern std::set<int> JSet;

	void nopOp(VM &, UWord instruction);    // 0   X

	void addOp(VM &, UWord instruction);    // 1   R 0
	void subOp(VM &, UWord instruction);    // 1   R 1
	void multOp(VM &, UWord instruction);   // 1   R 2
	// 1.3
	// 1.4
	void multuOp(VM &, UWord instruction);  // 1   R 5
	void sllOp(VM &, UWord instruction);    // 1   R 6
	void srlOp(VM &, UWord instruction);    // 1   R 7
	void sraOp(VM &, UWord instruction);    // 1   R 8
	void modOp(VM &, UWord instruction);    // 1   R 9
	void divOp(VM &, UWord instruction);    // 1   R 10
	void divuOp(VM &, UWord instruction);   // 1   R 11

	void andOp(VM &, UWord instruction);    // 2   R 0
	void nandOp(VM &, UWord instruction);   // 2   R 1
	void norOp(VM &, UWord instruction);    // 2   R 2
	void notOp(VM &, UWord instruction);    // 2   R 3
	void orOp(VM &, UWord instruction);     // 2   R 4
	void xnorOp(VM &, UWord instruction);   // 2   R 5
	void xorOp(VM &, UWord instruction);    // 2   R 6
	void landOp(VM &, UWord instruction);   // 2   R 8
	void lnandOp(VM &, UWord instruction);  // 2   R 9
	void lnorOp(VM &, UWord instruction);   // 2   R 10
	void lnotOp(VM &, UWord instruction);   // 2   R 11
	void lorOp(VM &, UWord instruction);    // 2   R 12
	void lxnorOp(VM &, UWord instruction);  // 2   R 13
	void lxorOp(VM &, UWord instruction);   // 2   R 14

	void addiOp(VM &, UWord instruction);   // 3   I
	void subiOp(VM &, UWord instruction);   // 4   I
	void multiOp(VM &, UWord instruction);  // 5   I
	// 22
	// 23
	void multuiOp(VM &, UWord instruction); // 24  I
	void slliOp(VM &, UWord instruction);   // 34  I
	void srliOp(VM &, UWord instruction);   // 35  I
	void sraiOp(VM &, UWord instruction);   // 36  I
	void modiOp(VM &, UWord instruction);   // 30  I
	void diviOp(VM &, UWord instruction);   // 52  I
	void divuiOp(VM &, UWord instruction);  // 53  I
	void diviiOp(VM &, UWord instruction);  // 54  I
	void divuiiOp(VM &, UWord instruction); // 55  I

	void andiOp(VM &, UWord instruction);   // 6   I
	void nandiOp(VM &, UWord instruction);  // 7   I
	void noriOp(VM &, UWord instruction);   // 8   I
	void oriOp(VM &, UWord instruction);    // 9   I
	void xnoriOp(VM &, UWord instruction);  // 10  I
	void xoriOp(VM &, UWord instruction);   // 11  I

	void luiOp(VM &, UWord instruction);    // 13  I
	void slOp(VM &, UWord instruction);     // 14  R 0
	void sleOp(VM &, UWord instruction);    // 14  R 1
	void seqOp(VM &, UWord instruction);    // 14  R 2
	void sluOp(VM &, UWord instruction);    // 14  R 3
	void sleuOp(VM &, UWord instruction);   // 14  R 4

	void sliOp(VM &, UWord instruction);    // 25  I
	void sleiOp(VM &, UWord instruction);   // 26  I
	void seqiOp(VM &, UWord instruction);   // 27  I
	void sluiOp(VM &, UWord instruction);   // 28  I
	void sleuiOp(VM &, UWord instruction);  // 29  I
	void sgiOp(VM &, UWord instruction);    // 41  I
	void sgeiOp(VM &, UWord instruction);   // 42  I

	void jOp(VM &, UWord instruction);      // 15  J
	void jcOp(VM &, UWord instruction);     // 16  J

	void jrOp(VM &, UWord instruction);     // 17  R 0
	void jrcOp(VM &, UWord instruction);    // 17  R 1
	void jrlOp(VM &, UWord instruction);    // 17  R 2
	void jrlcOp(VM &, UWord instruction);   // 17  R 3

	void cOp(VM &, UWord instruction);      // 18  R 0
	void lOp(VM &, UWord instruction);      // 18  R 1
	void sOp(VM &, UWord instruction);      // 18  R 2
	void cbOp(VM &, UWord instruction);     // 18  R 3
	void lbOp(VM &, UWord instruction);     // 18  R 4
	void sbOp(VM &, UWord instruction);     // 18  R 5
	void spushOp(VM &, UWord instruction);  // 18  R 6
	void spopOp(VM &, UWord instruction);   // 18  R 7
	void chOp(VM &, UWord instruction);     // 18  R 8
	void lhOp(VM &, UWord instruction);     // 18  R 9
	void shOp(VM &, UWord instruction);     // 18  R 10

	void liOp(VM &, UWord instruction);     // 19  I
	void siOp(VM &, UWord instruction);     // 20  I
	void setOp(VM &, UWord instruction);    // 21  I
	void lbiOp(VM &, UWord instruction);    // 37  I
	void sbiOp(VM &, UWord instruction);    // 38  I
	void lniOp(VM &, UWord instruction);    // 39  I
	void lbniOp(VM &, UWord instruction);   // 40  I

	void intOp(VM &, UWord instruction);    // 32  I
	void ritOp(VM &, UWord instruction);    // 33  I
	void timeOp(VM &, UWord instruction);   // 48  R
	void timeiOp(VM &, UWord instruction);  // 49  I
	void ringOp(VM &, UWord instruction);   // 50  R
	void ringiOp(VM &, UWord instruction);  // 51  I

	void prOp(VM &, UWord instruction);     // 31 ?R 1
	void haltOp(VM &, UWord instruction);   // 31 ?R 2
	void evalOp(VM &, UWord instruction);   // 31 ?R 3
	void prcOp(VM &, UWord instruction);    // 31 ?R 4
	void prdOp(VM &, UWord instruction);    // 31 ?R 5
	void prxOp(VM &, UWord instruction);    // 31 ?R 6
	void sleepOp(VM &, UWord instruction);  // 31 ?R 7

#define OP_NOP 0

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
#define FN_OP 9
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
