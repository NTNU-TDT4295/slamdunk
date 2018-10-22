package rosetta

import Chisel._
import fpgatidbits.ocm._
import fpgatidbits.dma._
import fpgatidbits.streams._
import fpgatidbits.PlatformWrapper._

// Accelerator
// TODO: validate with logic analyser:
// - Internal: Vivado, verify shift regs are posedge
// - External: efm->zynq, verify transmission signals, stop-bit (1, 1.5, 2)
class SPI_Slave() extends RosettaAccelerator {
  val numMemPorts = 0
  val io = new RosettaAcceleratorIF(numMemPorts) {
    // val decouple = IODecoupled()
    // SPI - FPGA is slave
    // val spi_miso = Bits(OUTPUT, 1)
    // val spi_mosi = Bits(INPUT, 1)
    // val spi_sck  = Bits(INPUT, 1)
    // val spi_ss   = Bits(INPUT, 1)
    val spi_valid = Bool(OUTPUT)
    val spi_ready = Bool(INPUT)

    val read_data = Bits(OUTPUT, 32)
    val read_addr = Bits(INPUT, 11)
    // Decoupled
  }

  io.signature := makeDefaultSignature()

  io.led1 := false.B

  // Setup
  // sync SCK to the FPGA clock using 3-bit shift register
  val sckreg = Reg(init = Bits("b000", 3))
  sckreg := Cat(sckreg(1, 0), io.spi_sck)
  val sck_rise = Bool()
  sck_rise := sckreg(2, 1) === Bits("b01")
  // val sck_rise = sckreg(2) == Bits(0) && sckreg(1) == Bits(1) // detect SCK rise
  // val sck_fall = sckreg(2) == Bits(1) && sckreg(1) == Bits(0) // TX only

  // sync ss
  val ssreg = Reg(init = Bits("b000", 3))
  ssreg := Cat(ssreg(1, 0), io.spi_ss)
  val ss_active = Bool()
  ss_active := ~ssreg(1)
  // val ss_startmsg = ssreg(2) == UInt(1) && ssreg(1) == UInt(0)
  // val ss_endmsg = ssreg(2) == UInt(0) && ssreg(1) == UInt(1)
  val ss_startmsg = Bool()
  ss_startmsg := ssreg(2, 1) === Bits("b10")
  val ss_endmsg = Bool()
  ss_endmsg := ssreg(2, 1) === Bits("b01")

  // sync mosi
  val mosireg = Reg(init = Bits("b00", 2))
  mosireg := Cat(mosireg(0), io.spi_mosi)
  val mosi_data = Bool()
  mosi_data := mosireg(1)

  // Receive
  val bitcnt = Reg(init = Bits("b000", 3))
  val byte_received = Reg(Bool(), init = false.B)
  // val byte_data_received = Reg(UInt(width = 8), init = 0.U)
  val byte_data_received = Reg(UInt(width = 8))

  when (~ss_active) {
    bitcnt := 0.U
  } .otherwise {
    when (sck_rise) {
      bitcnt := bitcnt + 1.U

      // MSB first, left-shift data reg
      byte_data_received := Cat(byte_data_received(6, 0), mosi_data)
    }
  }

  // byte_received := ss_active && sck_rise && bitcnt === 7.U
  val byte_signal_received = Bool()
  byte_signal_received := ss_active && sck_rise && bitcnt === 7.U
  byte_received := byte_signal_received // TODO: remove

  val write_adress = Reg(init = UInt(0, width = 11)) // 2048 lines of words
  val byte_counter = Reg(init = UInt(0, width = 3)) // 4 bytes/word
  when (byte_signal_received) {
    byte_counter := (byte_counter + 1.U) % 5.U
  }

  val word_signal_received = Bool()
  word_signal_received := byte_signal_received && byte_counter === 4.U
  when (word_signal_received) { // 5 bytes received => next write_adress
    write_adress := write_adress + 1.U
  }

  val word_data_received = Reg(init = UInt(0, width = 32))
  word_data_received := Cat(byte_data_received, word_data_received(31, 8))
  val word_received = Reg(init = Bool(false))
  word_received := word_signal_received


  // when (byte_received) {
  //   io.led1 := true.B
  // }

  // io.led1 := byte_received


  // AXI handshake (source)
  // io.spi_valid := byte_received
  // io.spi_data := byte_data_received

  // Status LEDs
  // io.led0 := ~io.spi_ss // ss low, activity
  // io.led1 := io.spi_mosi
  // io.led3 := io.spi_mosi

  io.led3 := io.btn(3)

  // Choose buffer (double buffering) 0x0-0x7ff
  val bram = Module(new DualPortBRAM(addrBits = 11, dataBits = 32)).io
  val writePort = bram.ports(0)
  val readPort = bram.ports(1)

  writePort.req.addr := write_adress
  writePort.req.writeData := word_data_received
  writePort.req.writeEn := word_received

  readPort.req.writeEn := Bool(false)
  readPort.req.addr := io.read_addr
  // readPort.req.addr := io.readAddr
  io.read_data := readPort.rsp.readData

  // the result will appear the next cycle, it is not noticeable in
  // software, but be wary when interfacing with BRAM from chisel
  // io.readData := readPort.rsp.readData
}
