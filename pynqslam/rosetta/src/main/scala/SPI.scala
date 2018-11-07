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
    // Represents signals in RosettaAcceleratorIF used in this RosettaAccelerator
    // val spi_miso = Bits(OUTPUT, 1)
    // val spi_mosi = Bits(INPUT, 1)
    // val spi_sck  = Bits(INPUT, 1)
    // val spi_ss   = Bits(INPUT, 1)
    // val buffer_reset = Bits(INPUT, 1)

    val valid_buffer = Bool(OUTPUT)

    val read_data = Bits(OUTPUT, 32)
    val read_addr = Bits(INPUT, 11)
    val read_size = Bits(OUTPUT, 11) // Number of valid points in valid buffer
  }

  io.signature := makeDefaultSignature()

  // Setup
  // sync SCK to the FPGA clock using 3-bit shift register
  val sckreg   = Reg(init = Bits("b000", 3))
  val sck_rise = Bool()
  sckreg      := Cat(sckreg(1, 0), io.spi_sck)
  sck_rise    := sckreg(2, 1) === Bits("b01")

  // sync ss
  val ssreg       = Reg(init = Bits("b000", 3))
  val ss_active   = Bool()
  val ss_startmsg = Bool()
  val ss_endmsg   = Bool()
  ssreg          := Cat(ssreg(1, 0), io.spi_ss)
  ss_active      := ~ssreg(1)
  ss_startmsg    := ssreg(2, 1) === Bits("b10")
  ss_endmsg      := ssreg(2, 1) === Bits("b01")

  // sync mosi
  val mosireg   = Reg(init = Bits("b00", 2))
  val mosi_data = Bool()
  mosireg      := Cat(mosireg(0), io.spi_mosi)
  mosi_data    := mosireg(1)

  // Receive
  val bitcnt = Reg(init = Bits("b000", 3))
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

  val byte_signal_received  = Reg(init = Bool(false))
  val write_address         = Reg(init = UInt(0, width = 11))
  val byte_counter          = Reg(init = UInt(0, width = 3))

  val packet_data           = Reg(init = UInt(0, width = 40))
  val write_data            = Reg(init = UInt(0, width = 32))
  val current_buffer        = Reg(Bool())
  val current_buffer_size   = Reg(init = UInt(0, width = 11))
  val valid_buffer_size     = Reg(init = UInt(0, width = 11))
  val write_enable          = Reg(init = Bool(false))
  val sync_signal           = Reg(init = Bool(false))

  byte_signal_received := ss_active && sck_rise && bitcnt === 7.U

  when (byte_signal_received) {
    packet_data  := Cat(packet_data(31, 0), byte_data_received)
    byte_counter := byte_counter + 1.U
  }
 
  val packet_received   = Reg(init = Bool(false))
  val packet_quality    = packet_data(39, 33)

  sync_signal := false.B
  write_enable := false.B

  packet_received := byte_signal_received && (byte_counter === 5.U)

  when (packet_received) {
    byte_counter := 0.U
    sync_signal := packet_data(32) === 1.U

    when (packet_quality > 10.U) {
      write_data := packet_data(31, 0)
      write_enable := true.B
      current_buffer_size := current_buffer_size + 1.U
    }
  }

  when (sync_signal) {
    when (current_buffer) {
      write_address := 512.U
    } .otherwise {
      write_address := 0.U
    }

    current_buffer := ~current_buffer
    valid_buffer_size := current_buffer_size
    current_buffer_size := 0.U
  }

  when (write_enable) {
    write_address := write_address + 1.U
  }

  when (~io.buffer_reset) { // Pullup button
    byte_counter          := 0.U
    write_address         := 0.U
    current_buffer_size   := 0.U
    valid_buffer_size     := 0.U
  }

  io.io4_led := false.B
  when (write_address === 0.U) {
    io.io4_led := true.B
  }

  io.valid_buffer := current_buffer

  // Choose buffer (double buffering in BRAM)
  val bram      = Module(new DualPortBRAM(addrBits = 11, dataBits = 32)).io
  val writePort = bram.ports(0)
  val readPort  = bram.ports(1)

  writePort.req.addr      := write_address
  writePort.req.writeData := write_data
  writePort.req.writeEn   := write_enable

  readPort.req.writeEn    := Bool(false)
  readPort.req.addr       := io.read_addr
  io.read_data            := readPort.rsp.readData

  // printf("\t\tCurrent buffer = %d\t\tpacket_received = %d\n", current_buffer, packet_received)
  // printf("packet_data = %x\n", packet_data)
  // printf("\t\twrite_data = %x\n", write_data)

  // the result will appear the next cycle, it is not noticeable in
  // software, but be wary when interfacing with BRAM from chisel
}
