package rosetta

import Chisel._
import fpgatidbits.ocm._

class EchoNumber() extends RosettaAccelerator {
  val numMemPorts = 0
  val numMemLines = 1024
  val dataOutReg = Reg(init = UInt(0, 4))
  val dataReadyReg = Reg(init = Bool(false))

  val io = new RosettaAcceleratorIF(numMemPorts) {
    val dataIn = UInt(INPUT, 4)
    val dataWrite = Bool(INPUT)

    val dataOut = UInt(OUTPUT, 4)
    val dataReady = Bool(OUTPUT)

    // For testing BRAM, not EchoNumber
    val writeEnable = Bool(INPUT)
    val writeAddr = UInt(INPUT, width = log2Up(numMemLines))
    val writeData = UInt(INPUT, width = 32)
    val readAddr = UInt(INPUT, width = log2Up(numMemLines))
    val readData = UInt(OUTPUT, width = 32)
  }

  when (io.dataWrite) {
    dataOutReg := io.dataIn
    dataReadyReg := Bool(true)
  }

  io.dataOut := dataOutReg
  io.dataReady := dataReadyReg

  // BRAM module
  val bram = Module(new DualPortBRAM(addrBits = log2Up(numMemLines), dataBits = 32)).io

  val writePort = bram.ports(0)
  val readPort = bram.ports(1)

  writePort.req.addr := io.writeAddr
  writePort.req.writeData := io.writeData
  writePort.req.writeEn := io.writeEnable

  readPort.req.writeEn := Bool(false)
  readPort.req.addr := io.readAddr

  // the result will appear the next cycle, it is not noticeable in
  // software, but be wary when interfacing with BRAM from chisel
  io.readData := readPort.rsp.readData
}
