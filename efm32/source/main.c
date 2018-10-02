#include "setup.h"
#include "leds.h"
#include "lcd.h"
#include "interrupt.h"
#include "serial.h"
/* #include "bno055.h" */
#include <string.h>


#include "em_cmu.h"


#define NO_RX                    0
#define NO_TX                    NO_RX

#define USART0_NUM               0
#define USART1_NUM               1
#define USART2_NUM               2

#define GPIO_POS0                0
#define GPIO_POS1                1
#define GPIO_POS2                2


void SPI_setup(uint8_t spiNumber, uint8_t location, bool master);
void SPI2_setupRXInt(char* receiveBuffer, int bytesToReceive);
void SPI1_setupSlaveInt(char* receiveBuffer, int receiveBufferSize, char* transmitBuffer, int transmitBufferSize);


#define HFRCO_FREQUENCY 14000000
#define SPI_PERCLK_FREQUENCY HFRCO_FREQUENCY
#define SPI_BAUDRATE 1000000

char transmitBuffer[] = "EFM32 SPI";
#define            BUFFERSIZE    (sizeof(transmitBuffer) / sizeof(char))
char receiveBuffer[BUFFERSIZE];
char receiveBuffer2[BUFFERSIZE];


int main(void)
{
	// General chip initialization
	init();

	// Interrupts (works with LCD)
	/* interrupt_test(); */

	// Setup screen for debug
	SegmentLCD_Init(false);

	/* // UART */
	/* init_uart(); */

	/* // I2C and the BNO055 */
	/* init_i2c(); */
	/* init_bno055(); */

	/* // RTC */
	/* /\* rtcSetup(); *\/ */

	/* uint8_t status_buf[1] = { 7 }; */
	/* struct euler angles; */
	/* struct accel accelerations; */

	/* while (1) { */
	/* 	// Fetch system status */
	/* 	performI2CRead(BNO055_I2C_ADDRESS, BNO055_SYS_STAT_ADDR, status_buf, 1); */
	/* 	uartPutChar(status_buf[0]); */

	/* 	angles = get_euler_sample(); */
	/* 	accelerations = get_linear_acceleration_sample(); */

	/* 	char str[8]; */
	/* 	snprintf(str, 8, "%d", (int) angles.z); */
	/* 	SegmentLCD_Write(str); */

	/* 	Delay(50); */
	/* } */


  CMU_ClockEnable(cmuClock_USART1, true);
  /* CMU_ClockEnable(cmuClock_USART2, true); */

  SPI_setup(USART1_NUM, GPIO_POS0, true);


  USART1_sendBuffer(transmitBuffer, BUFFERSIZE);


	// LEDS, (disabled for now, as they collide with UART)
	/* leds_test(); */

	/* for (;;); */
}
