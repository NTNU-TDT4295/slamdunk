#include "setup.h"
#include "leds.h"
#include "lcd.h"
#include "interrupt.h"
#include "serial.h"
#include "bno055.h"
#include "sonar.h"
#include <string.h>

// SPI
char transmitBuffer[] = "abcdefgh";
#define BUFFERSIZE (sizeof(transmitBuffer) / sizeof(char))

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

	// SPI
	SPI_setup();

	// RTC
	/* rtcSetup(); */

	// Initalize GPIO interrupts for sonar
	init_sonar();

	uint8_t status_buf[1] = { 7 };
	struct euler angles;
	struct quaternion quat;
	struct accel accelerations;

	while (1) {
		// Fetch system status
		performI2CRead(BNO055_I2C_ADDRESS, BNO055_SYS_STAT_ADDR, status_buf, 1);
		uartPutChar(0, status_buf[0]);

		// Fetch samples for sensors available
		angles = get_euler_sample();
		quat = get_quaternion_sample();
		accelerations = get_linear_acceleration_sample();

		// Send quaternion data
		uartPutData((uint8_t *) &quat.w, 8);

		// Trigger the sonar, it will interrupt you
		/* trigger_sonar(); */

		// Testing of accelerometer data
		/* accelerations = get_linear_acceleration_sample(); */
		/* uartPutData((uint8_t *) &accelerations.x, 6); */

		// SPI
		SPI_sendBuffer(transmitBuffer, BUFFERSIZE);
	}

	// LEDS, (disabled for now, as they collide with UART)
	/* leds_test(); */

	/* for (;;); */
}
