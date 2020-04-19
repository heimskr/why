#ifndef WVM_OPERATIONS_H_
#define WVM_OPERATIONS_H_

#include "Defs.h"

namespace WVM::Operations {

	void decodeRType(int &opcode, Word *&rs, Word *&rt, Word *&rd, Conditions &, int &flags, int &funct);
	void decodeIType(int &opcode, Word *&rs, Word *&rd,  Conditions &, int &flags, HWord &immediate);
	void decodeJType(int &opcode, Word *&rs, bool &link, Conditions &, int &flags, HWord &address);

	void nopOp(VM &, Word instruction);    // 0   X

	void addOp(VM &, Word instruction);    // 1   R 0
	void subOp(VM &, Word instruction);    // 1   R 1
	void multOp(VM &, Word instruction);   // 1   R 2
	// 1.3
	// 1.4
	void multuOp(VM &, Word instruction);  // 1   R 5
	void sllOp(VM &, Word instruction);    // 1   R 6
	void srlOp(VM &, Word instruction);    // 1   R 7
	void sraOp(VM &, Word instruction);    // 1   R 8
	void modOp(VM &, Word instruction);    // 1   R 9
	void divOp(VM &, Word instruction);    // 1   R 10
	void divuOp(VM &, Word instruction);   // 1   R 11

	void andOp(VM &, Word instruction);    // 2   R 0
	void nandOp(VM &, Word instruction);   // 2   R 1
	void norOp(VM &, Word instruction);    // 2   R 2
	void notOp(VM &, Word instruction);    // 2   R 3
	void orOp(VM &, Word instruction);     // 2   R 4
	void xnorOp(VM &, Word instruction);   // 2   R 5
	void xorOp(VM &, Word instruction);    // 2   R 6
	void landOp(VM &, Word instruction);   // 2   R 8
	void lnandOp(VM &, Word instruction);  // 2   R 9
	void lnorOp(VM &, Word instruction);   // 2   R 10
	void lnotOp(VM &, Word instruction);   // 2   R 11
	void lorOp(VM &, Word instruction);    // 2   R 12
	void lxnorOp(VM &, Word instruction);  // 2   R 13
	void lxorOp(VM &, Word instruction);   // 2   R 14

	void addiOp(VM &, Word instruction);   // 3   I
	void subiOp(VM &, Word instruction);   // 4   I
	void multiOp(VM &, Word instruction);  // 5   I
	// 22
	// 23
	void multuiOp(VM &, Word instruction); // 24  I
	void slliOp(VM &, Word instruction);   // 34  I
	void srliOp(VM &, Word instruction);   // 35  I
	void sraiOp(VM &, Word instruction);   // 36  I
	void modiOp(VM &, Word instruction);   // 30  I
	void diviOp(VM &, Word instruction);   // 52  I
	void divuiOp(VM &, Word instruction);  // 53  I
	void diviiOp(VM &, Word instruction);  // 54  I
	void divuiiOp(VM &, Word instruction); // 55  I

	void andiOp(VM &, Word instruction);   // 6   I
	void nandiOp(VM &, Word instruction);  // 7   I
	void noriOp(VM &, Word instruction);   // 8   I
	void oriOp(VM &, Word instruction);    // 9   I
	void xnoriOp(VM &, Word instruction);  // 10  I
	void xoriOp(VM &, Word instruction);   // 11  I

	void luiOp(VM &, Word instruction);    // 13  I
	void slOp(VM &, Word instruction);     // 14  R 0
	void sleOp(VM &, Word instruction);    // 14  R 1
	void seqOp(VM &, Word instruction);    // 14  R 2
	void sluOp(VM &, Word instruction);    // 14  R 3
	void sleuOp(VM &, Word instruction);   // 14  R 4

	void sliOp(VM &, Word instruction);    // 25  I
	void sleiOp(VM &, Word instruction);   // 26  I
	void seqiOp(VM &, Word instruction);   // 27  I
	void sluiOp(VM &, Word instruction);   // 28  I
	void sleuiOp(VM &, Word instruction);  // 29  I
	void sgiOp(VM &, Word instruction);    // 41  I
	void sgeiOp(VM &, Word instruction);   // 42  I

	void jOp(VM &, Word instruction);      // 15  J
	void jcOp(VM &, Word instruction);     // 16  J

	void jrOp(VM &, Word instruction);     // 17  R 0
	void jrcOp(VM &, Word instruction);    // 17  R 1
	void jrlOp(VM &, Word instruction);    // 17  R 2
	void jrlcOp(VM &, Word instruction);   // 17  R 3

	void cOp(VM &, Word instruction);      // 18  R 0
	void lOp(VM &, Word instruction);      // 18  R 1
	void sOp(VM &, Word instruction);      // 18  R 2
	void cbOp(VM &, Word instruction);     // 18  R 3
	void lbOp(VM &, Word instruction);     // 18  R 4
	void sbOp(VM &, Word instruction);     // 18  R 5
	void spushOp(VM &, Word instruction);  // 18  R 6
	void spopOp(VM &, Word instruction);   // 18  R 7
	void chOp(VM &, Word instruction);     // 18  R 8
	void lhOp(VM &, Word instruction);     // 18  R 9
	void shOp(VM &, Word instruction);     // 18  R 10

	void liOp(VM &, Word instruction);     // 19  I
	void siOp(VM &, Word instruction);     // 20  I
	void setOp(VM &, Word instruction);    // 21  I
	void lbiOp(VM &, Word instruction);    // 37  I
	void sbiOp(VM &, Word instruction);    // 38  I
	void lniOp(VM &, Word instruction);    // 39  I
	void lbniOp(VM &, Word instruction);   // 40  I

	void intOp(VM &, Word instruction);    // 32  I
	void ritOp(VM &, Word instruction);    // 33  I
	void timeOp(VM &, Word instruction);   // 48  R
	void timeiOp(VM &, Word instruction);  // 49  I
	void ringOp(VM &, Word instruction);   // 50  R
	void ringiOp(VM &, Word instruction);  // 51  I

	void prOp(VM &, Word instruction);     // 31 ?R 1
	void haltOp(VM &, Word instruction);   // 31 ?R 2
	void evalOp(VM &, Word instruction);   // 31 ?R 3
	void prcOp(VM &, Word instruction);    // 31 ?R 4
	void prdOp(VM &, Word instruction);    // 31 ?R 5
	void prxOp(VM &, Word instruction);    // 31 ?R 6
	void sleepOp(VM &, Word instruction);  // 31 ?R 7
}

#endif
