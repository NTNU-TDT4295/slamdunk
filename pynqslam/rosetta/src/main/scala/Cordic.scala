package Core
import chisel3._
import chisel3.core.Input
import chisel3.iotesters.PeekPokeTester
import chisel3.util.Counter
// import utilz._


// All numbers are 32bit signed fixed point integers, with radix between 2nd and 3rd most significant bit
class Cordic() extends Module {

  val io = IO(
    new Bundle {

      val start    = Input(Bool())
      val angle_in = Input(SInt(32.W))

      val cos_out  = Output(SInt(32.W))
      val sin_out  = Output(SInt(32.W))
    
    })

  val cos     = RegInit(SInt(32.W), 0.S)
  val sin     = RegInit(SInt(32.W), 0.S)
  val angle   = RegInit(SInt(32.W), 0.S)
  val count   = Counter(32)
  val state   = Reg(Bool())

  val cos_shr            = Wire(SInt())
  val sin_shr            = Wire(SInt())
  val direction_negative = Wire(Bool())
  val beta               = Wire(SInt())

  val K           = 652032874.S(32.W)
  val beta_lut    = VecInit(Array(
  843314857.S(32.W), 497837829.S(32.W), 263043837.S(32.W), 133525159.S(32.W),
  67021687.S(32.W),  33543516.S(32.W),  16775851.S(32.W),  8388437.S(32.W),
  4194283.S(32.W),   2097149.S(32.W),   1048576.S(32.W),   524288.S(32.W),
  262144.S(32.W),    131072.S(32.W),    65536.S(32.W),     32768.S(32.W),
  16384.S(32.W),     8192.S(32.W),      4096.S(32.W),      2048.S(32.W),
  1024.S(32.W),      512.S(32.W),       256.S(32.W),       128.S(32.W),
  64.S(32.W),        32.S(32.W),        16.S(32.W),        8.S(32.W),
  4.S(32.W),         2.S(32.W),         1.S(32.W),         0.S(32.W)
))


    io.cos_out := cos
    io.sin_out := sin

    when (state) {

      when(direction_negative) {
        cos := cos + sin_shr
        sin := sin - cos_shr
        angle := angle + beta
      }.otherwise {
        cos := cos - sin_shr
        sin := sin + cos_shr
        angle := angle - beta
      }

      when (count.inc()) {
        state := false.B
      }

    }.otherwise {

      when(io.start) {
        cos := K
        sin := 0.S
        angle := io.angle_in
        state := true.B
      }
  }

  // Shift right using arithmetic shift
  cos_shr            := cos >> count.value
  sin_shr            := sin >> count.value
  // Fetch signbit from angle
  direction_negative := angle(31)
  printf("%d\n",direction_negative)
  beta               := beta_lut(count.value)

}
