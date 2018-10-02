package rosetta

import Chisel._

// Accelerator
class SPI_Slave() extends RosettaAccelerator {
  val numMemPorts = 0
  val io = new RosettaAcceleratorIF(numMemPorts) {
    // SPI - FPGA is slave
    // val spi_miso = Bits(OUTPUT, 1)
    // val spi_mosi = Bits(INPUT, 1)
    // val spi_sck  = Bool(INPUT)
    // val spi_ss   = Bool(INPUT)
  }

  // Setup
  val clk_rate = 100000000
  // sync SCK to the FPGA clock using a 3-bits shift register
  val sckreg = Reg(init = Bits("b000", 3))
  sckreg := Cat(sckreg << 1, io.spi_sck)
  val sck_rise = sckreg(2) == Bits(0) && sckreg(1) == Bits(1)
  val sck_fall = sckreg(2) == Bits(1) && sckreg(1) == Bits(0)

  val ssreg = Reg(init = Bits("b000", 3))
  ssreg := Cat(ssreg << 1, io.spi_ss)
  val ss_active = ~ssreg(1)
  val ss_startmsg = ssreg(2) == Bits(1) && ssreg(1) == Bits(0)
  val ss_endmsg = ssreg(2) == Bits(0) && ssreg(1) == Bits(1)

  val mosireg = Reg(init = Bits("b00", 2))
  mosireg := Cat(mosireg << 1, io.spi_mosi)
  val mosi_data = mosireg(1)

  // TODO: might be DELAYED...


  // Receive
  val bitcnt = Reg(init = Bits(0, 3))
  val byte_received = Reg(Bool(), init = false.B)
  val byte_data_received = Reg(UInt(width = 8), init = 0.U)

  when (~ss_active) {
    bitcnt := 0.U
  } .otherwise {
    when (sck_rise.B) {
      bitcnt := bitcnt + 1.U

      // MSB first, left-shift reg
      byte_data_received := Cat(byte_data_received << 1, mosi_data)

    }
  }

  byte_received := ss_active && sck_rise.B && bitcnt === 7.U

  // when (byte_received) {
  //   io.led(1) := byte_data_received(0) // LSB to control LED
  // }

  // io.led4(0) := io.sw(0)
  // io.led5(1) := io.sw(0)
  // io.led4(1) := io.sw(1)
  // io.led5(2) := io.sw(1)

  // Status LEDs
  io.led0 := ~io.spi_ss // ss low, activity
  io.led1 := io.spi_mosi
  io.led2 := io.spi_miso
  io.led3 := io.spi_sck

  // SPI bit active
  // io.led(3) := io.btn(3)
}
