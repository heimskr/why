package why

import chisel3._

class InstructionDecoder extends Module {
	val io = IO(new Bundle {
		val instruction = Input(UInt(64.W))
		val types       = Input(UInt(32.W))
		val out         = Output(IDBundle())
	})

	// Common
	io.out.opcode := io.instruction(63, 52)

	// R-types
	io.out.rRs    := io.instruction(44, 38)
	io.out.rRt    := io.instruction(51, 45)
	io.out.rRd    := io.instruction(37, 31)
	io.out.rCond  := io.instruction(17, 14)
	io.out.rFunc  := io.instruction(11,  0)

	// I-types
	io.out.iCond  := io.instruction(51, 48)
	io.out.iRs    := io.instruction(45, 39)
	io.out.iRd    := io.instruction(38, 32)

	// J-types
	io.out.jRs    := io.instruction(51, 45)
	io.out.jCond  := io.instruction(37, 34)
	io.out.jLink  := io.instruction(44)

	// I- and J-types
	io.out.imm    := io.instruction(31, 0)

	// TODO: change instruction format (wasmc, wvm, ISA.md) to move J-types' rs type one byte to the left
	io.out.rdType := io.types(7, 0)
	io.out.rsType := io.types(15, 8)
	io.out.rtType := io.types(23, 16)
	io.out.immType := io.types(23, 16)
}
