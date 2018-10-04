#include "setup.h"
#include "leds.h"
#include "lcd.h"
#include "interrupt.h"
#include "serial.h"
/* #include "bno055.h" */
#include <string.h>

/* #define DEBUG 1 */

char transmitBuffer[] = "KIS";
#define BUFFERSIZE (sizeof(transmitBuffer) / sizeof(char))
char receiveBuffer[BUFFERSIZE];

#define NO_RX 0
#define NO_TX NO_RX

/* #define MASTER */

void USART1_sendBuffer(char*, int);
void SPI1_setupRXIntMaster(char* receiveBuffer, int bytesToReceive);
void SPI1_setupRXIntSlave(char* receiveBuffer, int bytesToReceive);
void SPI_setup();

int main(void)
{
	// General chip initialization
	init();

	// Interrupts (works with LCD)
	/* interrupt_test(); */

	CMU_ClockEnable(cmuClock_USART1, true);


	// Setup screen for debug
	SegmentLCD_Init(false);
	SegmentLCD_Write("SPI <3");
	Delay(500);


#ifdef MASTER
	SPI_setup();
	SegmentLCD_Write("MASTER");
	SPI1_setupRXIntMaster(NO_RX, NO_RX); // TODO: omit?
	Delay(500);
#else
	SPI_setup_slave_rec();
	SPI1_setupSlaveInt(receiveBuffer, BUFFERSIZE, NO_TX, NO_TX);
	SegmentLCD_Write("SLAVE");
	Delay(500);
#endif


	while (1) {
#ifdef MASTER
		USART1_sendBuffer(transmitBuffer, BUFFERSIZE);
#else
		SegmentLCD_Write(receiveBuffer);
		Delay(200);
#endif
	}
}
