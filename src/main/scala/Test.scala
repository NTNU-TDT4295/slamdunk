package rosetta

import Chisel._

class EchoNumber() extends RosettaAccelerator {
  val numMemPorts = 0
  val dataOutReg = Reg(init = UInt(0, 4))
  val dataReadyReg = Reg(init = Bool(false))

  val io = new RosettaAcceleratorIF(numMemPorts) {
    val dataIn = UInt(INPUT, 4)
    val dataWrite = Bool(INPUT)

    val dataOut = UInt(OUTPUT, 4)
    val dataReady = Bool(OUTPUT)
  }

  when (io.dataWrite) {
    dataOutReg := io.dataIn
    dataReadyReg := Bool(true)
  }

  io.dataOut := dataOutReg
  io.dataReady := dataReadyReg
}
