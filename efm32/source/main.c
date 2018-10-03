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

    /*
	// I2C and the BNO055
	init_i2c();
	init_bno055();

	// RTC
	//rtcSetup();

	// Initalize GPIO interrupts for sonar
	init_sonar();

	uint8_t status_buf[1] = { 7 };
	struct euler angles;
	struct quaternion quat;
	struct accel accelerations;
    */
	char recv;

	// Reset Lidar
	uint8_t reset_packet[2];
	reset_packet[0] = 0xA5;
	reset_packet[1] = 0x40;
	uartPutData(1, reset_packet, 2);

	// Throw away all the garbage mumbo that comes from the lidar
	// on reset
	for (int i = 0; i < 64; i++){
		recv = USART_Rx(UART1);
	}

	// Query health of Lidar
	uint8_t health_packet[2];
	health_packet[0] = 0xA5;
	health_packet[1] = 0x52;
	uartPutData(1, health_packet, 2);
	for (int i = 0; i < 10; i++){
		recv = USART_Rx(UART1);
		USART_Tx(UART0, recv);
	}

	// Init scan
	DelayMs(10);
	uint8_t scan_packet[2];
	scan_packet[0] = 0xA5;
	scan_packet[1] = 0x20;
	uartPutData(1, scan_packet, 2);

	// Get scan descriptor
	for (int i = 0 ; i < 7 ; i++){
		recv = USART_Rx(UART1);
		USART_Tx(UART0, recv);
	}

	int lidar_i = 0;
	uint8_t lidar_data[5];
	float radius;
	uint16_t radius_nonfloat;
	uint16_t angle_q;

	while (1) {
		lidar_data[lidar_i] = USART_Rx(UART1);

		if (lidar_i == 4) {
			angle_q = ((lidar_data[2] << 8) | (lidar_data[1]));
			radius = (float) (angle_q >> 1);
			radius = radius / 64.0;
			radius_nonfloat = (uint16_t) radius;
			USART_Tx(UART0, (uint8_t) radius_nonfloat);
			USART_Tx(UART0, (uint8_t) (radius_nonfloat >> 8));
		}

		lidar_i++;
		lidar_i = lidar_i % 5;


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
