package why

import chisel3._
import chisel3.util._

sealed abstract class ControlState(val id: Int) {
	def U: UInt = id.U
}

object ControlState {
	val all: Set[ControlState] = Set(
		sStartup0, sStartup1, sInvalid, sDebugPause, sHalted, sIf0, sIf1, sIf2, sRmath, sRlogic, sRwriteALU, sSet, sLui,
		sExternal, sJump, sJumpConditional, sRjump, sRjumpConditional, sRSetIf, sMemory0, sMemory1, sMemory2, sImath,
		sIlogic, sIwriteALU, sISetIf
	)

	def count: Int = log2Ceil(all.size)
	def width = count.W
	def chiselType() = UInt(width)
}

case object sStartup0         extends ControlState(0)
case object sStartup1         extends ControlState(1)
case object sInvalid          extends ControlState(2)
case object sDebugPause       extends ControlState(3)
case object sHalted           extends ControlState(4)
case object sIf0              extends ControlState(5)
case object sIf1              extends ControlState(6)
case object sIf2              extends ControlState(7)
case object sRmath            extends ControlState(8)
case object sRlogic           extends ControlState(9)
case object sRwriteALU        extends ControlState(10)
case object sSet              extends ControlState(11)
case object sLui              extends ControlState(12)
case object sExternal         extends ControlState(13)
case object sJump             extends ControlState(14)
case object sJumpConditional  extends ControlState(15)
case object sRjump            extends ControlState(16)
case object sRjumpConditional extends ControlState(17)
case object sRSetIf           extends ControlState(18)
case object sMemory0          extends ControlState(19)
case object sMemory1          extends ControlState(20)
case object sMemory2          extends ControlState(21)
case object sImath            extends ControlState(22)
case object sIlogic           extends ControlState(23)
case object sIwriteALU        extends ControlState(24)
case object sISetIf           extends ControlState(25)
