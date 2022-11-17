#include <bitset>
#include <cassert>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <unistd.h>

#include "lib/ansi.h"
#include "mult.h"
#include "Operations.h"
#include "Util.h"
#include "VM.h"

// If set, the server will print a notice each time paging is enabled or disabled or when the page table address is
// changed.
#define DEBUG_PAGING

namespace WVM::Operations {
	std::set<int> RSet {
		OP_RMATH, OP_AND, OP_RLOGIC, OP_RCOMP, OP_RJUMP, OP_RMEM, OP_TIME, OP_RING, OP_SEL, OP_PAGE, OP_QUERY, OP_REXT,
		OP_INTERRUPTS, OP_TRANS,
	};

	std::set<int> ISet {
		OP_ADDI, OP_SUBI, OP_MULTI, OP_SLLI, OP_SRLI, OP_SRAI, OP_MODI, OP_DIVI, OP_DIVII, OP_ANDI, OP_NANDI, OP_NORI,
		OP_ORI, OP_XNORI, OP_XORI, OP_LUI, OP_SLI, OP_SLEI, OP_CMPI, OP_SEQI, OP_SGI, OP_SGEI, OP_LI, OP_SI, OP_SET,
		OP_LNI, OP_INT, OP_RIT, OP_TIMEI, OP_RINGI, OP_SLLII, OP_SRLII, OP_SRAII,
	};

	std::set<int> JSet {OP_J, OP_JC};

	void execute(VM &vm, Wasmc::TypedInstruction typed) {
		auto [instruction, types] = typed;
		auto lock = vm.lockVM();
		int opcode = (instruction >> 52) & 0xfff;
		if (opcode == OP_NOP) {
			vm.increment();
		} else if (RSet.count(opcode) == 1) {
			int rs, rt, rd;
			Conditions conditions;
			int flags, funct;
			int st, tt, dt;
			decodeRType(typed, rs, rt, rd, conditions, flags, funct, st, tt, dt);
			RArgs args(vm, vm.registers[rs], vm.registers[rt], vm.registers[rd], conditions, flags, funct, st, tt, dt);
			executeRType(opcode, args);
		} else if (ISet.count(opcode) == 1) {
			int rs, rd;
			Conditions conditions;
			int flags;
			HWord immediate;
			int st, dt, it;
			decodeIType(typed, rs, rd, conditions, flags, immediate, st, dt, it);
			IArgs args(vm, vm.registers[rs], vm.registers[rd], conditions, flags, immediate, st, dt, it);
			executeIType(opcode, args);
		} else if (JSet.count(opcode) == 1) {
			int rs;
			bool link;
			Conditions conditions;
			int flags;
			HWord address;
			int st;
			decodeJType(typed, rs, link, conditions, flags, address, st);
			JArgs args(vm, vm.registers[rs], link, conditions, flags, address, st);
			executeJType(opcode, args);
		} else
			throw std::runtime_error("Unknown opcode at " + std::to_string(vm.programCounter) + ": " +
				std::to_string(opcode));
	}

	void executeRType(int opcode, RArgs &args) {
		switch (opcode) {
			case OP_RMATH:
				switch (args.function) {
					case FN_ADD:       addOp(args); return;
					case FN_SUB:       subOp(args); return;
					case FN_MULT:     multOp(args); return;
					case FN_SLL:       sllOp(args); return;
					case FN_SRL:       srlOp(args); return;
					case FN_SRA:       sraOp(args); return;
					case FN_MOD:       modOp(args); return;
					case FN_DIV:       divOp(args); return;
					case FN_SEXT:     sextOp(args); return;
				}
				break;
			case OP_RLOGIC:
				switch (args.function) {
					case FN_AND:     andOp(args); return;
					case FN_NAND:   nandOp(args); return;
					case FN_NOR:     norOp(args); return;
					case FN_NOT:     notOp(args); return;
					case FN_OR:       orOp(args); return;
					case FN_XNOR:   xnorOp(args); return;
					case FN_XOR:     xorOp(args); return;
					case FN_LAND:   landOp(args); return;
					case FN_LNAND: lnandOp(args); return;
					case FN_LNOR:   lnorOp(args); return;
					case FN_LNOT:   lnotOp(args); return;
					case FN_LOR:     lorOp(args); return;
					case FN_LXNOR: lxnorOp(args); return;
					case FN_LXOR:   lxorOp(args); return;
				}
				break;
			case OP_RCOMP:
				switch (args.function) {
					case FN_SL:     slOp(args); return;
					case FN_SLE:   sleOp(args); return;
					case FN_SEQ:   seqOp(args); return;
				}
				break;
			case OP_RJUMP:
				switch (args.function) {
					case FN_JR:     jrOp(args); return;
					case FN_JRC:   jrcOp(args); return;
					case FN_JRL:   jrlOp(args); return;
					case FN_JRLC: jrlcOp(args); return;
				}
				break;
			case OP_RMEM:
				switch (args.function) {
					case FN_C:         cOp(args); return;
					case FN_L:         lOp(args); return;
					case FN_S:         sOp(args); return;
					case FN_SPUSH: spushOp(args); return;
					case FN_SPOP:   spopOp(args); return;
				}
				break;
			case OP_REXT:
				switch (args.function) {
					case FN_PR:       prOp(args); return;
					case FN_HALT:   haltOp(args); return;
					case FN_PRC:     prcOp(args); return;
					case FN_PRD:     prdOp(args); return;
					case FN_PRX:     prxOp(args); return;
					case FN_SLEEP: sleepOp(args); return;
					case FN_PRB:     prbOp(args); return;
					case FN_REST:   restOp(args); return;
					case FN_IO:       ioOp(args); return;
				}
				break;
			case OP_TIME:
				switch (args.function) {
					case FN_TIME:     timeOp(args); return;
					case FN_SVTIME: svtimeOp(args); return;
				}
				break;
			case OP_RING:
				switch (args.function) {
					case FN_RING:     ringOp(args); return;
					case FN_SVRING: svringOp(args); return;
				}
				break;
			case OP_SEL:   selOp(args); return;
			case OP_PAGE:
				switch (args.function) {
					case FN_PGOFF: pgoffOp(args); return;
					case FN_PGON:   pgonOp(args); return;
					case FN_SETPT: setptOp(args); return;
					case FN_SVPG:   svpgOp(args); return;
					case FN_PPUSH: ppushOp(args); return;
					case FN_PPOP:   ppopOp(args); return;
				}
				break;
			case OP_QUERY:
				switch (args.function) {
					case FN_QM: qmOp(args); return;
				}
				break;
			case OP_INTERRUPTS:
				switch (args.function) {
					case FN_DI: diOp(args); return;
					case FN_EI: eiOp(args); return;
				}
				break;
			case OP_TRANS: transOp(args); return;
		}

		throw std::runtime_error("Unknown R-type: " + std::to_string(opcode) + ":" + std::to_string(args.function));
	}

