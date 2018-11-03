#include "setup.h"
#include "gpio.h"
#include "serial.h"
#include "imu.h"
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

	// UART
	init_uart();

	// I2C and the BNO055
	init_i2c();
	init_imu();

	// SPI
	init_SPI();

	// GPIO interrupts
	init_GPIO();

	// Initalize GPIO interrupts for sonar
	//init_sonar();

	// Hold IMU data
	/* uint8_t status_buf[1] = {7}; */
	/* struct euler angles; */
	/* struct quaternion quat; */
	/* struct accel accelerations; */

	// Hold LIDAR data
	/* size_t lidar_samples = 360; */
	/* uint8_t lidar_data[lidar_samples * 5]; */
	char lidar_sample;
	/* init_lidar(false, 0); */

	while (1) {
		while (!enable_lidar)
			; // Busy wait, toggled by Top left button

		lidar_sample = USART_Rx(UART0);
		// /* put_uart_simple(1, lidar_sample, 1); */
		SPI_sendBuffer(&lidar_sample, 1);

		// TODO:
		// sync sequence

		/* // IMU */
		/* quat = get_quaternion_sample(); */
		/* for (size_t i = 0; i < 8; ++i) { */
		/* 	uint8_t quat_data = *(((uint8_t *) &quat.w) + i); */
		/* 	USART_Tx(UART0, quat_data); */
		/* } */

		// Fetch system status
		/* performI2CRead(BNO055_I2C_ADDRESS, BNO055_SYS_STAT_ADDR, status_buf, 1); */
		/* put_uart_simple(1, status_buf, 1); */

		/* DelayMs(100); */

		// Fetch samples for sensors available
		/* angles = get_euler_sample(); */
		/* quat = get_quaternion_sample(); */
		/* accelerations = get_linear_acceleration_sample(); */

		// Send quaternion data
		/* put_uart_simple(1, (uint8_t *) &quat.w, 8); */
	}
}
