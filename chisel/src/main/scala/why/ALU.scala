package why

import chisel3._
import chisel3.util._

class ALU extends Module {
	val io = IO(new Bundle {
		val in = Input(ALUInput())
		val out = Output(SInt(64.W))
	})

	val a_u = io.in.a.asUInt
	val b_u = io.in.b.asUInt

	val b_short = Wire(UInt(19.W))
	b_short := io.in.b.asUInt()

	io.out := "hdeadbeef".U.asSInt

	switch (io.in.op) {
		is (ALU.Zero)                       { io.out := 0.S }
		is (ALU.Add)                        { io.out := io.in.a + io.in.b }
		is (ALU.Subtract)                   { io.out := io.in.a - io.in.b }
		is (ALU.SignedMultiply)             { io.out := io.in.a * io.in.b }
		is (ALU.UnsignedMultiply)           { io.out := (a_u * b_u).asSInt }
		is (ALU.SignedDivide)               { io.out := io.in.a / io.in.b }
		is (ALU.UnsignedDivide)             { io.out := (a_u / b_u).asSInt }
		is (ALU.SignedModulo)               { io.out := io.in.a % io.in.b }
		is (ALU.UnsignedModulo)             { io.out := (a_u % b_u).asSInt }
		is (ALU.BitwiseAND)                 { io.out := io.in.a & io.in.b }
		is (ALU.BitwiseOR)                  { io.out := io.in.a | io.in.b }
		is (ALU.BitwiseXOR)                 { io.out := io.in.a ^ io.in.b }
		is (ALU.BitwiseNAND)                { io.out := ~(io.in.a & io.in.b) }
		is (ALU.BitwiseNOR)                 { io.out := ~(io.in.a | io.in.b) }
		is (ALU.BitwiseXNOR)                { io.out := ~(io.in.a ^ io.in.b) }
		is (ALU.LogicalAND)                 { io.out := (a_u.orR & b_u.orR).asSInt * -1.S }
		is (ALU.LogicalOR)                  { io.out := (a_u.orR | b_u.orR).asSInt * -1.S }
		is (ALU.LogicalXOR)                 { io.out := (a_u.orR ^ b_u.orR).asSInt * -1.S }
		is (ALU.LogicalNAND)                { io.out := ((a_u.orR & b_u.orR) === 0.U).asSInt * -1.S }
		is (ALU.LogicalNOR)                 { io.out := ((a_u.orR | b_u.orR) === 0.U).asSInt * -1.S }
		is (ALU.LogicalXNOR)                { io.out := ((a_u.orR ^ b_u.orR) === 0.U).asSInt * -1.S }
		is (ALU.ShiftLeftLogical)           { io.out := io.in.a << b_short }
		is (ALU.ShiftRightLogical)          { io.out := io.in.a >> b_short }
		is (ALU.ShiftRightArithmetic)       { io.out := (a_u >> b_short).asSInt }
		is (ALU.Equal)                      { io.out := (io.in.a === io.in.b).asSInt * -1.S }
		is (ALU.NotEqual)                   { io.out := (io.in.a =/= io.in.b).asSInt * -1.S }
		is (ALU.SignedLessThan)             { io.out := (io.in.a <  io.in.b).asSInt * -1.S }
		is (ALU.SignedLessThanOrEqual)      { io.out := (io.in.a <= io.in.b).asSInt * -1.S }
		is (ALU.SignedGreaterThan)          { io.out := (io.in.a >  io.in.b).asSInt * -1.S }
		is (ALU.SignedGreaterThanOrEqual)   { io.out := (io.in.a >= io.in.b).asSInt * -1.S }
		is (ALU.UnsignedLessThan)           { io.out := (a_u <  b_u).asSInt * -1.S }
		is (ALU.UnsignedLessThanOrEqual)    { io.out := (a_u <= b_u).asSInt * -1.S }
		is (ALU.UnsignedGreaterThan)        { io.out := (a_u >  b_u).asSInt * -1.S }
		is (ALU.UnsignedGreaterThanOrEqual) { io.out := (a_u >= b_u).asSInt * -1.S }
		is (ALU.BitwiseNOT)                 { io.out := ~io.in.a }
		is (ALU.LogicalNOT)                 { io.out := (io.in.a.asUInt.orR === 0.U).asSInt * -1.S }
	}
}

object ALU {
	val Zero                       = 0.U
	val Add                        = 1.U
	val Subtract                   = 2.U
	val SignedMultiply             = 3.U
	val UnsignedMultiply           = 4.U
	val SignedDivide               = 5.U
	val UnsignedDivide             = 6.U
	val SignedModulo               = 7.U
	val UnsignedModulo             = 8.U
	val BitwiseAND                 = 9.U
	val BitwiseOR                  = 10.U
	val BitwiseXOR                 = 11.U
	val BitwiseNAND                = 12.U
	val BitwiseNOR                 = 13.U
	val BitwiseXNOR                = 14.U
	val LogicalAND                 = 15.U
	val LogicalOR                  = 16.U
	val LogicalXOR                 = 17.U
	val LogicalNAND                = 18.U
	val LogicalNOR                 = 19.U
	val LogicalXNOR                = 20.U
	val ShiftLeftLogical           = 21.U
	val ShiftRightLogical          = 22.U
	val ShiftRightArithmetic       = 23.U
	val Equal                      = 24.U
	val NotEqual                   = 25.U
	val SignedLessThan             = 26.U
	val SignedLessThanOrEqual      = 27.U
	val SignedGreaterThan          = 28.U
	val SignedGreaterThanOrEqual   = 29.U
	val UnsignedLessThan           = 30.U
	val UnsignedLessThanOrEqual    = 31.U
	val UnsignedGreaterThan        = 32.U
	val UnsignedGreaterThanOrEqual = 33.U
	val BitwiseNOT                 = 34.U
	val LogicalNOT                 = 35.U
}
