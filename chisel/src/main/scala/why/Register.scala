package why

import chisel3._
import chisel3.util._

class Register(initialValue: UInt, width: Int = 64) extends Module {
	val io = IO(new Bundle {
		val input       = Input(UInt(width.W))
		val writeEnable = Input(Bool())
		val output      = Output(UInt(width.W))
	})

	val register = RegInit(initialValue)

	when (io.writeEnable) {
		register := io.input
	}

	io.output := register
}
