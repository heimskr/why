package why

import chisel3._
import chisel3.util._

case class IDBundle() extends Bundle {
	val opcode = UInt(12.W)
	val imm    = UInt(32.W)

	val rRs   = UInt(7.W)
	val rRt   = UInt(7.W)
	val rRd   = UInt(7.W)
	val rCond = UInt(4.W)
	val rFunc = UInt(12.W)

	val iCond = UInt(4.W)
	val iRs   = UInt(7.W)
	val iRd   = UInt(7.W)

	val jRs   = UInt(7.W)
	val jCond = UInt(4.W)
	val jLink = Bool()
}

case class RegisterBankIO(idSize: Int, registerWidth: Int) extends Bundle {
	val inputData   = Input(UInt(registerWidth.W))
	val sID         = Input(UInt(idSize.W)) // ID of first register to read.
	val tID         = Input(UInt(idSize.W)) // ID of second register to read.
	val dID         = Input(UInt(idSize.W)) // ID of register to write.
	val writeEnable = Input(Bool())
	val sOutput     = Output(UInt(registerWidth.W)) // Read output of first register.
	val tOutput     = Output(UInt(registerWidth.W)) // Read output of second register.
}

case class MemoryIO() extends Bundle {
	val writeIn = Flipped(Valid(UInt(64.W)))
	val readOut = Decoupled(UInt(64.W))
	val address = Input(UInt(64.W))
	val size    = Input(UInt(2.W)) // 0 -> byte, 1 -> quarterword, 2 -> halfword, 3 -> word
	val busy    = Output(Bool())
}

case class ALUInput() extends Bundle {
	val a  = UInt(64.W)
	val b  = UInt(64.W)
	val op = UInt(8.W)
}

case class Overrides() extends Bundle {
	val registers   = RegisterBankIO(7, 64)
	val state       = Input(ControlState.chiselType)
	val stateEnable = Input(Bool())
	val mar         = Input(UInt(64.W))
	val inputWord   = Input(UInt(64.W))
	val writeWord   = Input(Bool())
}
