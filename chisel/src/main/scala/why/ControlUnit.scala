package why

import chisel3._
import chisel3.util._

class ControlUnit(memorySize: Int) extends Module {
	val io = IO(new Bundle {
		val memoryWordLE    = Input(UInt(64.W))
		val memoryWordBE    = Input(UInt(64.W))
		val memoryHWord     = Input(UInt(32.W)) // Always little-endian
		val memoryQWord     = Input(UInt(16.W)) // Always little-endian
		val memoryByte      = Input(UInt(8.W))
		val programCounter  = Input(UInt(64.W))
		val instruction     = Input(UInt(64.W))
		val instructionAddr = Input(UInt(64.W))
		val aluOut          = Input(SInt(64.W))
		val decoded         = Input(IDBundle())
		val state           = Output(ControlState.chiselType())
		val inputWord       = Output(UInt(64.W))
		val inputHWord      = Output(UInt(32.W))
		val inputQWord      = Output(UInt(16.W))
		val inputByte       = Output(UInt(8.W))
		val writeWord       = Output(Bool())
		val writeHWord      = Output(Bool())
		val writeQWord      = Output(Bool())
		val writeByte       = Output(Bool())
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

	val stateReg = RegInit(sStartup0.U)
	val state = Mux(io.overrides.stateEnable, io.overrides.state, stateReg)
	io.state := state

	val mar = RegInit(0.U(64.W))
	io.mar := mar

	when (state === sDebugPause.U) {
		io.mar := io.overrides.mar
	}
	
	val alu = Reg(ALUInput())
	io.alu := alu

	val writeWord = Wire(Bool())
	writeWord := false.B

	io.inputWord  := io.overrides.inputWord
	io.inputHWord := DontCare
	io.inputByte  := DontCare
	io.writeWord  := Mux(state === sDebugPause.U, io.overrides.writeWord, writeWord)
	io.writeHWord := false.B
	io.writeByte  := false.B
	io.pcInput    := 0.U
	io.pcWrite    := false.B
	io.irInput    := 0.U
	io.irWrite    := false.B

	io.registers.inputData   := DontCare
	io.registers.sID         := DontCare
	io.registers.tID         := DontCare
	io.registers.dID         := DontCare
	io.registers.writeEnable := false.B

	io.overrides.registers.sOutput := DontCare
	io.overrides.registers.tOutput := DontCare

	// When copying from one location in memory to another, this register stores the value of [$rs].
	val copyStore = Reg(UInt(64.W))

	switch (state) {
		// When the CPU first loads, it needs to set the program counter. The start of the code section is at the second
		// word in memory. It also needs to set the stack pointer to the last word in memory.
		is (sStartup0.U) {
			stateReg                 := sStartup1.U
			mar                      := 8.U
			io.registers.dID         := Registers.StackPointer
			io.registers.inputData   := (memorySize - 8).U
			io.registers.writeEnable := true.B
		}

		is (sStartup1.U) {
			stateReg   := sIf0.U
			io.pcInput := io.memoryWordBE
			io.pcWrite := true.B
		}

		is (sInvalid.U) {
			printf(p"Invalid state detected. Halting.\n")
			printf(p"PC: ${io.programCounter}\n")
			printf(p"Instruction: ${Hexadecimal(io.instruction)}\n")
			stateReg := sHalted.U
		}

		is (sHalted.U) { /* Stay halted forever. */ }

		is (sDebugPause.U) {
			io.registers <> io.overrides.registers
		}

		is (sIf0.U) {
			stateReg   := sIf1.U
			mar        := io.programCounter
			io.pcInput := io.programCounter + 8.U
			io.pcWrite := true.B
		}

		is (sIf1.U) {
			stateReg   := sIf2.U
			io.irInput := io.memoryWordBE
			io.irWrite := true.B
		}

		is (sIf2.U) {
			stateReg := sInvalid.U

			if (Settings.PrintOpcodes) {
				printf(p"[If2] instruction == 0x${Hexadecimal(io.instruction)}, pc == ${io.instructionAddr}, opcode == ${io.decoded.opcode}\n")
			}

			switch (io.decoded.opcode) {
				is (Opcodes.Nop.U)             { stateReg := sIf0.U             }
				is (Opcodes.Rmath.U)           { stateReg := sRmath.U           }
				is (Opcodes.Rlogic.U)          { stateReg := sRlogic.U          }
				is (Opcodes.Lui.U)             { stateReg := sLui.U             }
				is (Opcodes.SetIf.U)           { stateReg := sRSetIf.U          }
				is (Opcodes.Jump.U)            { stateReg := sJump.U            }
				is (Opcodes.JumpConditional.U) { stateReg := sJumpConditional.U }
				is (Opcodes.Memory.U)          { stateReg := sMemory0.U         }
				is (Opcodes.Set.U)             { stateReg := sSet.U             }
				is (Opcodes.External.U)        { stateReg := sExternal.U        }
				is (Opcodes.Rjump.U) {
					switch (io.decoded.rFunc) {
						is (0.U, 2.U) { stateReg := sRjump.U            }
						is (1.U, 3.U) { stateReg := sRjumpConditional.U }
					}
				}
				is (Opcodes.Addi.U, Opcodes.Subi.U, Opcodes.Multi.U, Opcodes.Modi.U, Opcodes.Slli.U, Opcodes.Srli.U,
				    Opcodes.Srai.U, Opcodes.Divi.U, Opcodes.Divii.U) {
					stateReg := sImath.U
				}
				is (Opcodes.Andi.U, Opcodes.Nandi.U, Opcodes.Nori.U, Opcodes.Ori.U, Opcodes.Xnori.U, Opcodes.Xori.U) {
					stateReg := sIlogic.U
				}
				is (Opcodes.Sli.U, Opcodes.Slei.U, Opcodes.Seqi.U, Opcodes.Sgi.U, Opcodes.Sgei.U) {
					stateReg := sISetIf.U
				}
			}

			when (stateReg === sInvalid.U) {
				printf(p"Opcode led to invalid state: ${io.decoded.opcode}\n")
			}
		}

		is (sRmath.U) {
			stateReg         := sRwriteALU.U
			io.registers.sID := io.decoded.rRs
			io.registers.tID := io.decoded.rRt
			alu.a            := io.registers.sOutput.asSInt
			alu.b            := io.registers.tOutput.asSInt
			switch (io.decoded.rFunc) {
				is (0.U)  { alu.op := ALU.Add                  }
				is (1.U)  { alu.op := ALU.Subtract             }
				is (2.U)  { alu.op := ALU.Multiply             }
				// 3–4 are missing for historical reasons, while 5 was removed for simplicity.
				is (6.U)  { alu.op := ALU.ShiftLeftLogical     }
				is (7.U)  { alu.op := ALU.ShiftRightLogical    }
				is (8.U)  { alu.op := ALU.ShiftRightArithmetic }
				is (9.U)  { alu.op := ALU.Modulo               }
				is (10.U) { alu.op := ALU.Divide               }
			}
		}
		
		is (sRlogic.U) {
			stateReg         := sRwriteALU.U
			io.registers.sID := io.decoded.rRs
			io.registers.tID := io.decoded.rRt
			alu.a            := io.registers.sOutput.asSInt
			alu.b            := io.registers.tOutput.asSInt
			switch (io.decoded.rFunc) {
				is (0.U)  { alu.op := ALU.BitwiseAND  }
				is (1.U)  { alu.op := ALU.BitwiseNAND }
				is (2.U)  { alu.op := ALU.BitwiseNOR  }
				is (3.U)  { alu.op := ALU.BitwiseNOT  }
				is (4.U)  { alu.op := ALU.BitwiseOR   }
				is (5.U)  { alu.op := ALU.BitwiseXNOR }
				is (6.U)  { alu.op := ALU.BitwiseXOR  }
				// 7 is missing due to a hiccup during WhySA's organic evolution.
				is (8.U)  { alu.op := ALU.LogicalAND  }
				is (9.U)  { alu.op := ALU.LogicalNAND }
				is (10.U) { alu.op := ALU.LogicalNOR  }
				is (11.U) { alu.op := ALU.LogicalNOT  }
				is (12.U) { alu.op := ALU.LogicalOR   }
				is (13.U) { alu.op := ALU.LogicalXNOR }
				is (14.U) { alu.op := ALU.LogicalXOR  }
			}
		}

		is (sRwriteALU.U) {
			stateReg                 := sIf0.U
			io.registers.dID         := io.decoded.rRd
			io.registers.inputData   := io.aluOut.asUInt
			io.registers.writeEnable := true.B
		}

		is (sSet.U) {
			stateReg                 := sIf0.U
			io.registers.dID         := io.decoded.iRd
			io.registers.inputData   := io.decoded.imm
			io.registers.writeEnable := true.B
		}

		is (sLui.U) {
			stateReg                 := sIf0.U
			io.registers.sID         := io.decoded.iRd
			io.registers.dID         := io.decoded.iRd
			io.registers.inputData   := Cat(io.decoded.imm, io.registers.sOutput(31, 0))
			io.registers.writeEnable := true.B
		}

		is (sExternal.U) {
			stateReg := sInvalid.U
			io.registers.sID := io.decoded.rRs

			switch (io.decoded.rFunc) {
				is (1.U) { // print register
					stateReg := sIf0.U
					printf(p"$$${io.decoded.rRs}: ${io.registers.sOutput}\n")
				}

				is (2.U) { // halt
					stateReg := sHalted.U
					printf(p"Machine halted.\n")
				}

				// 3 was a WhyJS-specific instruction that was removed for being irrelevant.

				is (4.U) { // print character
					stateReg := sIf0.U
					printf(p"${Character(io.registers.sOutput)}")
				}

				is (5.U) { // print decimal
					stateReg := sIf0.U
					printf(p"${io.registers.sOutput.asSInt}")
				}

				is (6.U) { // print hex
					stateReg := sIf0.U
					printf(p"${Hexadecimal(io.registers.sOutput)}")
				}

				// 7 was also removed.

				is (8.U) { // print binary
					stateReg := sIf0.U
					printf(p"${Binary(io.registers.sOutput)}")
				}
			}
		}

		is (sJump.U) {
			stateReg   := sIf0.U
			io.pcInput := io.decoded.imm
			io.pcWrite := true.B

			when (io.decoded.jLink) {
				io.registers.dID         := Registers.ReturnAddress
				io.registers.inputData   := io.programCounter // already has 8 added to it
				io.registers.writeEnable := true.B
			}
		}

		is (sJumpConditional.U) {
			stateReg         := sIf0.U
			io.registers.sID := io.decoded.jRs

			when (io.registers.sOutput =/= 0.U) {
				io.pcInput := io.decoded.imm
				io.pcWrite := true.B

				when (io.decoded.jLink) {
					io.registers.dID         := Registers.ReturnAddress
					io.registers.inputData   := io.programCounter // already has 8 added to it
					io.registers.writeEnable := true.B
				}
			}
		}

		is (sRjump.U) {
			stateReg         := sIf0.U
			io.registers.sID := io.decoded.rRd
			io.pcInput       := io.registers.sOutput
			io.pcWrite       := true.B

			when (io.decoded.rFunc === 2.U) {
				io.registers.dID         := Registers.ReturnAddress
				io.registers.inputData   := io.programCounter // already has 8 added to it
				io.registers.writeEnable := true.B
			}
		}

		is (sRjumpConditional.U) {
			stateReg         := sIf0.U
			io.registers.sID := io.decoded.rRs

			when (io.registers.sOutput =/= 0.U) {
				io.registers.tID := io.decoded.rRd
				io.pcInput       := io.registers.tOutput
				io.pcWrite       := true.B

				when (io.decoded.rFunc === 3.U) {
					io.registers.dID         := Registers.ReturnAddress
					io.registers.inputData   := io.programCounter // already has 8 added to it
					io.registers.writeEnable := true.B
				}
			}
		}

		is (sRSetIf.U) {
			stateReg         := sInvalid.U
			io.registers.sID := io.decoded.rRs
			io.registers.tID := io.decoded.rRt
			alu.a            := io.registers.sOutput.asSInt
			alu.b            := io.registers.tOutput.asSInt
			switch (io.decoded.rFunc) {
				is (0.U) {
					stateReg := sRwriteALU.U
					alu.op   := ALU.SignedLessThan
				}

				is (1.U) {
					stateReg := sRwriteALU.U
					alu.op   := ALU.SignedLessThanOrEqual
				}

				is (2.U) {
					stateReg := sRwriteALU.U
					alu.op   := ALU.Equal
				}

				is (3.U) {
					stateReg := sRwriteALU.U
					alu.op   := ALU.UnsignedLessThan
				}

				is (4.U) {
					stateReg := sRwriteALU.U
					alu.op   := ALU.UnsignedLessThanOrEqual
				}
			}
		}

		is (sMemory0.U) {
			stateReg := sInvalid.U
			switch (io.decoded.rFunc) {
				is (0.U) { // copy: [$rs] -> [$rd]
					stateReg         := sMemory1.U
					io.registers.sID := io.decoded.rRs
					mar              := io.registers.sOutput
				}

				is (1.U) { // load: [$rs] -> $rd
					stateReg         := sMemory1.U
					io.registers.sID := io.decoded.rRs
					mar              := io.registers.sOutput
				}

				is (2.U) { // store: $rs -> [$rd]
					stateReg         := sMemory1.U
					io.registers.sID := io.decoded.rRd
					mar              := io.registers.sOutput
				}

				is (3.U) { // copy byte: [$rs] -> [$rd] /b
					stateReg         := sMemory1.U
					io.registers.sID := io.decoded.rRs
					mar              := io.registers.sOutput
				}

				is (4.U) { // load byte: [$rs] -> $rd /b
					stateReg         := sMemory1.U
					io.registers.sID := io.decoded.rRs
					mar              := io.registers.sOutput
				}

				is (5.U) { // store byte: $rs -> [$rd] /b
					stateReg         := sMemory1.U
					io.registers.sID := io.decoded.rRd
					mar              := io.registers.sOutput
				}

				is (6.U) { // stack push: [ $rs
					// In the first stage, we write to memory.
					stateReg         := sMemory1.U
					io.registers.sID := Registers.StackPointer
					mar              := io.registers.sOutput
				}

				is (7.U) { // stack pop: ] $rd
					// In the first stage, we increment the stack pointer by one word (8 bytes) and read from memory.
					stateReg                 := sMemory1.U
					io.registers.sID         := Registers.StackPointer
					io.registers.dID         := Registers.StackPointer
					io.registers.inputData   := io.registers.sOutput + 8.U
					io.registers.writeEnable := true.B
					mar                      := io.registers.sOutput + 8.U
				}

				is (8.U) { // copy halfword: [$rs] -> [$rd] /h
					stateReg         := sMemory1.U
					io.registers.sID := io.decoded.rRs
					mar              := io.registers.sOutput
				}

				is (9.U) { // load halfword: [$rs] -> $rd /h
					stateReg         := sMemory1.U
					io.registers.sID := io.decoded.rRs
					mar              := io.registers.sOutput
				}

				is (10.U) { // store halfword: $rs -> [$rd] /h
					stateReg         := sMemory1.U
					io.registers.sID := io.decoded.rRd
					mar              := io.registers.sOutput
				}
			}
		}

		is (sMemory1.U) {
			stateReg := sInvalid.U
			switch (io.decoded.rFunc) {
				is (0.U) { // copy: [$rs] -> [$rd]
					stateReg         := sMemory2.U
					copyStore        := io.memoryWordLE
					io.registers.sID := io.decoded.rRd
					mar              := io.registers.sOutput
				}

				is (1.U) { // load: [$rs] -> $rd
					stateReg                 := sIf0.U
					io.registers.dID         := io.decoded.rRd
					io.registers.inputData   := io.memoryWordLE
					io.registers.writeEnable := true.B
				}

				is (2.U) { // store: $rs -> [$rd]
					stateReg         := sIf0.U
					io.registers.sID := io.decoded.rRs
					io.inputWord     := io.registers.sOutput
					writeWord        := true.B
				}

				is (3.U) { // copy byte: [$rs] -> [$rd] /b
					stateReg         := sMemory2.U
					copyStore        := io.memoryByte
					io.registers.sID := io.decoded.rRd
					mar              := io.registers.sOutput
				}

				is (4.U) { // load byte: [$rs] -> $rd /b
					stateReg                 := sIf0.U
					io.registers.dID         := io.decoded.rRd
					io.registers.inputData   := io.memoryByte
					io.registers.writeEnable := true.B
				}

				is (5.U) { // store byte: $rs -> [$rd] /b
					stateReg         := sIf0.U
					io.registers.sID := io.decoded.rRs
					io.inputByte     := io.registers.sOutput(7, 0)
					io.writeByte     := true.B
				}

				is (6.U) { // stack push: [ $rs
					// In the second stage, we decrement the stack pointer by one word (8 bytes).
					stateReg                 := sIf0.U
					io.registers.sID         := io.decoded.rRs
					io.inputWord             := io.registers.sOutput
					writeWord                := true.B
					io.registers.tID         := Registers.StackPointer
					io.registers.dID         := Registers.StackPointer
					io.registers.inputData   := io.registers.tOutput - 8.U
					io.registers.writeEnable := true.B
				}

				is (7.U) { // stack pop: ] $rd
					// In the second stage, we copy from memory to the register.
					stateReg                 := sIf0.U
					io.registers.dID         := io.decoded.rRd
					io.registers.inputData   := io.memoryWordLE
					io.registers.writeEnable := true.B
				}

				is (8.U) { // copy halfword: [$rs] -> [$rd] /h
					stateReg         := sMemory2.U
					copyStore        := io.memoryHWord
					io.registers.sID := io.decoded.rRd
					mar              := io.registers.sOutput
				}

				is (9.U) { // load halfword: [$rs] -> $rd /h
					stateReg                 := sIf0.U
					io.registers.dID         := io.decoded.rRd
					io.registers.inputData   := io.memoryHWord
					io.registers.writeEnable := true.B
				}

				is (10.U) { // store halfword: $rs -> [$rd] /h
					stateReg         := sIf0.U
					io.registers.sID := io.decoded.rRs
					io.inputHWord    := io.registers.sOutput(31, 0)
					io.writeHWord    := true.B
				}
			}
		}

		is (sMemory2.U) {
			stateReg := sInvalid.U
			switch (io.decoded.rFunc) {
				is (0.U) { // copy: [$rs] -> [$rd]
					stateReg     := sIf0.U
					io.inputWord := copyStore
					writeWord    := true.B
				}

				is (3.U) { // copy byte: [$rs] -> [$rd] /b
					stateReg     := sIf0.U
					io.inputByte := copyStore
					io.writeByte := true.B
				}

				is (8.U) { // copy halfword: [$rs] -> [$rd] /h
					stateReg      := sIf0.U
					io.inputHWord := copyStore
					io.writeHWord := true.B
				}
			}
		}

		is (sImath.U) {
			stateReg         := sIwriteALU.U
			io.registers.sID := io.decoded.iRs
			alu.op           := ALU.Zero

			when (io.decoded.opcode === Opcodes.Divii.U) {
				alu.a        := io.decoded.imm.asSInt
				alu.b        := io.registers.sOutput.asSInt
			} .otherwise {
				alu.a        := io.registers.sOutput.asSInt
				alu.b        := io.decoded.imm.asSInt
			}

			switch (io.decoded.opcode) {
				is (Opcodes.Addi.U)  { alu.op := ALU.Add                  }
				is (Opcodes.Subi.U)  { alu.op := ALU.Subtract             }
				is (Opcodes.Multi.U) { alu.op := ALU.Multiply             }
				is (Opcodes.Modi.U)  { alu.op := ALU.Modulo               }
				is (Opcodes.Slli.U)  { alu.op := ALU.ShiftLeftLogical     }
				is (Opcodes.Srli.U)  { alu.op := ALU.ShiftRightLogical    }
				is (Opcodes.Srai.U)  { alu.op := ALU.ShiftRightArithmetic }
				is (Opcodes.Divi.U)  { alu.op := ALU.Divide               }
				is (Opcodes.Divii.U) { alu.op := ALU.Divide               }
			}
		}

		is (sIlogic.U) {
			stateReg         := sIwriteALU.U
			io.registers.sID := io.decoded.iRs
			alu.a            := io.registers.sOutput.asSInt
			alu.b            := io.decoded.imm.asSInt
			switch (io.decoded.opcode) {
				is (Opcodes.Andi.U)  { alu.op := ALU.BitwiseAND  }
				is (Opcodes.Nandi.U) { alu.op := ALU.BitwiseNAND }
				is (Opcodes.Nori.U)  { alu.op := ALU.BitwiseNOR  }
				is (Opcodes.Ori.U)   { alu.op := ALU.BitwiseOR   }
				is (Opcodes.Xnori.U) { alu.op := ALU.BitwiseXNOR }
				is (Opcodes.Xori.U)  { alu.op := ALU.BitwiseXOR  }
			}
		}

		is (sIwriteALU.U) {
			stateReg                 := sIf0.U
			io.registers.dID         := io.decoded.iRd
			io.registers.inputData   := io.aluOut.asUInt
			io.registers.writeEnable := true.B
		}

		is (sISetIf.U) {
			stateReg         := sInvalid.U
			io.registers.sID := io.decoded.iRs
			alu.a            := io.registers.sOutput.asSInt
			alu.b            := io.decoded.imm.asSInt
			switch (io.decoded.opcode) {
				is (Opcodes.Sli.U) {
					stateReg := sIwriteALU.U
					alu.op   := ALU.SignedLessThan
				}

				is (Opcodes.Slei.U) {
					stateReg := sIwriteALU.U
					alu.op   := ALU.SignedLessThanOrEqual
				}

				is (Opcodes.Seqi.U) {
					stateReg := sIwriteALU.U
					alu.op   := ALU.Equal
				}

				is (Opcodes.Sgi.U) {
					stateReg := sIwriteALU.U
					alu.op   := ALU.SignedGreaterThan
				}

				is (Opcodes.Sgei.U) {
					stateReg := sIwriteALU.U
					alu.op   := ALU.SignedGreaterThanOrEqual
				}
			}
		}
	}
}
