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

	// Setup screen for debug
	SegmentLCD_Init(false);

	// UART
	init_uart();

	// I2C
	init_i2c();

	// Setup the BNO055
	performI2CTransfer();

	uint8_t buf[1] = { 7 };
	uint8_t euler_buf[6];

	while (1) {
		// Fetch system status
		performI2CRead(BNO055_SYS_STAT_ADDR, buf, 1);
		uartPutChar(buf[0]);

		int16_t x, y, z;
		double xa, ya, za;

		performI2CRead(BNO055_EULER_H_LSB_ADDR, euler_buf, 6);
		x = ((int16_t) euler_buf[0]) | (((int16_t) euler_buf[1]) << 8);
		y = ((int16_t) euler_buf[2]) | (((int16_t) euler_buf[3]) << 8);
		z = ((int16_t) euler_buf[4]) | (((int16_t) euler_buf[5]) << 8);

		// Euler angles
		xa = ((double) x) / 16.0;
		ya = ((double) y) / 16.0;
		za = ((double) z) / 16.0;

		char str[8];
		snprintf(str, 8, "%d", x);
		SegmentLCD_Write(str);

		Delay(10);
	}

	// LEDS, (disabled for now, as they collide with UART)
	/* leds_test(); */

	for (;;);
}
