package rosetta

import Chisel._

// Accelerator
class UART() extends RosettaAccelerator {
  val numMemPorts = 0
  val io = new RosettaAcceleratorIF(numMemPorts) {
    // val uart_tx = Bits(OUTPUT, 1)
    // val uart_rx = Bits(INPUT, 1)
  }

  // LEDs just for fun
  io.led4(0) := io.sw(0)
  io.led5(1) := io.sw(0)
  io.led4(1) := io.sw(1)
  io.led5(2) := io.sw(1)

  io.signature := makeDefaultSignature()
  // io.led := io.btn

  // Sends message on uart receive (kind of wonky, depends on number
  // of 1s in signal)

  // Pass through on pins
   io.uart_tx := io.uart_rx
}


/*
 * Copyright: 2014-2018, Technical University of Denmark, DTU Compute
 * Author: Martin Schoeberl (martin@jopdesign.com)
 * License: Simplified BSD License
 * 
 * A UART is a serial port, also called an RS232 interface.
 * 
 */


/**
 * This is a minimal AXI style data plus handshake channel.
 */
class Channel extends Bundle {
  val data = Bits(INPUT, 8)
  val ready = Bool(OUTPUT) // addr, data, ctrl info is available
  val valid = Bool(INPUT)  // dest. asserts to indicate it can accpept info
                           // Transfer occurs when both ready and valid are high.
}

/**
 * Transmit part of the UART.
 * A minimal version without any additional buffering.
 * Use an AXI like valid/ready handshake.
 */
class Tx(frequency: Int, baudRate: Int) extends Module {
  val io = new Bundle {
    val txd = Bits(OUTPUT, 1)
    val channel = new Channel()
  }

  val BIT_CNT = UInt((frequency + baudRate / 2) / baudRate - 1)

  val shiftReg = Reg(init = Bits(0x7ff))
  val cntReg = Reg(init = UInt(0, 20))
  val bitsReg = Reg(init = UInt(0, 4))

  io.channel.ready := (cntReg === UInt(0)) && (bitsReg === UInt(0))
  io.txd := shiftReg(0)

  // TODO: make the counter a tick generator
  when(cntReg === UInt(0)) {

    cntReg := BIT_CNT
    when(bitsReg =/= UInt(0)) {
      val shift = shiftReg >> 1
      shiftReg := Cat(Bits(1), shift(9, 0))
      bitsReg := bitsReg - UInt(1)
    }.otherwise {
      when(io.channel.valid) {
        shiftReg(0) := Bits(0) // start bit
        shiftReg(8, 1) := io.channel.data // data
        shiftReg(10, 9) := Bits(3) // two stop bits
        bitsReg := UInt(11)
      }.otherwise {
        shiftReg := Bits(0x7ff)
      }
    }

  }.otherwise {
    cntReg := cntReg - UInt(1)
  }

  debug(shiftReg)
}

/**
 * Receive part of the UART.
 * A minimal version without any additional buffering.
 * Use an AXI like valid/ready handshake.
 *
 * The following code is inspired by Tommy's receive code at:
 * https://github.com/tommythorn/yarvi
 */
class Rx(frequency: Int, baudRate: Int) extends Module {
  val io = new Bundle {
    val rxd = Bits(INPUT, 1)
    val channel = new Channel().flip
  }

  val BIT_CNT = UInt((frequency + baudRate / 2) / baudRate - 1)
  val START_CNT = UInt((3 * frequency / 2 + baudRate / 2) / baudRate - 1)

  // Sync in the asynchronous RX data
  val rxReg = Reg(next = Reg(next = io.rxd))

  val shiftReg = Reg(init = Bits('A', 8))
  val cntReg = Reg(init = UInt(0, 20))
  val bitsReg = Reg(init = UInt(0, 4))
  val valReg = Reg(init = Bool(false))

  when(cntReg =/= UInt(0)) {
    cntReg := cntReg - UInt(1)
  }.elsewhen(bitsReg =/= UInt(0)) {
    cntReg := BIT_CNT
    shiftReg := Cat(rxReg, shiftReg >> 1)
    bitsReg := bitsReg - UInt(1)
    // the last shifted in
    when(bitsReg === UInt(1)) {
      valReg := Bool(true)
    }
  }.elsewhen(rxReg === UInt(0)) { // wait 1.5 bits after falling edge of start
    cntReg := START_CNT
    bitsReg := UInt(8)
  }

  when(io.channel.ready) {
    valReg := Bool(false)
  }

  io.channel.data := shiftReg
  io.channel.valid := valReg
}

/**
 * A single byte buffer with an AXI style channel
 */
class Buffer extends Module {
  val io = new Bundle {
    val in = new Channel()
    val out = new Channel().flip
  }

  val empty :: full :: Nil = Enum(UInt(), 2)
  val stateReg = Reg(init = empty)
  val dataReg = Reg(init = Bits(0, 8))

  io.in.ready := stateReg === empty
  io.out.valid := stateReg === full

  when(stateReg === empty) {
    when(io.in.valid) {
      dataReg := io.in.data
      stateReg := full
    }
  }.otherwise { // full, io.out.valid := true
    when(io.out.ready) {
      stateReg := empty
    }
  }
  io.out.data := dataReg
}

/**
 * A transmitter with a single buffer.
 */
class BufferedTx(frequency: Int, baudRate: Int) extends Module {
  val io = new Bundle {
    val txd = Bits(OUTPUT, 1)
    val channel = new Channel()
  }
  val tx = Module(new Tx(frequency, baudRate))
  val buf = Module(new Buffer())

  buf.io.in <> io.channel
  tx.io.channel <> buf.io.out
  io.txd <> tx.io.txd
}

/**
 * Send 'hello'.
 */
class Sender(frequency: Int, baudRate: Int) extends Module {
  val io = new Bundle {
    val txd = Bits(OUTPUT, 1)
    val reset = Bool(INPUT)
  }

  val tx = Module(new BufferedTx(frequency, baudRate))

  io.txd := tx.io.txd

  // This is not super elegant
  val hello = Array[Bits](Bits('H'), Bits('e'), Bits('l'), Bits('l'), Bits('o'))
  val text = Vec[Bits](hello)

  val cntReg = Reg(init = UInt(0, 3))


  when (io.reset) {
    cntReg := 0.U
    tx.io.channel.valid := false.B //
  }

  tx.io.channel.data := text(cntReg)
  tx.io.channel.valid := cntReg =/= UInt(5)

  when(tx.io.channel.ready && cntReg =/= UInt(5)) {
    cntReg := cntReg + UInt(1)
  }
}

class Echo(frequency: Int, baudRate: Int) extends Module {
  val io = new Bundle {
    val txd = Bits(OUTPUT, 1)
    val rxd = Bits(INPUT, 1)
  }
  // io.txd := Reg(next = io.rxd, init = UInt(0))
  val tx = Module(new BufferedTx(frequency, baudRate))
  val rx = Module(new Rx(frequency, baudRate))
  io.txd := tx.io.txd
  rx.io.rxd := io.rxd
  tx.io.channel <> rx.io.channel
  tx.io.channel.valid := rx.io.channel.valid
}
