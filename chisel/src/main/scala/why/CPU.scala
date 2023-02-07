package why

import chisel3._
import chisel3.util._

class CPU extends Module {
	val io = IO(new Bundle {
		val memory = Output(MemoryIO())
	})

	val instructionRegister = RegInit(0.U(64.W))
	val typeRegister = RegInit(0.U(32.W))

	val programCounter = RegInit(0.U(64.W))

	val alu = Module(new ALU())

}
