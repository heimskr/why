package why

package educpu

import chisel3._
import chisel3.util.log2Ceil

// A bank of registers capable of reading from two registers and writing to one register in one clock cycle.
// Reading and writing the same register at once results in undefined behavior.
class RegisterBank(count: Int, width: Int = 64) extends Module {
	val io = IO(RegisterBankIO(log2Ceil(count), width))

	val dataStore = RegInit(VecInit(Seq.fill(count)(0.U(width.W))))

	when (io.writeEnable) {
		if (Settings.DebugRegisterWrites) {
			when (io.dID =/= 104.U) { // Ignore writes to $m2, which is used for the <prc> instruction.
				printf(p"Writing ${io.inputData} to register ${io.dID}\n")
			}
		}

		dataStore(io.dID) := io.inputData
	}

	io.sOutput := dataStore(io.sID)
	io.tOutput := dataStore(io.tID)
}
