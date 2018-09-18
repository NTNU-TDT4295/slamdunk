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

	// RTC
	/* rtcSetup(); */

	// Normal power mode
	uint8_t power_mode[] = "\x3E\x00";
	performI2CTransfer(power_mode, 2);

	Delay(50);

	// Page
	uint8_t page[] = "\x07\x00";
	performI2CTransfer(page, 2);

	// Perform self test
	uint8_t self_test[] = "\x3F\x00";
	performI2CTransfer(self_test, 2);

	Delay(50);

	uint8_t mode[] = "\x3D\x0C";
	performI2CTransfer(mode, 2);

	Delay(50);

	uint8_t buf[1] = { 7 };
	uint8_t euler_buf[6];
	uint8_t quat_buf[8];
	memset(euler_buf, 0, 6);
	memset(quat_buf, 0, 8);

	while (1) {
		// Fetch system status
		performI2CRead(BNO055_SYS_STAT_ADDR, buf, 1);
		uartPutChar(buf[0]);

		int16_t x, y, z, w;
		double xa, ya, za, zw;

		performI2CRead(BNO055_EULER_H_LSB_ADDR, euler_buf, 6);
		x = ((int16_t) euler_buf[0]) | (((int16_t) euler_buf[1]) << 8);
		y = ((int16_t) euler_buf[2]) | (((int16_t) euler_buf[3]) << 8);
		z = ((int16_t) euler_buf[4]) | (((int16_t) euler_buf[5]) << 8);

		/* // Euler angles */
		/* xa = ((double) x) / 16.0; */
		/* /\* ya = ((double) y) / 16.0; *\/ */
		/* /\* za = ((double) z) / 16.0; *\/ */

		/* performI2CRead(BNO055_QUATERNION_DATA_W_LSB_ADDR, quat_buf, 8); */
		/* x = ((int16_t) quat_buf[0]) | (((int16_t) quat_buf[1]) << 8); */
		/* y = ((int16_t) quat_buf[2]) | (((int16_t) quat_buf[3]) << 8); */
		/* z = ((int16_t) quat_buf[4]) | (((int16_t) quat_buf[5]) << 8); */
		/* w = ((int16_t) quat_buf[6]) | (((int16_t) quat_buf[7]) << 8); */

		char str[8];
		snprintf(str, 8, "%d", z);
		SegmentLCD_Write(str);

		Delay(50);
	}

	// LEDS, (disabled for now, as they collide with UART)
	/* leds_test(); */

	for (;;);
}
