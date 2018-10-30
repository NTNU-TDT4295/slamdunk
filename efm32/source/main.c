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

extern bool enable_lidar;

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
	/* GPIO_PinModeSet(gpioPortF, 8, gpioModePushPull, 1); //motoctl */


	// I2C and the BNO055
	init_i2c();
	init_bno055();

	// SPI
	init_SPI();

	// GPIO interrupts
	init_GPIO();

	// RTC
	//rtcSetup();

	// Initalize GPIO interrupts for sonar
	//init_sonar();

	// Reset lidar completely to make it ready for scanning
	unsigned int uart_channel = 0; // uart input channel

	// Hold IMU data
	uint8_t status_buf[1] = {7};
	struct euler angles;
	struct quaternion quat;
	struct accel accelerations;

	// Hold LIDAR data
	size_t lidar_samples = 360;
	uint8_t lidar_data[lidar_samples * 5];

	char lidar_sample;
	/* init_lidar(false, 0); */

	while (1) {
		// 500 us per sample, 2000 samples per second, LIDAR
		// SPI
		/* get_samples_lidar(lidar_data, lidar_samples); */
		/* put_uart_simple(0, lidar_data, lidar_samples*5); */
		/* SPI_sendBuffer(lidar_data, lidar_samples*5); */
		/* put_uart_simple(1, buffer, 255); */

		/* while (!enable_lidar) */
		/* 	; // Busy wait, toggled by Top left button */

		/* lidar_sample = USART_Rx(UART0); */
		// /* put_uart_simple(1, lidar_sample, 1); */
		/* SPI_sendBuffer(&lidar_sample, 1); */

		// TODO:
		// toggle lidar
		// sync sequence

		/* continue; */

		/* // IMU */
		/* quat = get_quaternion_sample(); */
		/* for (size_t i = 0; i < 8; ++i) { */
		/* 	uint8_t quat_data = *(((uint8_t *) &quat.w) + i); */
		/* 	USART_Tx(UART0, quat_data); */
		/* } */

		// Fetch system status
		performI2CRead(BNO055_I2C_ADDRESS, BNO055_SYS_STAT_ADDR, status_buf, 1);
		put_uart_simple(1, status_buf, 1);

		DelayMs(100);

		// Fetch samples for sensors available
		angles = get_euler_sample();
		quat = get_quaternion_sample();
		accelerations = get_linear_acceleration_sample();

		// Send quaternion data
		/* put_uart_simple(1, (uint8_t *) &quat.w, 8); */
		uartPutData((uint8_t *) &quat.w, 8);

		// Trigger the sonar, it will interrupt you
		// trigger_sonar();

		// Testing of accelerometer data
		/* accelerations = get_linear_acceleration_sample(); */
		/* uartPutData((uint8_t *) &accelerations.x, 6); */
	}

	// LEDS, (disabled for now, as they collide with UART)
	/* leds_test(); */
}
