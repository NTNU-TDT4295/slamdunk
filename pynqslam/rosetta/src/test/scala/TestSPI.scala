import Chisel._
import rosetta._
import org.scalatest.junit.JUnitSuite
import org.junit.Test
import RosettaTest._

class TestSPISuite extends JUnitSuite {
  @Test def AddTest {

    // Tester-derived class to give stimulus and observe the outputs for the
    // Module to be tested
    class AddTest(c: SPI_Slave) extends Tester(c) {
      // use peek() to read I/O output signal values
      // peek(c.io.signature)
      // // use poke() to set I/O input signal values
      // poke(c.io.op(0), 10)
      // poke(c.io.op(1), 20)
      // // use step() to advance the clock cycle
      // step(1)
      // // use expect() to read and check I/O output signal values
      // expect(c.io.sum, 10+20)

      // setup
      poke(c.io.spi_mosi, 0)
      poke(c.io.spi_sck, 0)
      poke(c.io.spi_ss, 1)
      step(1)


      printf("Reset complete\n--------------------\n\n\n")
      // val sck = List(0,1,0,1,0,1,0)
      val mosi = List(
        0,1,0,0,0,0,0,1,
        0,1,0,0,0,0,1,0,
        0,1,0,0,0,0,1,1,
        0,1,0,0,0,1,0,0,
        0,1,0,0,0,1,0,1,
        0,1,0,0,0,1,1,0,
        0,1,0,0,0,1,1,1,
        0,1,0,0,1,0,0,0,
        0,1,0,0,1,0,0,1,
        0,1,0,0,1,0,1,0,
        0,1,0,0,1,0,1,1,
        0,1,0,0,1,1,0,0,
        0,1,0,0,1,1,0,1,
                0,1,0,0,0,0,0,1,
        0,1,0,0,0,0,1,0,
        0,1,0,0,0,0,1,1,
        0,1,0,0,0,1,0,0,
        0,1,0,0,0,1,0,1,
        0,1,0,0,0,1,1,0,
        0,1,0,0,0,1,1,1,
        0,1,0,0,1,0,0,0,
        0,1,0,0,1,0,0,1,
        0,1,0,0,1,0,1,0,
        0,1,0,0,1,0,1,1,
        0,1,0,0,1,1,0,0,
        0,1,0,0,1,1,0,1,
                0,1,0,0,0,0,0,1,
        0,1,0,0,0,0,1,0,
        0,1,0,0,0,0,1,1,
        0,1,0,0,0,1,0,0,
        0,1,0,0,0,1,0,1,
        0,1,0,0,0,1,1,0,
        0,1,0,0,0,1,1,1,
        0,1,0,0,1,0,0,0,
        0,1,0,0,1,0,0,1,
        0,1,0,0,1,0,1,0,
        0,1,0,0,1,0,1,1,
        0,1,0,0,1,1,0,0,
        0,1,0,0,1,1,0,1,
                0,1,0,0,0,0,0,1,
        0,1,0,0,0,0,1,0,
        0,1,0,0,0,0,1,1,
        0,1,0,0,0,1,0,0,
        0,1,0,0,0,1,0,1,
        0,1,0,0,0,1,1,0,
        0,1,0,0,0,1,1,1,
        0,1,0,0,1,0,0,0,
        0,1,0,0,1,0,0,1,
        0,1,0,0,1,0,1,0,
        0,1,0,0,1,0,1,1,
        0,1,0,0,1,1,0,0,
        0,1,0,0,1,1,0,1
      ) // A->M

      poke(c.io.spi_ss, 0) // select
      for (i <- 0 until mosi.size) {

        poke(c.io.spi_mosi, mosi(i))
        poke(c.io.spi_sck, 1)
        step(1)

        poke(c.io.spi_sck, 0)
        step(1)

        when (c.word_received) {
          println("==================================================================")
        }
        // peek(c.byte_counter)
        // peek(c.word_data_received)
        // peek(c.write_address)
peek(c.word_received)
        // peek(c.word_received)
        // peek(c.byte_signal_received)
        // peek(c.word_received)
        // peek(c.bitcnt)
        // peek(c.byte_counter)
        // peek(c.write_adress)
        // peek(c.word_data_received)
      }

      step(1)
      poke(c.io.spi_ss, 0)
      step(1)
      peek(c.word_data_received)
        // peek(c.word_received)
    }

    // Chisel arguments to pass to chiselMainTest
    def testArgs = RosettaTest.stdArgs
    // function that instantiates the Module to be tested
    def testModuleInstFxn = () => { Module(new SPI_Slave()) }
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
