package Core
import chisel3._
import chisel3.iotesters._
import org.scalatest.{Matchers, FlatSpec}
import testUtils._


class cordicSpec extends FlatSpec with Matchers {

  behavior of "cordic"

  it should "fail, i just want to see the output" in {

    val ins = (0 to 37).map(ii =>
      CycleTask[Cordic](
        ii,
        d => d.poke(d.dut.io.start, 1),
        // Angle is 32bit signed fixed point integer, with radix between 2nd and 3rd most significant bit
        d => d.poke(d.dut.io.angle_in, 652032874),
        d => d.expect(d.dut.io.cos_out, 4),
        d => d.expect(d.dut.io.sin_out, 4),
        d => d.peek(d.dut.io.start))
    ).toList


    iotesters.Driver.execute(() => new Cordic(), new TesterOptionsManager) { c =>
      IoSpec[Cordic](ins, c).myTester
    } should be(true)
  }
}
