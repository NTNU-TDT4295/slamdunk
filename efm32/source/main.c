#include "setup.h"
#include "leds.h"
#include "lcd.h"
#include "interrupt.h"
#include "serial.h"
/* #include "bno055.h" */
#include <string.h>

/* #define DEBUG 1 */

char transmitBuffer[] = "SPI";
#define BUFFERSIZE (sizeof(transmitBuffer) / sizeof(char))
char receiveBuffer[BUFFERSIZE];

#define NO_RX 0
#define NO_TX NO_RX

#define MASTER

void USART2_sendBuffer(char*, int);
void SPI2_setupRXInt(char* receiveBuffer, int bytesToReceive);
void SPI_setup();

int main(void)
{
	// General chip initialization
	init();

	// Interrupts (works with LCD)
	/* interrupt_test(); */

	CMU_ClockEnable(cmuClock_USART2, true);


	// Setup screen for debug
	SegmentLCD_Init(false);
	SegmentLCD_Write("SPI <3");
	Delay(1000);



#ifdef MASTER
	SPI_setup();
	SegmentLCD_Write("MASTER");
	Delay(1000);
#else
	SPI_setup_slave_rec();
	SPI1_setupSlaveInt(receiveBuffer, BUFFERSIZE, NO_TX, NO_TX);
	SegmentLCD_Write("SLAVE");
	Delay(1000);
#endif


	SPI2_setupRXInt(NO_RX, NO_RX); // TODO: omit?

	while (1) {
#ifdef MASTER
		USART2_sendBuffer(transmitBuffer, BUFFERSIZE);
#else
		SegmentLCD_Write(receiveBuffer);
#endif
	}


	// SPI

	/* mySPI_Thread(NULL); */
}
