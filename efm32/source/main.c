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

	uint8_t status_buf[1] = { 7 };
	struct euler angles;

	while (1) {
		// Fetch system status
		performI2CRead(BNO055_I2C_ADDRESS, BNO055_SYS_STAT_ADDR, status_buf, 1);
		uartPutChar(status_buf[0]);

		angles = get_euler_sample();

		char str[8];
		snprintf(str, 8, "%d", (int) angles.z);
		SegmentLCD_Write(str);

		Delay(50);
	}

	// LEDS, (disabled for now, as they collide with UART)
	/* leds_test(); */

	/* for (;;); */
}
