//EFM32 blink test

#include "setup.h"
#include "leds.h"
#include "lcd.h"
#include "interrupt.h"
#include "serial.h"
#include <string.h>

// Buffers++
/* extern uint8_t i2c_txBuffer[6]; */
/* extern uint8_t i2c_txBufferSize; */
/* extern uint8_t i2c_rxBuffer[I2C_RXBUFFER_SIZE]; */
/* extern uint8_t i2c_rxBufferIndex; */


/* extern volatile bool i2c_rxInProgress; */
/* extern volatile bool i2c_startTx; */


int main(void)
{
	init();

	// Interrupts (works with LCD)
	/* interrupt_test(); */

	// UART
	init_uart();

	// I2C
	init_i2c();

	// Setup the BNO055
	performI2CTransfer();

	uint8_t buf[1] = { 7 };

	while (1) {
		// Fetch system status
		performI2CRead(BNO055_SYS_STAT_ADDR, buf, 1);
		uartPutChar('a');
		uartPutChar(buf[0]);
		uartPutChar('b');
		Delay(1000);
	}

	// LEDS, (disabled for now, as they collide with UART)
	/* leds_test(); */

	for (;;);
}