	void executeIType(int opcode, IArgs &args) {
		switch (opcode) {
			case OP_ADDI:     addiOp(args); break;
			case OP_SUBI:     subiOp(args); break;
			case OP_MULTI:   multiOp(args); break;
			case OP_SLLI:     slliOp(args); break;
			case OP_SRLI:     srliOp(args); break;
			case OP_SRAI:     sraiOp(args); break;
			case OP_MODI:     modiOp(args); break;
			case OP_DIVI:     diviOp(args); break;
			case OP_DIVII:   diviiOp(args); break;
			case OP_ANDI:     andiOp(args); break;
			case OP_NANDI:   nandiOp(args); break;
			case OP_NORI:     noriOp(args); break;
			case OP_ORI:       oriOp(args); break;
			case OP_XNORI:   xnoriOp(args); break;
			case OP_XORI:     xoriOp(args); break;
			case OP_LUI:       luiOp(args); break;
			case OP_SLI:       sliOp(args); break;
			case OP_SLEI:     sleiOp(args); break;
			case OP_SEQI:     seqiOp(args); break;
			case OP_SGI:       sgiOp(args); break;
			case OP_SGEI:     sgeiOp(args); break;
			case OP_LI:         liOp(args); break;
			case OP_SI:         siOp(args); break;
			case OP_SET:       setOp(args); break;
			case OP_LNI:       lniOp(args); break;
			case OP_INT:       intOp(args); break;
			case OP_RIT:       ritOp(args); break;
			case OP_TIMEI:   timeiOp(args); break;
			case OP_RINGI:   ringiOp(args); break;
			case OP_CMPI:     cmpiOp(args); break;
			case OP_SLLII:   slliiOp(args); break;
			case OP_SRLII:   srliiOp(args); break;
			case OP_SRAII:   sraiiOp(args); break;
			default:
				throw std::runtime_error("Unknown I-type: " + std::to_string(opcode));
		}
	}

	void executeJType(int opcode, JArgs &args) {
		switch (opcode) {
			case OP_J:   jOp(args); break;
			case OP_JC: jcOp(args); break;
			default:
				throw std::runtime_error("Unknown J-type: " + std::to_string(opcode));
		}
	}

	void decodeRType(Wasmc::TypedInstruction typed, int &rs, int &rt, int &rd, Conditions &conds, int &flags,
	                 int &funct, int &st, int &tt, int &dt) {
		auto [instruction, types] = typed;
		rd = (instruction >> 31) & 0b1111111;
		rs = (instruction >> 38) & 0b1111111;
		rt = (instruction >> 45) & 0b1111111;
		conds = static_cast<Conditions>((instruction >> 14) & 0b1111);
		flags = (instruction >> 12) & 0b11;
		funct = instruction & 0xfff;
		st = (types >>  8) & 0xff;
		tt = (types >> 16) & 0xff;
		dt = types & 0xff;
	}

	void decodeIType(Wasmc::TypedInstruction typed, int &rs, int &rd,  Conditions &conds, int &flags, HWord &immediate,
	                 int &st, int &dt, int &it) {
		auto [instruction, types] = typed;
		rs = (instruction >> 39) & 0b1111111;
		rd = (instruction >> 32) & 0b1111111;
		conds = static_cast<Conditions>((instruction >> 48) & 0b1111);
		flags = (instruction >> 46) & 0b11;
		immediate = instruction & 0xffffffff;
		st = (types >>  8) & 0xff;
		it = (types >> 16) & 0xff; 
		dt = types & 0xff;
	}

	void decodeJType(Wasmc::TypedInstruction typed, int &rs, bool &link, Conditions &conds, int &flags, HWord &address,
	                 int &st) {
		auto [instruction, types] = typed;
		rs = (instruction >> 45) & 0b1111111;
		link = (instruction >> 44) & 1;
		conds = static_cast<Conditions>((instruction >> 34) & 0b1111);
		flags = (instruction >> 32) & 0b11;
		address = instruction & 0xffffffff;
		st = types & 0xff;

		DBG("======================");
		DBG("opcode[" << ((typed.instruction >> 52) & 0xfff) << "]");
		DBG("rs[" << Why::coloredRegister(rs) << "]");
		DBG("link[" << std::boolalpha << link << "]");
		DBG("conds[" << static_cast<int>(conds) << "]");
		DBG("flags[" << flags << "]");
		DBG("address[" << Util::toHex(address, 8) << "]");
		DBG("st[" << Util::toHex(uint8_t(st), 1) << "]");
	}

	static void setReg(VM &vm, Register &rd, Register new_value, bool update_flags = true) {
		if (vm.registerID(rd) == Why::zeroOffset)
			std::cerr << "Set register $0 at " << vm.programCounter << "!\n";
		vm.bufferChange<RegisterChange>(vm, vm.registerID(rd), new_value.value, new_value.type);
		if (update_flags)
			vm.updateFlags(rd = new_value);
		else
			rd = new_value;
		vm.onRegisterChange(vm.registerID(rd));
	}

	static bool typeCheck(const OperandType &one, const OperandType &two, int level_difference = 0) {
		if (level_difference == 0)
			return one == two;
		return one.isSigned == two.isSigned && one.primitive == two.primitive &&
			one.pointerLevel - level_difference == two.pointerLevel;
	}

	static bool typeCheck(Register &one, Register &two, int level_difference = 0) {
		return typeCheck(one.type, two.type, level_difference);
	}

	static bool typeCheck(Register &one, int two) {
		assert(0 <= two && two <= 0xff);
		return one.type == OperandType(static_cast<uint8_t>(two));
	}

	/** Returns whether the current type of rs matches the instruction's encoded rs and rd types. */
	static bool typeCheckOne(RArgs &args, bool check_number = false) {
		const OperandType dt(args.rdType), st(args.rsType);
		if (args.rs.type == st && args.rs.type == dt)
			return check_number? args.rs.type.isNumber() : true;
		return false;
	}

	/** Returns whether the current type of rs matches the instruction's encoded rs and rd types and whether the current
	 *  type of rt matches the instruction's encoded rt type. */
	static bool typeCheckTwo(RArgs &args, bool check_number = false) {
		const OperandType dt(args.rdType), st(args.rsType), tt(args.rtType);
		if (args.rs.type == st && args.rs.type == dt && args.rt.type == tt)
			return check_number? args.rs.type.isNumber() : true;
		return false;
	}

	/** Returns whether the current type of rs matches the instruction's encoded rs and rd types. */
	static bool typeCheck(IArgs &args, bool check_number = false) {
		const OperandType dt(args.rdType), st(args.rsType);
		if (args.rs.type == st && args.rs.type == dt)
			return check_number? args.rs.type.isNumber() : true;
		return false;
	}

