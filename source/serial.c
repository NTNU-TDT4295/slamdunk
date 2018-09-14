#include <stdio.h>

#include "serial.h"

void recv_char_cb(char c)
{
	static char str[LCD_BUFFER_SIZE];
	static int idx = 0;

	switch (c) {
	case BACKSPACE:
		if (idx != 0) {
			str[--idx] = 0;
		}
		break;
	case ENTER:
		for (size_t i = 0; i < LCD_BUFFER_SIZE; ++i) {
			str[i] = 0;
			idx = 0;
		}
		break;
    default:
        uartPutChar(c);
        break;
	}

	if ('a' <= c && c <= 'z') {
		str[idx] = c - 32;
		idx = idx == SEGMENT_LCD_NUM_BLOCK_COLUMNS
			? SEGMENT_LCD_NUM_BLOCK_COLUMNS
			: idx + 1;
	}

	SegmentLCD_Write(str);
}

void init_uart(void)
{
	/* LCD for proof of concept, remove later */
	SegmentLCD_Init(false);

    /* Setup UART for testing */
    set_recv_callback(&recv_char_cb);
    setup_uart();
}

void echo_uart()
{
    char rx_data;
    /* Echo RX to TX indefinitely */
    for (;;) {
        rx_data = uartGetChar();

        if (rx_data) {
            uartPutChar(rx_data);
        }
    }
}




////////////////
uint8_t i2c_txBuffer[] = "Gecko";
uint8_t i2c_txBufferSize = sizeof(i2c_txBuffer);
uint8_t i2c_rxBuffer[I2C_RXBUFFER_SIZE];
uint8_t i2c_rxBufferIndex;

volatile bool i2c_rxInProgress;
volatile bool i2c_startTx;


void init_i2c(void)
{

    CMU_ClockEnable(cmuClock_I2C0, true);

	// Using default settings
	I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;

	/* Using PD6 (SDA) and PD7 (SCL) */
	GPIO_PinModeSet(gpioPortD, 7, gpioModeWiredAndPullUpFilter, 1);
	GPIO_PinModeSet(gpioPortD, 6, gpioModeWiredAndPullUpFilter, 1);

	// Setting up PC0 to indicate transfer direction
	GPIO_PinModeSet(gpioPortC, 0, gpioModePushPull, 0);

	/* Enable pins at location 1 */
	I2C0->ROUTE = I2C_ROUTE_SDAPEN | I2C_ROUTE_SCLPEN | (1 << _I2C_ROUTE_LOCATION_SHIFT);

	/* Initializing the I2C */
	I2C_Init(I2C0, &i2cInit);

	/* Setting the status flags and index */
	i2c_rxInProgress = false;
	i2c_startTx = false;
	i2c_rxBufferIndex = 0;

	/* Setting up to enable slave mode */
	I2C0->SADDR = I2C_ADDRESS;
	I2C0->CTRL |= I2C_CTRL_SLAVE | I2C_CTRL_AUTOACK | I2C_CTRL_AUTOSN;
	enableI2cSlaveInterrupts();


        //

}

void enableI2cSlaveInterrupts(void)
{
	I2C_IntClear(I2C0, I2C_IEN_ADDR | I2C_IEN_RXDATAV | I2C_IEN_SSTOP);
	I2C_IntEnable(I2C0, I2C_IEN_ADDR | I2C_IEN_RXDATAV | I2C_IEN_SSTOP);
	NVIC_EnableIRQ(I2C0_IRQn);
}


/**************************************************************************/ /**
 * @brief  Transmitting I2C data. Will busy-wait until the transfer is complete.
 *****************************************************************************/
void performI2CTransfer(void)
{
	/* Transfer structure */
	I2C_TransferSeq_TypeDef i2cTransfer;

	/* Setting pin to indicate transfer */
	GPIO_PinOutSet(gpioPortC, 0);

	/* Initializing I2C transfer */
	i2cTransfer.addr = I2C_ADDRESS;
	i2cTransfer.flags = I2C_FLAG_WRITE;
	i2cTransfer.buf[0].data = i2c_txBuffer;
	i2cTransfer.buf[0].len = i2c_txBufferSize;
	i2cTransfer.buf[1].data = i2c_rxBuffer;
	i2cTransfer.buf[1].len = I2C_RXBUFFER_SIZE;
	I2C_TransferInit(I2C0, &i2cTransfer);

	/* Sending data */
	while (I2C_Transfer(I2C0) == i2cTransferInProgress) {
		;
	}

	/* Clearing pin to indicate end of transfer */
	GPIO_PinOutClear(gpioPortC, 0);
	enableI2cSlaveInterrupts();
}

/**************************************************************************/ /**
 * @brief I2C Interrupt Handler.
 *        The interrupt table is in assembly startup file startup_efm32.s
 *****************************************************************************/
void I2C0_IRQHandler(void)
{
	int status;
        	    uartPutChar('A');

	status = I2C0->IF;

	if (status & I2C_IF_ADDR) {
		/* Address Match */
		/* Indicating that reception is started */
		i2c_rxInProgress = true;
		I2C0->RXDATA;

		I2C_IntClear(I2C0, I2C_IFC_ADDR);


	} else if (status & I2C_IF_RXDATAV) {
		/* Data received */
		i2c_rxBuffer[i2c_rxBufferIndex] = I2C0->RXDATA;
		i2c_rxBufferIndex++;
	}

	if (status & I2C_IEN_SSTOP) {
		/* Stop received, reception is ended */
		I2C_IntClear(I2C0, I2C_IEN_SSTOP);
		i2c_rxInProgress = false;
		i2c_rxBufferIndex = 0;
	}
}


/**************************************************************************/ /**
 * @brief  Receiving I2C data. Along with the I2C interrupt, it will keep the 
  EFM32 in EM1 while the data is received.
 *****************************************************************************/
void receiveI2CData(void)
{
	while (i2c_rxInProgress) {
	    uartPutChar('A');
	/* EMU_EnterEM1(); */
	}

}
