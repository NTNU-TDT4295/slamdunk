#include "setup.h"
#include "leds.h"
#include "lcd.h"
#include "interrupt.h"
#include "serial.h"
#include "bno055.h"
#include "sonar.h"
#include "lidar.h"
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
	//rtcSetup();

	// Initalize GPIO interrupts for sonar
	//init_sonar();

	// Reset lidar completely to make it ready for scanning
	init_lidar(false);

	// Hold IMU data
	uint8_t status_buf[1] = { 7 };
	struct euler angles;
	struct quaternion quat;
	struct accel accelerations;

	/* int lidar_i = 0; */
	/* uint8_t lidar_data[5]; */
	/* float radius; */
	/* uint16_t radius_nonfloat; */
	/* uint16_t angle_q; */

	// Hold LIDAR data
	char recv;
	int lidar_i = 0;
	uint8_t lidar_data[2000];

	init_scan_lidar(false);
	while (1) {
		// 500 us per sample, 2000 samples per second
		lidar_data[lidar_i] = USART_Rx(UART1);

		if (lidar_i == 1999) {
			stop_lidar();
			for (int i = 0; i < 2000; ++i) {
				USART_Tx(UART0, lidar_data[i]);
			}
			init_scan_lidar(false);
		}

		lidar_i++;
		lidar_i = lidar_i % 2000;

		/*
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
		// trigger_sonar();

		// Testing of accelerometer data
		// accelerations = get_linear_acceleration_sample();
		// uartPutData((uint8_t *) &accelerations.x, 6);
		*/
	}

	// LEDS, (disabled for now, as they collide with UART)
	/* leds_test(); */

	/* for (;;); */
}