	void addOp(RArgs &args) {
		if (!typeCheck(args.rs, args.rt)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value + args.rt.value, args.rdType));
		args.vm.increment();
	}

	void subOp(RArgs &args) {
		if (!typeCheck(args.rs, args.rt)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value - args.rt.value, args.rdType));
		args.vm.increment();
	}

	void multOp(RArgs &args) {
		// TODO: handle unsigned
		VM &vm = args.vm;

		if (!typeCheckTwo(args, true)) {
			vm.intBadtyp();
			return;
		}

		Word old_hi = vm.hi().value;
		Word old_lo = vm.lo().value;
		CAT_MUL128(vm.hi().value, vm.lo().value, args.rs.value, args.rt.value);
		vm.onRegisterChange(Why::loOffset);
		vm.onRegisterChange(Why::hiOffset);
		const OperandType new_type(args.rs.type.isSigned, Primitive::Long);
		vm.bufferChange<RegisterChange>(Why::hiOffset, old_hi, vm.hi().value, vm.hi().type, new_type);
		vm.bufferChange<RegisterChange>(Why::loOffset, old_lo, vm.lo().value, vm.lo().type, new_type);
		vm.increment();
	}

	void sllOp(RArgs &args) {
		if (!typeCheckTwo(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value << args.rt.value, args.rdType));
		args.vm.increment();
	}

	void srlOp(RArgs &args) {
		if (!typeCheckTwo(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(static_cast<UWord>(args.rs.value) >> static_cast<UWord>(args.rt.value),
			args.rdType));
		args.vm.increment();
	}

	void sraOp(RArgs &args) {
		if (!typeCheckTwo(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value >> args.rt.value, args.rdType));
		args.vm.increment();
	}

	void modOp(RArgs &args) {
		if (!typeCheckTwo(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		if (args.rt.value == 0) {
			args.vm.mathError();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value % args.rt.value, args.rdType));
		args.vm.increment();
	}

	void divOp(RArgs &args) {
		if (!typeCheckTwo(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		if (args.rt.value == 0) {
			args.vm.mathError();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value / args.rt.value, args.rdType));
		args.vm.increment();
	}

	void sextOp(RArgs &args) {
		// TODO!

		// if ((rs & 0x80000000) != 0)
		// 	setReg(args.vm, args.rd, 0xffffffff00000000ul | rs);
		// else
		// 	setReg(args.vm, args.rd, 0x00000000fffffffful & rs);
		args.vm.increment();
	}

	// void sext16Op(RArgs &args) {
	// 	if ((rs & 0x8000) != 0)
	// 		setReg(args.vm, args.rd, 0xffffffffffff0000ul | rs);
	// 	else
	// 		setReg(args.vm, args.rd, 0x000000000000fffful & rs);
	// 	args.vm.increment();
	// }

	// void sext8Op(RArgs &args) {
	// 	if ((rs & 0x80) != 0)
	// 		setReg(args.vm, args.rd, 0xffffffffffffff00ul | rs);
	// 	else
	// 		setReg(args.vm, args.rd, 0x00000000000000fful & rs);
	// 	args.vm.increment();
	// }

	void andOp(RArgs &args) {
		if (!typeCheckTwo(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value & args.rt.value, args.rdType));
		args.vm.increment();
	}

	void nandOp(RArgs &args) {
		if (!typeCheckTwo(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(~(args.rs.value & args.rt.value), args.rdType));
		args.vm.increment();
	}

	void norOp(RArgs &args) {
		if (!typeCheckTwo(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(~(args.rs.value | args.rt.value), args.rdType));
		args.vm.increment();
	}

	void notOp(RArgs &args) {
		if (!typeCheckOne(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(~args.rs.value, args.rdType));
		args.vm.increment();
	}

	void orOp(RArgs &args) {
		if (!typeCheckTwo(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value | args.rt.value, args.rdType));
		args.vm.increment();
	}

	void xnorOp(RArgs &args) {
		if (!typeCheckTwo(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(~(args.rs.value ^ args.rt.value), args.rdType));
		args.vm.increment();
	}

	void xorOp(RArgs &args) {
		if (!typeCheckTwo(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value ^ args.rt.value, args.rdType));
		args.vm.increment();
	}

	void landOp(RArgs &args) {
		if (!typeCheckTwo(args, false)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value && args.rt.value, args.rdType));
		args.vm.increment();
	}

	void lnandOp(RArgs &args) {
		if (!typeCheckTwo(args, false)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(!(args.rs.value && args.rt.value), args.rdType));
		args.vm.increment();
	}

	void lnorOp(RArgs &args) {
		if (!typeCheckOne(args, false)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(!(args.rs.value || args.rt.value), args.rdType));
		args.vm.increment();
	}

	void lnotOp(RArgs &args) {
		if (!typeCheckOne(args, false)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(!args.rs.value, args.rdType));
		args.vm.increment();
	}

	void lorOp(RArgs &args) {
		if (!typeCheckTwo(args, false)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value || args.rt.value, args.rdType));
		args.vm.increment();
	}

	void lxnorOp(RArgs &args) {
		if (!typeCheckTwo(args, false)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(!(!!args.rs.value ^ !!args.rt.value), args.rdType));
		args.vm.increment();
	}

	void lxorOp(RArgs &args) {
		if (!typeCheckTwo(args, false)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(!!(!!args.rs.value ^ !!args.rt.value), args.rdType));
		args.vm.increment();
	}

	void addiOp(IArgs &args) {
		if (!typeCheck(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value + args.immediate, args.rdType));
		args.vm.increment();
	}

	void subiOp(IArgs &args) {
		if (!typeCheck(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value - args.immediate, args.rdType));
		args.vm.increment();
	}

	void multiOp(IArgs &args) {
		// TODO: handle unsigned
		VM &vm = args.vm;

		if (!typeCheck(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		Word old_hi = vm.hi().value;
		Word old_lo = vm.lo().value;
		CAT_MUL128(vm.hi().value, vm.lo().value, args.rs.value, args.immediate);
		vm.onRegisterChange(Why::hiOffset);
		vm.onRegisterChange(Why::loOffset);
		const OperandType new_type(args.rs.type.isSigned, Primitive::Long);
		vm.bufferChange<RegisterChange>(Why::hiOffset, old_hi, vm.hi().value, vm.hi().type, new_type);
		vm.bufferChange<RegisterChange>(Why::loOffset, old_lo, vm.lo().value, vm.lo().type, new_type);
		args.vm.increment();
	}

	void slliOp(IArgs &args) {
		if (!typeCheck(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value << args.immediate, args.rdType));
		args.vm.increment();
	}

	void srliOp(IArgs &args) {
		if (!typeCheck(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(UWord(args.rs.value) >> UWord(args.immediate), args.rdType));
		args.vm.increment();
	}

	void sraiOp(IArgs &args) {
		if (!typeCheck(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(Word(args.rs.value) >> Word(args.immediate), args.rdType));
		args.vm.increment();
	}

	void modiOp(IArgs &args) {
		if (!typeCheck(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		if (args.immediate == 0) {
			args.vm.mathError();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value % args.immediate, args.rdType));
		args.vm.increment();
	}

	void diviOp(IArgs &args) {
		if (!typeCheck(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		if (args.immediate == 0) {
			args.vm.mathError();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value / args.immediate, args.rdType));
		args.vm.increment();
	}

	void slliiOp(IArgs &args) {
		if (!typeCheck(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.immediate << args.rs.value, args.rdType));
		args.vm.increment();
	}

	void srliiOp(IArgs &args) {
		if (!typeCheck(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(UWord(args.immediate) >> UWord(args.rs.value), args.rdType));
		args.vm.increment();
	}

	void sraiiOp(IArgs &args) {
		if (!typeCheck(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(Word(args.immediate) >> Word(args.rs.value), args.rdType));
		args.vm.increment();
	}

	void diviiOp(IArgs &args) {
		if (!typeCheck(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		if (args.rs.value == 0) {
			args.vm.mathError();
			return;
		}

		setReg(args.vm, args.rd, Register(args.immediate / args.rs.value, args.rdType));
		args.vm.increment();
	}

	void andiOp(IArgs &args) {
		if (!typeCheck(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value & args.immediate, args.rdType));
		args.vm.increment();
	}

	void nandiOp(IArgs &args) {
		if (!typeCheck(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(~(args.rs.value & args.immediate), args.rdType));
		args.vm.increment();
	}

	void noriOp(IArgs &args) {
		if (!typeCheck(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(~(args.rs.value | args.immediate), args.rdType));
		args.vm.increment();
	}

	void oriOp(IArgs &args) {
		if (!typeCheck(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value | args.immediate, args.rdType));
		args.vm.increment();
	}

	void xnoriOp(IArgs &args) {
		if (!typeCheck(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(~(args.rs.value ^ args.immediate), args.rdType));
		args.vm.increment();
	}

	void xoriOp(IArgs &args) {
		if (!typeCheck(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value ^ args.immediate, args.rdType));
		args.vm.increment();
	}

	void luiOp(IArgs &args) {
		if (!typeCheck(args, true)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register((args.rd.value & 0xffffffff) | (static_cast<UWord>(args.immediate) << 32),
			args.rdType));
		args.vm.increment();
	}

	void slOp(RArgs &args) {
		if (!typeCheckTwo(args, false)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value < args.rt.value, args.rdType), false);
		args.vm.increment();
	}

	void sleOp(RArgs &args) {
		if (!typeCheckTwo(args, false)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value <= args.rt.value, args.rdType), false);
		args.vm.increment();
	}

	void seqOp(RArgs &args) {
		if (!typeCheckTwo(args, false)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value == args.rt.value, args.rdType), false);
		args.vm.increment();
	}

	void cmpOp(RArgs &args) {
		if (!typeCheckTwo(args, false)) {
			args.vm.intBadtyp();
			return;
		}

		args.vm.updateFlags(args.rs.value - args.rt.value);
		args.vm.increment();
	}

	void sliOp(IArgs &args) {
		if (!typeCheck(args, false)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value < args.immediate, args.rdType), false);
		args.vm.increment();
	}

	void sleiOp(IArgs &args) {
		if (!typeCheck(args, false)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value <= args.immediate, args.rdType), false);
		args.vm.increment();
	}

	void seqiOp(IArgs &args) {
		if (!typeCheck(args, false)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value == args.immediate, args.rdType), false);
		args.vm.increment();
	}

	void sgiOp(IArgs &args) {
		if (!typeCheck(args, false)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value > args.immediate, args.rdType), false);
		args.vm.increment();
	}

	void sgeiOp(IArgs &args) {
		if (!typeCheck(args, false)) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.rs.value >= args.immediate, args.rdType), false);
		args.vm.increment();
	}

	void cmpiOp(IArgs &args) {
		if (!typeCheck(args, false)) {
			args.vm.intBadtyp();
			return;
		}

		args.vm.updateFlags(args.rs.value - args.immediate);
		args.vm.increment();
	}

	void jOp(JArgs &args) {
		if (args.vm.checkConditions(args.conditions))
			args.vm.jump(args.address, args.link);
		else
			args.vm.increment();
	}

	void jcOp(JArgs &args) {
		if (!typeCheck(args.rs, args.rsType)) {
			args.vm.intBadtyp();
			return;
		}

		if (args.rs.value != 0)
			args.vm.jump(args.address, args.link);
		else
			args.vm.increment();
	}

	void jrOp(RArgs &args) {
		if (args.rd.type != OperandType::VOID_PTR) {
			args.vm.intBadtyp();
			return;
		}

		if (args.vm.checkConditions(args.conditions)) {
			const auto reg_id = args.vm.registerID(args.rd);
			// Reenable interrupts if jumping to $e0.
			if (reg_id == Why::exceptionOffset && args.vm.checkRing(Ring::Zero)) {
				args.vm.hardwareInterruptsEnabled = true;
				args.vm.wakeRest();
			}
			args.vm.jump(args.rd.value, false, reg_id == Why::returnAddressOffset);
		} else
			args.vm.increment();
	}

	void jrcOp(RArgs &args) {
		if (!typeCheck(args.rs, args.rsType) || args.rd.type != OperandType::VOID_PTR) {
			args.vm.intBadtyp();
			return;
		}

		if (args.rs.value != 0) {
			const auto reg_id = args.vm.registerID(args.rd);
			if (reg_id == Why::exceptionOffset && args.vm.checkRing(Ring::Zero))
				args.vm.hardwareInterruptsEnabled = true;
			args.vm.jump(args.rd.value, false, reg_id == Why::returnAddressOffset);
		} else
			args.vm.increment();
	}

	void jrlOp(RArgs &args) {
		if (args.rd.type != OperandType::VOID_PTR) {
			args.vm.intBadtyp();
			return;
		}

		if (args.vm.checkConditions(args.conditions))
			args.vm.jump(args.rd.value, true);
		else
			args.vm.increment();
	}

	void jrlcOp(RArgs &args) {
		if (!typeCheck(args.rs, args.rsType) || args.rd.type != OperandType::VOID_PTR) {
			args.vm.intBadtyp();
			return;
		}

		if (args.rs.value != 0) {
			bool success;
			const Word translated = args.vm.translateAddress(args.rd.value, &success);
			if (!success)
				args.vm.intPfault();
			else
				args.vm.jump(translated, true);
		} else
			args.vm.increment();
	}

	static Size getSize(const OperandType &type) {
		switch (type.primitive) {
			case Primitive::Char:  return Size::Byte;
			case Primitive::Short: return Size::QWord;
			case Primitive::Int:   return Size::HWord;
			case Primitive::Long:  return Size::Word;
			default: throw std::invalid_argument("Can't get size of primitive " +
				std::to_string(static_cast<int>(type.primitive)));
		}
	}

	static Size getSize(int type) {
		switch (type & 0b111) {
			case 0b001: return Size::Byte;
			case 0b010: return Size::QWord;
			case 0b011: return Size::HWord;
			case 0b100: return Size::Word;
			default: throw std::invalid_argument("Can't get size of primitive " + std::to_string(type));
		}
	}

	void cOp(RArgs &args) {
		if (args.rs.type != args.rd.type || args.rs.type != args.rsType || args.rsType != args.rdType) {
			args.vm.intBadtyp();
			return;
		}

		bool success;
		const Word translated_source = args.vm.translateAddress(args.rs.value, &success);
		if (!success) {
			args.vm.intPfault();
			return;
		}

		const Size size = getSize(args.rs.type);
		const UWord value = args.vm.get(translated_source, size);
		const Word translated_destination = args.vm.translateAddress(args.rd.value, &success);
		if (!success) {
			args.vm.intPfault();
			return;
		}

		if (!args.vm.checkWritable()) {
			args.vm.intBwrite(translated_destination);
			return;
		}

		args.vm.bufferChange<MemoryChange>(args.vm, translated_destination, value, size);
		args.vm.setWord(translated_destination, value);
		args.vm.increment();
	}

	void lOp(RArgs &args) {
		if (!typeCheck(args.rsType, args.rdType, 1) || args.rs.type != args.rsType) {
			args.vm.intBadtyp();
			return;
		}

		bool success;
		const Word translated = args.vm.translateAddress(args.rs.value, &success);
		if (!success) {
			args.vm.intPfault();
			return;
		}

		const Size size = getSize(args.rdType);
		setReg(args.vm, args.rd, Register(args.vm.get(translated, size), args.rdType), false);
		args.vm.increment();
	}

	void sOp(RArgs &args) {
		if (!typeCheck(args.rsType, args.rdType, -1) || args.rs.type != args.rsType || args.rd.type != args.rdType) {
			args.vm.intBadtyp();
			return;
		}

		bool success;
		const Word translated = args.vm.translateAddress(args.rd.value, &success);
		if (!success) {
			args.vm.intPfault();
			return;
		}

		if (!args.vm.checkWritable()) {
			args.vm.intBwrite(translated);
			return;
		}

		const Size size = getSize(args.rsType);
		args.vm.bufferChange<MemoryChange>(args.vm, translated, args.rs.value, size);
		args.vm.set(translated, args.rs.value, size);
		args.vm.increment();
	}

	void spushOp(RArgs &args) {
		if (!typeCheck(args.rs, args.rsType)) {
			args.vm.intBadtyp();
			return;
		}

		VM &vm = args.vm;
		setReg(vm, vm.sp(), Register(vm.sp().value - 8, vm.sp().type), false);
		bool success;
		const Word translated = vm.translateAddress(vm.sp().value, &success);

		if (!success) {
			vm.intPfault();
			return;
		}
		
		if (!vm.checkWritable()) {
			vm.intBwrite(translated);
			return;
		}

		const Size size = getSize(args.rsType);
		vm.bufferChange<MemoryChange>(vm, translated, args.rs.value, size);
		vm.set(translated, args.rs.value, size);
		vm.increment();
	}

	void spopOp(RArgs &args) {
		bool success;
		const Word translated = args.vm.translateAddress(args.vm.sp().value, &success);
		if (!success) {
			args.vm.intPfault();
			return;
		}

		const Size size = getSize(args.rdType);
		setReg(args.vm, args.rd, Register(args.vm.get(translated, size), OperandType(args.rdType)), false);
		setReg(args.vm, args.vm.sp(), Register(args.vm.sp().value + static_cast<char>(size) / 8, args.vm.sp().type),
			false);
		args.vm.increment();
	}

	void msOp(RArgs &args) {
		// Requirement: all operands' types must match the corresponding encoded types.
		if (!typeCheck(args.rd, args.rdType) || !typeCheck(args.rs, args.rsType) || !typeCheck(args.rt, args.rtType)) {
			args.vm.intBadtyp();
			return;
		}

		// This is awfully slow.
		bool success;
		for (Word i = 0; i < args.rs.value; ++i) {
			const Word translated = args.vm.translateAddress(args.rd.value + i, &success);
			if (!success) {
				args.vm.intPfault();
				return;
			}
			
			if (!args.vm.checkWritable()) {
				args.vm.intBwrite(translated);
				return;
			}

			args.vm.bufferChange<MemoryChange>(args.vm, translated, args.rt.value & 0xff, Size::Byte);
			args.vm.setByte(translated, args.rt.value & 0xff);
		}

		args.vm.increment();
	}

	void transOp(RArgs &args) {
		VM &vm = args.vm;

		// Requirements:
		// - Current rs type must match encoded rs type
		// - rs type must be a pointer
		if (args.rs.type.pointerLevel < 1 || !typeCheck(args.rs, args.rsType)) {
			vm.intBadtyp();
			return;
		}

		if (vm.pagingOn) {
			bool success;
			const Word translated = vm.translateAddress(args.rs.value, &success);
			if (!success) {
				vm.intPfault();
				return;
			}

			setReg(vm, args.rd, Register(translated, OperandType::VOID_PTR), false);
			vm.increment();
			return;
		}

		setReg(vm, args.rd, args.rs, false);
		vm.increment();
	}

	void liOp(IArgs &args) {
		VM &vm = args.vm;

		// Requirements:
		// - Encoded immediate type must be a pointer
		// - Encoded rd type must be equivalent to encoded immediate type minus one pointer level
		const OperandType imm_type(args.immType);
		const OperandType rd_type(args.rdType);
		if (imm_type.pointerLevel < 1 || !typeCheck(imm_type, rd_type, 1)) {
			vm.intBadtyp();
			return;
		}
		
		bool success;
		const Word translated = vm.translateAddress(args.immediate, &success);
		if (!success) {
			vm.intPfault();
			return;
		}

		setReg(vm, args.rd, Register(vm.get(translated, getSize(rd_type)), rd_type), false);
		vm.increment();
	}

	void siOp(IArgs &args) {
		VM &vm = args.vm;

		// Requirements:
		// - Encoded immediate type must be a pointer
		// - Encoded rs type must be equivalent to encoded immediate type minus one pointer level
		const OperandType imm_type(args.immType);
		const OperandType rs_type(args.rsType);
		if (imm_type.pointerLevel < 1 || !typeCheck(imm_type, rs_type, 1)) {
			vm.intBadtyp();
			return;
		}

		bool success;
		const Word translated = vm.translateAddress(args.immediate, &success);
		if (!success) {
			vm.intPfault();
			return;
		}
		
		if (!vm.checkWritable()) {
			vm.intBwrite(translated);
			return;
		}

		const Size size = getSize(rs_type);
		vm.bufferChange<MemoryChange>(vm, translated, args.rs.value, size);
		vm.set(translated, args.rs.value, size);
		vm.increment();
	}

	void setOp(IArgs &args) {
		setReg(args.vm, args.rd, Register(args.immediate, args.rdType), false);
		args.vm.increment();
	}

	void lniOp(IArgs &args) {
		VM &vm = args.vm;

		// Requirements:
		// - Encoded immediate type and encoded rd type must be pointers
		// - Encoded immediate type must match encoded rd type
		const OperandType imm_type(args.immType);
		const OperandType rd_type(args.rdType);
		if (!typeCheck(imm_type, rd_type) || imm_type.pointerLevel < 1) {
			vm.intBadtyp();
			return;
		}

		bool success;
		const Word translated = vm.translateAddress(args.immediate, &success);

		if (!success) {
			vm.intPfault();
			return;
		}

		if (!vm.checkWritable()) {
			vm.intBwrite(translated);
			return;
		}

		const Size size = getSize(rd_type);
		const Word value = vm.get(translated, size);
		vm.bufferChange<MemoryChange>(vm, args.rd.value, value, size);
		vm.set(args.rd.value, value, size);
		vm.increment();
	}

	void intOp(IArgs &args) {
		// Requirements:
		// - Encoded immediate type must be a number and not a pointer
		const OperandType imm_type(args.immType);
		if (!imm_type.isNumber()) {
			args.vm.intBadtyp();
			return;
		}

		args.vm.interrupt(args.immediate, false);
	}

	void ritOp(IArgs &args) {
		VM &vm = args.vm;
		if (!vm.checkRing(Ring::Zero)) {
			vm.intProtec();
			return;
		}

		// Requirements:
		// - Encoded immediate type must be uv**
		if (OperandType(args.immType) != OperandType(false, Primitive::Void, 2)) {
			vm.intBadtyp();
			return;
		}

		bool success;
		const Word translated = vm.translateAddress(args.immediate, &success);
		if (!success) {
			vm.intPfault();
			return;
		}

		vm.bufferChange<InterruptTableChange>(vm, translated);
		vm.interruptTableAddress = translated;
		vm.onInterruptTableChange();
		vm.increment();
	}

	void timeOp(RArgs &args) {
		if (!args.vm.checkRing(Ring::Zero)) {
			args.vm.intProtec();
			return;
		}

		// Requirements:
		// - Current rs type must match encoded rs type
		// - rs type must be a number
		const OperandType rs_type(args.rsType);
		if (!typeCheck(args.rs.type, rs_type) || !rs_type.isNumber()) {
			args.vm.intBadtyp();
			return;
		}

		args.vm.setTimer(static_cast<UWord>(args.rs.value));
		args.vm.increment();
	}

	void svtimeOp(RArgs &args) {
		if (!args.vm.checkRing(Ring::Zero)) {
			args.vm.intProtec();
			return;
		}

		// Requirements:
		// - Encoded rd type must be a number
		const OperandType rd_type(args.rdType);
		if (!rd_type.isNumber()) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.vm.timerTicks, rd_type), false);
		args.vm.increment();
	}

	void timeiOp(IArgs &args) {
		if (!args.vm.checkRing(Ring::Zero)) {
			args.vm.intProtec();
			return;
		}

		// Requirements:
		// - Encoded immediate type must be a number
		if (!OperandType(args.immType).isNumber()) {
			args.vm.intBadtyp();
			return;
		}

		args.vm.setTimer(static_cast<UWord>(args.immediate));
		args.vm.increment();
	}

	void ringOp(RArgs &args) {
		// Requirements:
		// - Current rs type must match encoded rs type
		// - rs type must be a number
		const OperandType rs_type(args.rsType);
		if (!typeCheck(args.rs.type, rs_type) || !rs_type.isNumber()) {
			args.vm.intBadtyp();
			return;
		}

		if (args.vm.changeRing(static_cast<Ring>(args.rs.value)))
			args.vm.increment();
	}

	void svringOp(RArgs &args) {
		// Requirements:
		// - Encoded rd type must be a number
		const OperandType rd_type(args.rdType);
		if (!rd_type.isNumber()) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(static_cast<Word>(args.vm.ring), rd_type), false);
		args.vm.increment();
	}

	void ringiOp(IArgs &args) {
		// Requirements:
		// - Encoded immediate type must be a number
		if (!OperandType(args.immType).isNumber()) {
			args.vm.intBadtyp();
			return;
		}

		if (args.vm.changeRing(static_cast<Ring>(args.immediate)))
			args.vm.increment();
	}

	void prOp(RArgs &args) {
		// Requirements:
		// - None. Go wild.
		VM &vm = args.vm;
		auto &rs = args.rs;
		std::stringstream ss;
		if (rs.value < 0)
			ss << Why::coloredRegister(vm.registerID(rs)) << ": " << rs.value << " / " << static_cast<UWord>(rs.value)
			   << " / " << std::hex << rs.value;
		else
			ss << Why::coloredRegister(vm.registerID(rs)) << ": " // << "0x" << std::hex << rs << " \e[2m/\e[22m " << std::dec
			   << rs.value << " / " << std::hex << rs.value;
		// vm.onPrint(ss.str() + "\n");
		// DBG(ss.str());
		std::cerr << ss.str() << '\n';
		args.vm.increment();
	}

	void haltOp(RArgs &args) {
		args.vm.recordChange<HaltChange>();
		args.vm.stop();
	}

	void prcOp(RArgs &args) {
		// Requirements:
		// - Current rs type must match encoded rs type
		// - rs type must be uc
		const OperandType rs_type(args.rsType);
		if (!typeCheck(args.rs.type, rs_type) || rs_type != OperandType(false, Primitive::Char, 0)) {
			args.vm.intBadtyp();
			return;
		}

		args.vm.onPrint(std::string(1, static_cast<char>(args.rs.value)));
		args.vm.increment();
	}

	void prdOp(RArgs &args) {
		// Requirements:
		// - Current rs type must match encoded rs type
		// - rs type must be a number
		const OperandType rs_type(args.rsType);
		if (!typeCheck(args.rs.type, rs_type) || !rs_type.isNumber()) {
			args.vm.intBadtyp();
			return;
		}

		args.vm.onPrint(std::to_string(args.rs.value));
		args.vm.increment();
	}

	void prxOp(RArgs &args) {
		// Requirements:
		// - Current rs type must match encoded rs type
		// - rs type must be a number
		const OperandType rs_type(args.rsType);
		if (!typeCheck(args.rs.type, rs_type) || !rs_type.isNumber()) {
			args.vm.intBadtyp();
			return;
		}

		std::stringstream ss;
		ss << std::hex << args.rs.value;
		args.vm.onPrint(ss.str());
		args.vm.increment();
	}

	void sleepOp(RArgs &args) {
		// Requirements:
		// - Current rs type must match encoded rs type
		// - rs type must be a number
		const OperandType rs_type(args.rsType);
		if (!typeCheck(args.rs.type, rs_type) || !rs_type.isNumber()) {
			args.vm.intBadtyp();
			return;
		}

		usleep(args.rs.value);
		args.vm.increment();
	}

	void prbOp(RArgs &args) {
		// Requirements:
		// - Current rs type must match encoded rs type
		// - rs type must be a number
		const OperandType rs_type(args.rsType);
		if (!typeCheck(args.rs.type, rs_type) || !rs_type.isNumber()) {
			args.vm.intBadtyp();
			return;
		}

		std::stringstream ss;
		ss << std::bitset<64>(args.rs.value);
		args.vm.onPrint(ss.str());
		args.vm.increment();
	}

	void restOp(RArgs &args) {
		args.vm.increment();
		args.vm.rest();
	}

	// TODO: typechecking
	void ioOp(RArgs &args) {
		VM &vm = args.vm;
		if (vm.checkRing(Ring::Two)) {
			const Register &a0 = vm.registers[Why::argumentOffset + 0];
			const Register &a1 = vm.registers[Why::argumentOffset + 1];
			const Register &a2 = vm.registers[Why::argumentOffset + 2];
			const Register &a3 = vm.registers[Why::argumentOffset + 3];
			Register &r0 = vm.registers[Why::returnValueOffset];
			Register &e0 = vm.registers[Why::exceptionOffset];

			const size_t device_id = static_cast<size_t>(a1.value);
			const bool valid_id = device_id < vm.drives.size();

			auto ulong = [](auto &&value) {
				return Register(value, OperandType::ULONG);
			};

			switch (a0.value) {
				case IO_DEVCOUNT:
					setReg(vm, e0, ulong(0), false);
					setReg(vm, r0, ulong(vm.drives.size()), false);
					break;

				case IO_SEEKABS:
					if (!valid_id)
						setReg(vm, e0, ulong(1), false);
					else if (lseek(vm.drives.at(device_id).fd, a2.value, SEEK_SET) == -1)
						setReg(vm, e0, ulong(2), false);
					else
						setReg(vm, e0, ulong(0), false);
					break;

				case IO_SEEKREL:
					if (!valid_id) {
						setReg(vm, e0, ulong(1), false);
					} else {
						const ssize_t result = lseek(vm.drives.at(device_id).fd, a2.value, SEEK_CUR);
						if (result == -1) {
							setReg(vm, e0, ulong(2), false);
						} else {
							setReg(vm, e0, ulong(0), false);
							setReg(vm, r0, ulong(result), false);
						}
					}

					break;

				case IO_READ: { // TODO: verify. This code is suspicious.
					if (!valid_id) {
						setReg(vm, e0, ulong(1), false);
					} else {
						size_t address = a2.value;
						size_t remaining = a3.value;
						size_t total_bytes_read = 0;
						const int fd = vm.drives.at(device_id).fd;
						const size_t memsize = vm.getMemorySize();
						setReg(vm, e0, ulong(0), false);

						while (0 < remaining) {
							const size_t mod = address % VM::PAGE_SIZE; // Especially this.

							bool translate_success;
							const size_t translated = size_t(vm.translateAddress(address, &translate_success));
							if (!translate_success) {
								setReg(vm, e0, ulong(2), false);
								vm.intPfault();
								return;
							}

							if (!vm.checkWritable()) {
								setReg(vm, e0, ulong(3), false);
								vm.intBwrite(translated);
								return;
							}

							if (memsize <= translated)
								break;
							
							const size_t diff = memsize - translated;
							if (diff < remaining)
								remaining = diff;

							const size_t to_read = std::min(mod? mod : VM::PAGE_SIZE, remaining); // And this.
							const ssize_t bytes_read = ::read(fd, &vm.memory[translated], to_read);

							if (bytes_read < 0)
								setReg(vm, e0, ulong(errno + 3), false);

							if (bytes_read <= 0)
								break;

							remaining -= size_t(bytes_read);
							address += size_t(bytes_read);
							total_bytes_read += size_t(bytes_read);
						}

						setReg(vm, r0, ulong(total_bytes_read), false);
					}

					break;
				}

				case IO_WRITE: { // TODO: verify this too.
					if (!valid_id) {
						setReg(vm, e0, ulong(1), false);
					} else {
						size_t address = a2.value;
						size_t remaining = a3.value;
						size_t total_bytes_written = 0;
						const int fd = vm.drives.at(device_id).fd;
						const size_t memsize = vm.getMemorySize();
						setReg(vm, e0, ulong(0), false);

						while (0 < remaining) {
							const size_t mod = address % VM::PAGE_SIZE;

							bool translate_success;
							const size_t translated = size_t(vm.translateAddress(address, &translate_success));
							if (!translate_success) {
								setReg(vm, e0, ulong(2), false);
								vm.intPfault();
								return;
							}

							if (memsize <= translated) {
								break;
							} else {
								const size_t diff = memsize - translated;
								if (diff < remaining)
									remaining = diff;
							}

							const size_t to_write = std::min(mod? mod : VM::PAGE_SIZE, remaining); // And this.
							const ssize_t bytes_written = ::write(fd, &vm.memory[translated], to_write);

							if (bytes_written < 0)
								setReg(vm, e0, ulong(errno + 2), false);
							if (bytes_written <= 0)
								break;
							remaining -= size_t(bytes_written);
							address += size_t(bytes_written);
							total_bytes_written += size_t(bytes_written);
						}

						setReg(vm, r0, ulong(total_bytes_written), false);
					}

					break;
				}

				case IO_GETSIZE: {
					if (!valid_id) {
						setReg(vm, e0, ulong(1), false);
						break;
					}

					const int fd = vm.drives.at(device_id).fd;
					const off_t old_cursor = lseek(fd, 0, SEEK_CUR);
					if (old_cursor == -1) {
						setReg(vm, e0, ulong(errno + 1), false);
						break;
					}

					const off_t end_cursor = lseek(fd, 0, SEEK_END);
					if (end_cursor == -1) {
						setReg(vm, e0, ulong(errno + 1), false);
						break;
					}

					setReg(vm, r0, ulong(static_cast<Word>(end_cursor)), false);

					const off_t result = lseek(fd, old_cursor, SEEK_SET);
					setReg(vm, e0, ulong(result == -1? errno + 1 : 0), false);
					break;
				}

				case IO_GETCURSOR: {
					if (!valid_id) {
						setReg(vm, e0, ulong(1), false);
						break;
					}

					const off_t cursor = lseek(vm.drives.at(device_id).fd, 0, SEEK_CUR);
					setReg(vm, e0, ulong(cursor == -1? errno + 1 : 0), false);
					if (cursor != -1)
						setReg(vm, r0, ulong(cursor), false);
					break;
				}

				case IO_GETNAME: {
					if (!valid_id) {
						setReg(vm, e0, ulong(1), false);
					} else {
						const std::string &name = vm.drives.at(a1.value).name;
						const char *c_str = name.c_str();
						size_t address = a2.value;
						size_t remaining = std::min(static_cast<size_t>(a2.value), name.size() + 1);
						size_t total_bytes_read = 0;
						const size_t memsize = vm.getMemorySize();
						setReg(vm, e0, ulong(0), false);

						while (0 < remaining) {
							const size_t mod = address % VM::PAGE_SIZE;
							bool translate_success;
							const size_t translated = size_t(vm.translateAddress(address, &translate_success));\

							if (!translate_success) {
								setReg(vm, e0, ulong(2), false);
								vm.intPfault();
								return;
							}

							if (!vm.checkWritable()) {
								setReg(vm, e0, ulong(3), false);
								vm.intBwrite(translated);
								return;
							}

							if (memsize <= translated)
								break;

							const size_t diff = memsize - translated;
							if (diff < remaining)
								remaining = diff;

							const size_t to_read = std::min(mod? mod : VM::PAGE_SIZE, remaining);
							std::memcpy(&vm.memory[translated], c_str + total_bytes_read, to_read);

							remaining -= to_read;
							address += to_read;
							total_bytes_read += to_read;
						}

						// Don't include the null terminator in the byte count
						setReg(vm, r0, ulong(0 < total_bytes_read? total_bytes_read - 1 : 0), false);
					}

					break;
				}

				default:
					setReg(vm, e0, ulong(666), false);
			}

			args.vm.increment();
		} else
			vm.intProtec();
	}

	void selOp(RArgs &args) {
		setReg(args.vm, args.rd, args.vm.checkConditions(args.conditions)? args.rs : args.rt, false);
		args.vm.increment();
	}

	void pgoffOp(RArgs &args) {
		VM &vm = args.vm;
		if (vm.checkRing(Ring::Zero)) {
			vm.bufferChange<PagingChange>(vm.pagingOn, false);
			vm.pagingOn = false;
			std::cerr << "Paging disabled (PC: " << vm.programCounter << ").\n";
			vm.onPagingChange(false);
			vm.increment();
		} else
			vm.intProtec();
	}

	void pgonOp(RArgs &args) {
		VM &vm = args.vm;
		if (vm.checkRing(Ring::Zero)) {
			vm.bufferChange<PagingChange>(vm.pagingOn, true);
			vm.pagingOn = true;
			std::cerr << "Paging enabled (PC: " << vm.programCounter << ").\n";
			vm.onPagingChange(true);
			args.vm.increment();
		} else
			vm.intProtec();
	}

	void setptOp(RArgs &args) {
		VM &vm = args.vm;

		// Requirements:
		// - Current rs type must match encoded rs type
		// - rs type must be uv*
		// - If rt is nonzero, it must be uv*
		const OperandType rs_type(args.rsType);
		if (!typeCheck(args.rs.type, rs_type) || rs_type != OperandType::VOID_PTR) {
			vm.intBadtyp();
			return;
		}

		if (args.rt.value != 0) {
			const OperandType rt_type(args.rtType);
			if (!typeCheck(args.rt.type, rt_type) || rt_type != OperandType::VOID_PTR) {
				vm.intBadtyp();
				return;
			}
		}

		if (!vm.checkRing(Ring::Zero)) {
			vm.intProtec();
			return;
		}

		vm.bufferChange<P0Change>(vm.p0, args.rs.value);
		vm.p0 = args.rs.value;
		std::cerr << "Page table address set to " << vm.p0 << " (PC: " << vm.programCounter << ").\n";
		vm.onP0Change(args.rs.value);
		if (args.rt.value != 0) {
			const auto reg_id = vm.registerID(args.rt);
			// Reenable interrupts if jumping to $e0.
			if (reg_id == Why::exceptionOffset && vm.checkRing(Ring::Zero)) {
				vm.hardwareInterruptsEnabled = true;
				vm.wakeRest();
			}
			vm.jump(args.rt.value, false);
			return;
		}

		args.vm.increment();
	}

	void svpgOp(RArgs &args) {
		// Requirements:
		// - Encoded rd type must be a number
		const OperandType rd_type(args.rdType);
		if (!rd_type.isNumber()) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.vm.pagingOn? 1 : 0, rd_type), false);
		args.vm.increment();
	}

	void ppushOp(RArgs &args) {
		if (!args.vm.checkRing(Ring::Zero)) {
			args.vm.intProtec();
			return;
		}

		args.vm.pagingStack.emplace_back(args.vm);
		args.vm.increment();
	}

	void ppopOp(RArgs &args) {
		VM &vm = args.vm;
		if (!vm.checkRing(Ring::Zero)) {
			vm.intProtec();
			return;
		}

		// Requirements:
		// - If rs is nonzero, it must be uv*
		if (args.rs.value != 0) {
			const OperandType rs_type(args.rsType);
			if (!typeCheck(args.rs.type, rs_type) || rs_type != OperandType::VOID_PTR) {
				vm.intBadtyp();
				return;
			}
		}

		if (!vm.pagingStack.empty()) {
			const auto &back = vm.pagingStack.back();
			vm.bufferChange<PagingChange>(vm.pagingOn, back.enabled);
			vm.bufferChange<P0Change>(vm.p0, back.p0);
			vm.pagingOn = back.enabled;
			vm.p0 = back.p0;
			vm.pagingStack.pop_back();
		}

		if (args.rs.value != 0) {
			const auto reg_id = vm.registerID(args.rs);

			// Reenable interrupts if jumping to $e0.
			if (reg_id == Why::exceptionOffset && vm.checkRing(Ring::Zero)) {
				vm.hardwareInterruptsEnabled = true;
				vm.wakeRest();
			}

			vm.jump(args.rs.value, false);
			return;
		}

		args.vm.increment();
	}

	void qmOp(RArgs &args) {
		// Requirements:
		// - Encoded rd type must be a number
		const OperandType rd_type(args.rdType);
		if (!rd_type.isNumber()) {
			args.vm.intBadtyp();
			return;
		}

		setReg(args.vm, args.rd, Register(args.vm.getMemorySize(), rd_type), false);
		args.vm.increment();
	}

	void diOp(RArgs &args) {
		if (args.vm.checkRing(Ring::Zero)) {
			args.vm.hardwareInterruptsEnabled = false;
			args.vm.increment();
		} else
			args.vm.intProtec();
	}

	void eiOp(RArgs &args) {
		if (args.vm.checkRing(Ring::Zero)) {
			args.vm.hardwareInterruptsEnabled = true;
			args.vm.increment();
		} else
			args.vm.intProtec();
	}
}
