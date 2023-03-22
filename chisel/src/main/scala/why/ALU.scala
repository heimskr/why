package why

import chisel3._
import chisel3.util._

class ALU extends Module {
	val io = IO(new Bundle {
		val in  = Flipped(Valid(ALUInput()))
		val out = Valid(UInt(128.W))
	})

	val a = io.in.bits.a
	val b = io.in.bits.b
	val aS = io.in.bits.a.asSInt
	val bS = io.in.bits.b.asSInt

	val shiftOverflow = 64.U <= b

	io.out.bits  := DontCare
	io.out.valid := false.B

	val sReady :: sSignedMultiply :: sUnsignedMultiply :: Nil = Enum(3)
	val state = RegInit(sReady)

	val booth = Module(new BoothMultiplier)
	booth.io.in.bits  := DontCare
	booth.io.in.valid := false.B


	when (state === sSignedMultiply) {

		when (booth.io.out.valid) {
			io.out.bits  := booth.io.out.bits
			io.out.valid := true.B
			state := sReady
		}

	} .elsewhen (state === sReady) {

		switch (io.in.bits.op) {
			is (ALU.Zero) {
				io.out.bits  := 0.S
				io.out.valid := true.B
			}

			is (ALU.Add) {
				io.out.bits  := aS + bS
				io.out.valid := true.B
			}

			is (ALU.Subtract) {
				io.out.bits  := aS - bS
				io.out.valid := true.B
			}

			is (ALU.SignedMultiply) {
				booth.io.in.bits.m := aS
				booth.io.in.bits.r := bS
				booth.io.in.valid  := true.B
				state := sSignedMultiply
			}

			is (ALU.UnsignedMultiply) {
				// io.out.bits  := (a * b).asSInt
				// io.out.valid := true.B
			}

			is (ALU.SignedDivide) {
				// io.out.bits  := aS / bS
				// io.out.valid := true.B
			}

			is (ALU.UnsignedDivide) {
				// io.out.bits  := (a / b).asSInt
				// io.out.valid := true.B
			}

			is (ALU.SignedModulo) {
				// io.out.bits  := aS % bS
				// io.out.valid := true.B
			}

			is (ALU.UnsignedModulo) {
				// io.out.bits  := (a % b).asSInt
				// io.out.valid := true.B
			}

			is (ALU.BitwiseAND) {
				io.out.bits  := a & b
				io.out.valid := true.B
			}

			is (ALU.BitwiseOR) {
				io.out.bits  := a | b
				io.out.valid := true.B
			}

			is (ALU.BitwiseXOR) {
				io.out.bits  := a ^ b
				io.out.valid := true.B
			}

			is (ALU.BitwiseNAND) {
				io.out.bits  := ~(a & b)
				io.out.valid := true.B
			}

			is (ALU.BitwiseNOR) {
				io.out.bits  := ~(a | b)
				io.out.valid := true.B
			}

			is (ALU.BitwiseXNOR) {
				io.out.bits  := ~(a ^ b)
				io.out.valid := true.B
			}

			is (ALU.LogicalAND) {
				io.out.bits  := (a.orR & b.orR) =/= 0.U
				io.out.valid := true.B
			}

			is (ALU.LogicalOR) {
				io.out.bits  := (a.orR | b.orR) =/= 0.U
				io.out.valid := true.B
			}

			is (ALU.LogicalXOR) {
				io.out.bits  := (a.orR ^ b.orR) =/= 0.U
				io.out.valid := true.B
			}

			is (ALU.LogicalNAND) {
				io.out.bits  := (a.orR & b.orR) === 0.U
				io.out.valid := true.B
			}

			is (ALU.LogicalNOR) {
				io.out.bits  := (a.orR | b.orR) === 0.U
				io.out.valid := true.B
			}

			is (ALU.LogicalXNOR) {
				io.out.bits  := (a.orR ^ b.orR) === 0.U
				io.out.valid := true.B
			}

			is (ALU.ShiftLeftLogical) {
				io.out.bits  := Mux(shiftOverflow, 0.U, a << b(5, 0))
				io.out.valid := true.B
			}

			is (ALU.ShiftRightLogical) {
				io.out.bits  := Mux(shiftOverflow, 0.U, a >> b(5, 0))
				io.out.valid := true.B
			}

			is (ALU.ShiftRightArithmetic) {
				when (shiftOverflow) {
					io.out.bits := Mux(a(63), -1.U(64.W).asSInt, 0.U)
				} .otherwise {
					io.out.bits := a >> b(5, 0)
				}
				io.out.valid := true.B
			}

			is (ALU.Equal) {
				io.out.bits  := a === b
				io.out.valid := true.B
			}

			is (ALU.NotEqual) {
				io.out.bits  := a =/= b
				io.out.valid := true.B
			}

			is (ALU.SignedLessThan) {
				io.out.bits  := aS <  bS
				io.out.valid := true.B
			}

			is (ALU.SignedLessThanOrEqual) {
				io.out.bits  := aS <= bS
				io.out.valid := true.B
			}

			is (ALU.SignedGreaterThan) {
				io.out.bits  := aS >  bS
				io.out.valid := true.B
			}

			is (ALU.SignedGreaterThanOrEqual) {
				io.out.bits  := aS >= bS
				io.out.valid := true.B
			}

			is (ALU.UnsignedLessThan) {
				io.out.bits  := a <  b
				io.out.valid := true.B
			}

			is (ALU.UnsignedLessThanOrEqual) {
				io.out.bits  := a <= b
				io.out.valid := true.B
			}

			is (ALU.UnsignedGreaterThan) {
				io.out.bits  := a >  b
				io.out.valid := true.B
			}

			is (ALU.UnsignedGreaterThanOrEqual) {
				io.out.bits  := a >= b
				io.out.valid := true.B
			}

			is (ALU.BitwiseNOT) {
				io.out.bits  := ~a
				io.out.valid := true.B
			}

			is (ALU.LogicalNOT) {
				io.out.bits  := a === 0.U
				io.out.valid := true.B
			}
		}

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
