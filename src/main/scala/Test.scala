package rosetta

import Chisel._
import fpgatidbits.ocm._
import fpgatidbits.dma._
import fpgatidbits.streams._
import fpgatidbits.PlatformWrapper._

// (TODO): split this stuff into relevant modules when actual work is
// to be done
class DMPROTest() extends RosettaAccelerator {
  val p = PYNQZ1Params;
  val numMemPorts = 2

  val io = new RosettaAcceleratorIF(numMemPorts) {
    // For testing BRAM, not DMPROTest
    val writeEnable = Bool(INPUT)
    val writeAddr = UInt(INPUT, width = 32)
    val writeData = UInt(INPUT, width = 64)
    val readAddr = UInt(INPUT, width = 32)
    val readData = UInt(OUTPUT, width = 64)

    // For testing DRAM
    val start = Bool(INPUT)
    val finished = Bool(OUTPUT)
    val readBaseAddr = UInt(INPUT, width = 64)
    val writeBaseAddr = UInt(INPUT, width = 64)
    val byteCount = UInt(INPUT, width = 32)
  }

  // -- BRAM module --
  val bram = Module(new DualPortBRAM(addrBits = 16, dataBits = 64)).io

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

  // -- DRAM module --
  val reader = Module(new StreamReader(new StreamReaderParams(
    streamWidth = p.memDataBits, fifoElems = 8, mem = PYNQZ1Params.toMemReqParams(),
    maxBeats = 1, chanID = 0, disableThrottle = true
  ))).io

  val writer = Module(new StreamWriter(new StreamWriterParams(
    streamWidth = p.memDataBits, mem = PYNQZ1Params.toMemReqParams(), chanID = 0
  ))).io

  // Wiring up the reader to its memory port interface
  reader.start := io.start
  reader.baseAddr := io.readBaseAddr
  reader.byteCount := io.byteCount
  reader.req <> io.memPort(0).memRdReq
  io.memPort(0).memRdRsp <> reader.rsp
  plugMemWritePort(0)

  // Wire up the writer to its memory port interface
  writer.start := io.start
  writer.baseAddr := io.writeBaseAddr
  writer.byteCount := io.byteCount
  writer.req <> io.memPort(1).memWrReq
  writer.wdat <> io.memPort(1).memWrDat
  io.memPort(1).memWrRsp <> writer.rsp
  plugMemReadPort(1)

  // Connect reader and writer
  reader.out <> writer.in

  // Assert that we are done only when the writer is done
  io.finished := writer.finished
}
