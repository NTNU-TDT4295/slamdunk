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

    // Send reset
    uint8_t reset_packet[2];
    reset_packet[0] = 0xA5;
    reset_packet[1] = 0x40;
    uartPutData(1, reset_packet, 2);

    DelayMs(10);

    // Send stop
    uint8_t stop_packet[2];
    stop_packet[0] = 0xA5;
    stop_packet[1] = 0x25;
    uartPutData(1, stop_packet, 2);

    DelayMs(10);

    // Get status
    uint8_t status_packet[2];
    status_packet[0] = 0xA5;
    status_packet[1] = 0x52;
    uartPutData(1, status_packet, 2);

    // Get status descriptor
    char h_c;
    for(int i = 0; i < 10; i++){
        h_c = USART_Rx(UART1);
        USART_Tx(UART0, h_c);
    }

    DelayMs(10);

    // Init scan
    //uint8_t request_packet[2];
    //request_packet[0] = 0xA5;
    //request_packet[1] = 0x20;
    //uartPutData(1, request_packet, 2);

    // Get scan descriptor
    //uint8_t desc_response[7];
    //uartGetData(desc_response, 7);
    //char s_c;
    //for( int i = 0 ; i < 7 ; i++){
    //    s_c = USART_Rx(UART1);
    //    USART_Tx(UART0, s_c);
    //}

    int lidar_i = 0;
    uint8_t lidar_data[5];

    float radius;

	while (1) {
        //USART_Tx(UART0, cc);
        //lidar_data[lidar_i] = USART_Rx(UART1);

        //if (lidar_i == 4){
        //    //USART_Tx(UART0, lidar_data[0]);
        //    radius = (float) ((lidar_data[2] << 8) | (lidar_data[1] >> 1));
        //    radius = radius/64.0;
        //    USART_Tx(UART0, (uint8_t) radius);

        //    //USART_Tx(UART0, lidar_data[3]);
        //    //USART_Tx(UART0, lidar_data[4]);
        //    //USART_Tx(UART0, '\xff');
        //    //DelayMs(1000);
        //}
        //lidar_i++;
        //lidar_i = lidar_i % 5;


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
