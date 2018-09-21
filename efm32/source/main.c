#include "setup.h"
#include "leds.h"
#include "lcd.h"
#include "interrupt.h"
#include "serial.h"
#include "bno055.h"
#include "sonar.h"
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

	// Initalize GPIO interrupts for sonar
	init_sonar();

	uint8_t status_buf[1] = { 7 };
	struct euler angles;
	struct accel accelerations;

	while (1) {
		// Fetch system status
		performI2CRead(BNO055_I2C_ADDRESS, BNO055_SYS_STAT_ADDR, status_buf, 1);
		uartPutChar(0, status_buf[0]);
		uartPutChar(1, status_buf[0]);

		angles = get_euler_sample();
		accelerations = get_linear_acceleration_sample();

		char str[8];
		snprintf(str, 8, "%d", (int) angles.z);
		/* SegmentLCD_Write(str); */

		// Trigger the sonar, it will interrupt you
		trigger_sonar();

		DelayMs(50);
	}

	// LEDS, (disabled for now, as they collide with UART)
	/* leds_test(); */

	/* for (;;); */
}
