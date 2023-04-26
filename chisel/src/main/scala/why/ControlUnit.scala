package why

import chisel3._
import chisel3.util._

class ControlUnit(memorySize: Int) extends Module {
	val io = IO(new Bundle {
		val memory          = MemoryIO()
		val programCounter  = Input(UInt(64.W))
		val instruction     = Input(UInt(64.W))
		val instructionAddr = Input(UInt(64.W))
		val aluOut          = Input(SInt(64.W))
		val decoded         = Input(IDBundle())
		val state           = Output(ControlState.chiselType)
		val mar             = Output(UInt(64.W))
		val pcInput         = Output(UInt(64.W)) // Written to the program counter whenever pcWrite is true
		val pcWrite         = Output(Bool())
		val irInput         = Output(UInt(64.W))
		val irWrite         = Output(Bool())
		val alu             = Output(ALUInput())
		val registers       = Flipped(RegisterBankIO(7, 64))

		// These values are for testing purposes only.
		val overrides       = Overrides()
	})

}
