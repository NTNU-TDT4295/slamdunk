//EFM32 blink test

#include "setup.h"
#include "leds.h"
#include "lcd.h"
#include "interrupt.h"
#include "serial.h"
#include <string.h>

// Buffers++
/* extern uint8_t i2c_txBuffer[6]; */
/* extern uint8_t i2c_txBufferSize; */
/* extern uint8_t i2c_rxBuffer[I2C_RXBUFFER_SIZE]; */
/* extern uint8_t i2c_rxBufferIndex; */


/* extern volatile bool i2c_rxInProgress; */
/* extern volatile bool i2c_startTx; */


int main(void)
{
    init();

    // Interrupts (works with LCD)
    /* interrupt_test(); */

    // UART
    init_uart();


	while (1) {
		if (i2c_rxInProgress) {
			/* Receiving data */
            receiveI2CData();
		}/*  else if (i2c_startTx) { */
		/* 	/\* Transmitting data *\/ */
		/* 	performI2CTransfer(); */
		/* 	/\* Transmission complete *\/ */
		/* 	i2c_startTx = false; */
		/* } */

		/* Forever enter EM2. The RTC or I2C will wake up the EFM32 */
		/* EMU_EnterEM2(false); */
	}
    
	// LEDS, (disabled for now, as they collide with UART)
    /* leds_test(); */

    for (;;);
}
