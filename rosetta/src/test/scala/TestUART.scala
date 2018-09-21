import Chisel._
import rosetta._
import org.scalatest.junit.JUnitSuite
import org.junit.Test
import RosettaTest._

class TestUARTSuite extends JUnitSuite {
  @Test def AddTest {

    // Tester-derived class to give stimulus and observe the outputs for the
    // Module to be tested
    class AddTest(c: BufferedTx) extends Tester(c) {
      // use peek() to read I/O output signal values

      step(2)
      poke(c.io.channel.valid, 1)
      poke(c.io.channel.data, 'A')
      // now we have a buffer, keep valid only a single cycle
      step(1)
      poke(c.io.channel.valid, 0)
      poke(c.io.channel.data, 0)
      step(40)
      poke(c.io.channel.valid, 1)
      poke(c.io.channel.data, 'B')
      step(1)
      poke(c.io.channel.valid, 0)
      poke(c.io.channel.data, 0)
      step(30)
    }

    // Chisel arguments to pass to chiselMainTest
    def testArgs = RosettaTest.stdArgs
    // function that instantiates the Module to be tested
    def testModuleInstFxn = () => { Module(new BufferedTx(50000000, 9600)) }
    // function that instantiates the Tester to test the Module
    def testTesterInstFxn = c => new AddTest(c)

    // actually run the test
    chiselMainTest(
      testArgs,
      testModuleInstFxn
    ) {
      testTesterInstFxn
    }
  }
}
