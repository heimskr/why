package why

import chisel3._
import chisel3.util._

class BoothMultiplier extends Module {
	val io = IO(new Bundle {
		val in = Flipped(Valid(new Bundle {
			val m = SInt(64.W)
			val r = SInt(64.W)
		}))

		val out = Valid(SInt(128.W))
	})

	val sReady :: sBusy :: Nil = Enum(2)
	val state = RegInit(sReady)

	val m = RegInit(0.S(64.W))
	val r = RegInit(0.S(64.W))

	val a = RegInit(0.S(129.W))
	val s = RegInit(0.S(129.W))
	val p = RegInit(0.S(129.W))
	val t = RegInit(0.S(129.W))
	val c = RegInit(0.U(6.W))

	val out = RegInit(0.U.asTypeOf(chiselTypeOf(io.out)))
	io.out := out

	val step = RegInit(0.U(3.W))

	when (state === sReady && io.in.valid) {

		m := io.in.bits.m
		r := io.in.bits.r
		c := 0.U
		t := 0.U

		state := sBusy
		step  := 0.U

		out.bits  := 0.U
		out.valid := false.B

	} .elsewhen (state === sBusy) {

		when (step === 0.U) {

			a := Cat(m.asUInt, 0.U(65.W)).asSInt
			s := Cat((-m).asUInt, 0.U(65.W)).asSInt
			p := Cat(0.U(64.W), r.asUInt, 0.U(1.W)).asSInt
			step := 1.U

		} .elsewhen (step === 1.U) {

			t    := p
			step := 2.U

			switch (p(1, 0)) {
				is ("b01".U) {
					t := p + a
				}
				is ("b10".U) {
					t := p + s
				}
			}

		} .elsewhen (step === 2.U) {

			p := t >> 1

			when (c === 63.U) {
				out.bits  := t(128, 1)
				out.valid := true.B
				state     := sReady
			} .otherwise {
				c    := c + 1.U
				step := 1.U
			}

		}

	}
}
