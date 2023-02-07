package why

import chisel3._

case class Type() extends Bundle {
	val length       = UInt(3.W)
	val signed       = Bool()
	val pointerLevel = UInt(4.W)

	def :=(linkTo: UInt): Unit = {
		length := linkTo(2, 0)
		signed := linkTo(3)
		pointerLevel := linkTo(7, 4)
	}
}

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

	val rsType  = Type()
	val rtType  = Type()
	val rdType  = Type()
	val immType = Type()
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
	val inputByte       = Input(UInt(8.W))
	val inputHWord      = Input(UInt(32.W))
	val inputWord       = Input(UInt(64.W))
	val readAddress     = Input(UInt(64.W))
	val writeAddress    = Input(UInt(64.W))
	val writeByte       = Input(Bool())
	val writeHWord      = Input(Bool())
	val writeWord       = Input(Bool())
	val outputSWord     = Output(UInt(96.W)) // SWord = superword, 12 bytes
	val outputWordLE    = Output(UInt(64.W))
	val outputWordBE    = Output(UInt(64.W))
	val outputHWord     = Output(UInt(32.W))
	val outputByte      = Output(UInt(8.W))
}

case class ALUInput() extends Bundle {
	val a  = SInt(64.W)
	val b  = SInt(64.W)
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
