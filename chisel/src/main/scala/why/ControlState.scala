package why

import chisel3._
import chisel3.util._

sealed abstract class ControlState(val id: Int) {
	def U: UInt = id.U
}

object ControlState {
	val all: Set[ControlState] = Set(

	)

	def count: Int = log2Ceil(all.size)
	def width = count.W
	def chiselType() = UInt(width)
}

// case object sStartup0 extends ControlState(0)
