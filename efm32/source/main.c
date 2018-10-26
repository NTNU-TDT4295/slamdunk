#include "setup.h"
#include "leds.h"
#include "lcd.h"
#include "interrupt.h"
#include "serial.h"
#include "bno055.h"
#include "sonar.h"
#include "lidar.h"
#include <string.h>

// SPI
char transmitBuffer[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
#define BUFFERSIZE (sizeof(transmitBuffer) / sizeof(char))

int main(void)
{
	// General chip initialization
	init();

	// Interrupts (works with LCD)
	/* interrupt_test(); */

	// Setup screen for debug
	/* SegmentLCD_Init(false); */

	// UART
	init_uart();

	// I2C and the BNO055
	/* init_i2c(); */
	/* init_bno055(); */

	// SPI
	SPI_init();

	// RTC
	//rtcSetup();

	// Initalize GPIO interrupts for sonar
	//init_sonar();

	// Reset lidar completely to make it ready for scanning
    unsigned int uart_channel = 0; // uart input channel
	init_lidar(false, 0);

	// Hold IMU data
	uint8_t status_buf[1] = { 7 };
	struct euler angles;
	struct quaternion quat;
	struct accel accelerations;

	// Hold LIDAR data
	size_t lidar_samples = 360;
	uint8_t lidar_data[lidar_samples*5];

	char lidar_sample;
	init_scan_lidar(false, 0);

	while (1) {
		// 500 us per sample, 2000 samples per second, LIDAR
// SPI
		/* get_samples_lidar(lidar_data, lidar_samples); */
		/* put_uart_simple(0, lidar_data, lidar_samples*5); */
		/* SPI_sendBuffer(lidar_data, lidar_samples*5); */

		lidar_sample = USART_Rx(UART0);
        SPI_sendBuffer(&lidar_sample, 1);

		continue;

		/* // IMU */
		/* quat = get_quaternion_sample(); */
		/* for (size_t i = 0; i < 8; ++i) { */
		/* 	uint8_t quat_data = *(((uint8_t *) &quat.w) + i); */
		/* 	USART_Tx(UART0, quat_data); */
		/* } */

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
		*/

		// Trigger the sonar, it will interrupt you
		// trigger_sonar();

		// Testing of accelerometer data
		/* accelerations = get_linear_acceleration_sample(); */
		/* uartPutData((uint8_t *) &accelerations.x, 6); */

		// SPI
		SPI_sendBuffer(transmitBuffer, BUFFERSIZE);
		
		/* SPI_sendBuffer(lidar_data, lidar_samples*5); */
		Delay(25);
	}

	// LEDS, (disabled for now, as they collide with UART)
	/* leds_test(); */
}
