#include "setup.h"
#include "leds.h"
#include "lcd.h"
#include "interrupt.h"
#include "serial.h"
#include "bno055.h"
#include <string.h>

int main(void)
{
	// General chip initialization
	init();

	// Interrupts (works with LCD)
	/* interrupt_test(); */

	// Setup screen for debug
	SegmentLCD_Init(false);

	// UART
	init_uart();

	// I2C and the BNO055
	init_i2c();
	init_bno055();

	// RTC
	/* rtcSetup(); */

	uint8_t buf[1] = { 7 };
	uint8_t euler_buf[6];
	uint8_t quat_buf[8];
	memset(euler_buf, 0, 6);
	memset(quat_buf, 0, 8);

	while (1) {
		// Fetch system status
		performI2CRead(BNO055_I2C_ADDRESS, BNO055_SYS_STAT_ADDR, buf, 1);
		uartPutChar(buf[0]);

		int16_t x, y, z, w;
		double xa, ya, za, zw;

		performI2CRead(BNO055_I2C_ADDRESS, BNO055_EULER_H_LSB_ADDR, euler_buf, 6);
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

	/* for (;;); */
}
