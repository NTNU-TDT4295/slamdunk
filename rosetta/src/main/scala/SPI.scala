package rosetta

import Chisel._
import fpgatidbits.ocm._
import fpgatidbits.dma._
import fpgatidbits.streams._
import fpgatidbits.PlatformWrapper._

// Accelerator
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

    val lidar_burst_counter = UInt(OUTPUT, 32)

    val read_data = Bits(OUTPUT, 32)
    val read_addr = Bits(INPUT, 11)
    // Decoupled
  }

  io.signature := makeDefaultSignature()

  // Setup
  // sync SCK to the FPGA clock using 3-bit shift register
  val sckreg = Reg(init = Bits("b000", 3))
  sckreg := Cat(sckreg(1, 0), io.spi_sck)
  val sck_rise = Bool()
  sck_rise := sckreg(2, 1) === Bits("b01")

  // sync ss
  val ssreg = Reg(init = Bits("b000", 3))
  ssreg := Cat(ssreg(1, 0), io.spi_ss)
  val ss_active = Bool()
  ss_active := ~ssreg(1)
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

  val byte_signal_received = Reg(init = Bool(false))
  val write_address = Reg(init = UInt(0, width = 11)) // 4096 lines of words
  val write_address_delayed = Reg(next = write_address) // Needed to delay write_address
  val byte_counter = Reg(init = UInt(0, width = 2))  // 4 bytes/word
  val word_data_received = Reg(init = UInt(0, width = 32))
  val word_signal_received = Reg(init = Bool(false))
  val word_received = Reg(init = Bool(false))

  val lidar_burst_counter = Reg(init = UInt(0, width = 32))

  byte_signal_received := ss_active && sck_rise && bitcnt === 7.U

  word_signal_received := byte_signal_received && byte_counter === 3.U
  when (byte_signal_received) {
    word_data_received := Cat(word_data_received(23, 0), byte_data_received)
    byte_counter := byte_counter + 1.U
  }

  // Simple double buffering based on 1800 bytes
  // of lidar data bursts.
  when (word_signal_received) { // 4 bytes received => next write_address
    when (write_address === 449.U) {
      write_address := 512.U
      lidar_burst_counter := lidar_burst_counter + 1.U
    } .elsewhen (write_address === 961.U) {
      write_address := 0.U
      lidar_burst_counter := lidar_burst_counter + 1.U
    } .otherwise {
      write_address := write_address + 1.U
    }
  }

  io.lidar_burst_counter := lidar_burst_counter

  word_received := word_signal_received

  io.led3 := io.btn(3)

  // Choose buffer (double buffering) 0x0-0x7ff
  val bram = Module(new DualPortBRAM(addrBits = 11, dataBits = 32)).io
  val writePort = bram.ports(0)
  val readPort = bram.ports(1)

  writePort.req.addr := write_address_delayed
  writePort.req.writeData := word_data_received
  writePort.req.writeEn := word_received

  readPort.req.writeEn := Bool(false)
  readPort.req.addr := io.read_addr
  io.read_data := readPort.rsp.readData

  // the result will appear the next cycle, it is not noticeable in
  // software, but be wary when interfacing with BRAM from chisel
}
