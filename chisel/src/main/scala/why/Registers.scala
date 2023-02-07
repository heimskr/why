package why

import chisel3._

object Registers {
	def registerName(id: Int): String = {
		id match {
			case 0   => "\u001b[90m$0\u001b[39m";
			case 1   => "\u001b[36m$g\u001b[39m";
			case 2   => "\u001b[31m$sp\u001b[39m";
			case 3   => "\u001b[35m$fp\u001b[39m";
			case 4   => "\u001b[32m$rt\u001b[39m";
			case 5   => "\u001b[31m$lo\u001b[39m";
			case 6   => "\u001b[31m$hi\u001b[39m";
			case 101 => "$st";
			case 118 => "$ts";
			case _   => {
				def hex(offset: Int): String = { (id - offset).toHexString }
				val list = List(
					(  7, 16, "31m$r"),
					( 23, 16, "38;5;202m$a"),
					( 39, 23, "33m$t"),
					( 62, 23, "32m$s"),
					( 85, 16, "38;5;33m$k"),
					(122,  6, "38;5;88m$e"),
					(102, 16, "38;5;129m$m"),
					(119,  3, "38;5;126m$f")
				)

				for ((offset, length, prefix) <- list) {
					if (offset <= id && id < offset + length)
						return "\u001b[" + prefix + hex(offset) + "\u001b[39m"
				}

				"\u001b[31m$?\u001b[39m"
			}
		}
	}

	val Zero              = 0.U
	val GlobalAreaPointer = 1.U
	val StackPointer      = 2.U
	val ReturnAddress     = 4.U
	val Lo                = 5.U
	val Hi                = 6.U
	val StatusRegister    = 101.U
	val TypeStackPointer  = 118.U
}
