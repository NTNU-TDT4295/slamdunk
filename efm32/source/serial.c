#include <stdio.h>
#include "serial.h"

////////////////
// UART
////////////////

// Currently registered UART callback -- it really just prints to the
// LCD for now
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

void init_uart()
{
	// LCD for proof of concept, remove later (TODO)
	SegmentLCD_Init(false);

	/* Setup UART for testing */
	set_recv_callback(&recv_char_cb);
	setup_uart();
}

// Mainly for debugging purposes when you want to monitor the pins
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
// I2C
////////////////

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
}

// NOTE: the address of the register must be the first byte *buf (this
// is super stupid, TODO me)
void performI2CTransfer(uint8_t addr, uint8_t* buf, uint8_t bytes)
{
	/* Transfer structure */
	I2C_TransferSeq_TypeDef i2cTransfer;

	/* Setting pin to indicate transfer */
	GPIO_PinOutSet(gpioPortC, 0);

	/* Initializing I2C transfer */
	i2cTransfer.addr = addr;
	i2cTransfer.flags = I2C_FLAG_WRITE;

	i2cTransfer.buf[0].data = buf;
	i2cTransfer.buf[0].len = bytes;

	// Initialize the transfer and wait for completion
	I2C_TransferInit(I2C0, &i2cTransfer);
	while (I2C_Transfer(I2C0) == i2cTransferInProgress) {
		;
	}

	/* Clearing pin to indicate end of transfer */
	GPIO_PinOutClear(gpioPortC, 0);
}

void performI2CRead(uint8_t addr, int8_t reg, uint8_t* buf, uint8_t bytes)
{
	/* Transfer structure */
	I2C_TransferSeq_TypeDef i2cTransfer;

	uint8_t regid[1] = {reg};

	// EULER data registers
	// 0x1A to 0x1F, 0x1A is LSB 0x1B is MSB of 16-bit value

	/* Setting pin to indicate transfer */
	GPIO_PinOutSet(gpioPortC, 0);

	/* Initializing I2C transfer */
	i2cTransfer.addr = addr;
	i2cTransfer.flags = I2C_FLAG_WRITE_READ;

	i2cTransfer.buf[0].data = regid;
	i2cTransfer.buf[0].len = 1;

	i2cTransfer.buf[1].data = buf;
	i2cTransfer.buf[1].len = bytes;

	// Initialize a transfer and wait for completion, note that no
	// error handling/reporting is done (TODO)
	I2C_TransferInit(I2C0, &i2cTransfer);
	while (I2C_Transfer(I2C0) == i2cTransferInProgress) {
		;
	}

	/* Clearing pin to indicate end of transfer */
	GPIO_PinOutClear(gpioPortC, 0);
}

void I2C0_IRQHandler(void)
{
	;
}


////////////////// SPI////////////////////////////////////////// SPI////////////////////////


/**************************************************************************/ /**
 * @brief  Setup SPI as Master
 *****************************************************************************/
/// FIXME: DONT USE
void setupSpi(void)
{
	USART_InitSync_TypeDef usartInit = USART_INITSYNC_DEFAULT;

	/* Initialize SPI */
	usartInit.databits = usartDatabits8;
	usartInit.baudrate = 1000000;
	USART_InitSync(USART1, &usartInit);

	/* Turn on automatic Chip Select control */
	USART1->CTRL |= USART_CTRL_AUTOCS;

	/* Enable SPI transmit and receive */
	USART_Enable(USART1, usartEnable);

	/* Configure GPIO pins for SPI */
	GPIO_PinModeSet(gpioPortD, 0, gpioModePushPull, 0); /* MOSI */
	GPIO_PinModeSet(gpioPortD, 1, gpioModeInput, 0);	/* MISO */
	GPIO_PinModeSet(gpioPortD, 2, gpioModePushPull, 0); /* CLK */
	GPIO_PinModeSet(gpioPortD, 3, gpioModePushPull, 1); /* CS */

	/* Enable routing for SPI pins from USART to location 1 */
	USART1->ROUTE = USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | USART_ROUTE_CSPEN | USART_ROUTE_CLKPEN | USART_ROUTE_LOCATION_LOC1;
}


////////////////////////////////////////////////////////////////


/* char* slaveTxBuffer; */
/* int slaveTxBufferSize; */
/* volatile int slaveTxBufferIndex; */
char* slaveRxBuffer;
int slaveRxBufferSize;
volatile int slaveRxBufferIndex;

char* masterRxBuffer;
int masterRxBufferSize;
volatile int masterRxBufferIndex;

#define HFRCO_FREQUENCY 14000000
#define SPI_PERCLK_FREQUENCY HFRCO_FREQUENCY
#define SPI_BAUDRATE 1000000

void SPI_setup()
{
	USART_TypeDef* spi;

	/* Determining USART */
	spi = USART1;
	uint8_t location = 1;

	/* Setting baudrate */
	spi->CLKDIV = 128 * (SPI_PERCLK_FREQUENCY / SPI_BAUDRATE - 2);

	/* Configure SPI */
	/* Using synchronous (SPI) mode*/
	spi->CTRL = USART_CTRL_SYNC;
	/* Clearing old transfers/receptions, and disabling interrupts */
	spi->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;
	spi->IEN = 0;
	/* Enabling pins and setting location */
	spi->ROUTE = USART_ROUTE_TXPEN
				 | USART_ROUTE_RXPEN
				 | USART_ROUTE_CLKPEN
				 | USART_ROUTE_CSPEN
				 | (location << 8);

	/* Set to master and to control the CS line */
	/* Enabling Master, TX and RX */
	spi->CMD = USART_CMD_MASTEREN | USART_CMD_TXEN | USART_CMD_RXEN;
	spi->CTRL |= USART_CTRL_AUTOCS;

	/* Set GPIO config to master */
	GPIO_Mode_TypeDef gpioModeMosi = gpioModePushPull;
	GPIO_Mode_TypeDef gpioModeMiso = gpioModeInput;
	GPIO_Mode_TypeDef gpioModeCs = gpioModePushPull;
	GPIO_Mode_TypeDef gpioModeClk = gpioModePushPull;

	/* Clear previous interrupts */
	spi->IFC = _USART_IFC_MASK;

	/* IO configuration */
	// https://www.silabs.com/documents/public/data-sheets/efm32gg-datasheet.pdf (Rev. 2.0)
	// p. 380
	/* IO configuration (USART1, Location #1) */
	GPIO_PinModeSet(gpioPortD, 0, gpioModeMosi, 0); /* MOSI */
	GPIO_PinModeSet(gpioPortD, 1, gpioModeMiso, 0); /* MISO */
	GPIO_PinModeSet(gpioPortD, 3, gpioModeCs, 0);   /* CS */
	GPIO_PinModeSet(gpioPortD, 2, gpioModeClk, 0);  /* Clock */
}

/**************************************************************************/ /**
 * @brief Setting up RX interrupts from USART2 RX
 * @param receiveBuffer points to where received data is to be stored
 * @param bytesToReceive indicates the number of bytes to receive
 *****************************************************************************/
void SPI2_setupRXInt(char* receiveBuffer, int bytesToReceive)
{
	USART_TypeDef* spi = USART2;

	/* Setting up pointer and indexes */
	masterRxBuffer = receiveBuffer;
	masterRxBufferSize = bytesToReceive;
	masterRxBufferIndex = 0;

	/* Flushing rx */
	spi->CMD = USART_CMD_CLEARRX;

	/* Enable interrupts */
	NVIC_ClearPendingIRQ(USART2_RX_IRQn);
	NVIC_EnableIRQ(USART2_RX_IRQn);
	spi->IEN = USART_IEN_RXDATAV;
}


/******************************************************************************
 * @brief sends data using USART2
 * @param txBuffer points to data to transmit
 * @param bytesToSend bytes will be sent
 *****************************************************************************/
void USART2_sendBuffer(char* txBuffer, int bytesToSend)
{
	USART_TypeDef* spi = USART2;

	/* Sending the data */
	for (int i = 0; i < bytesToSend; ++i) {
		/* Waiting for the usart to be ready */
		while (!(spi->STATUS & USART_STATUS_TXBL))
			;

		if (txBuffer != 0) {
			/* Writing next byte to USART */
			spi->TXDATA = *txBuffer;
			++txBuffer;
		} else {
			spi->TXDATA = 0;
		}
	}

	/*Waiting for transmission of last byte */
	while (!(spi->STATUS & USART_STATUS_TXC))
		;
}


/// Random receive


void SPI_setup_slave_rec()
{
	USART_TypeDef* spi;

	/* Determining USART */

	spi = USART1;
	uint8_t location = 1;

	/* Setting baudrate */
	spi->CLKDIV = 128 * (SPI_PERCLK_FREQUENCY / SPI_BAUDRATE - 2);

	/* Configure SPI */
	/* Using synchronous (SPI) mode*/
	spi->CTRL = USART_CTRL_SYNC;
	/* Clearing old transfers/receptions, and disabling interrupts */
	spi->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;
	spi->IEN = 0;
	/* Enabling pins and setting location */
	spi->ROUTE = USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | USART_ROUTE_CLKPEN | USART_ROUTE_CSPEN | (location << 8);

	/* Set GPIO config to slave */
	GPIO_Mode_TypeDef gpioModeMosi = gpioModeInput;
	GPIO_Mode_TypeDef gpioModeMiso = gpioModePushPull;
	GPIO_Mode_TypeDef gpioModeCs = gpioModeInput;
	GPIO_Mode_TypeDef gpioModeClk = gpioModeInput;

	/* Enabling RX */
	spi->CMD = USART_CMD_RXEN;

	/* Clear previous interrupts */
	spi->IFC = _USART_IFC_MASK;

	/* IO configuration */

	/* IO configuration (USART1, Location #1) */
	GPIO_PinModeSet(gpioPortD, 0, gpioModeMosi, 0); /* MOSI */
	GPIO_PinModeSet(gpioPortD, 1, gpioModeMiso, 0); /* MISO */
	GPIO_PinModeSet(gpioPortD, 3, gpioModeCs, 0);   /* CS */
	GPIO_PinModeSet(gpioPortD, 2, gpioModeClk, 0);  /* Clock */
}


/**************************************************************************/ /**
 * @brief USART1 RX IRQ Handler Setup
 * @param receiveBuffer points to where to place recieved data
 * @param receiveBufferSize indicates the number of bytes to receive
 *****************************************************************************/
void SPI1_setupRXInt(char* receiveBuffer, int receiveBufferSize)
{
	USART_TypeDef* spi = USART1;

	/* Setting up pointer and indexes */
	slaveRxBuffer = receiveBuffer;
	slaveRxBufferSize = receiveBufferSize;
	slaveRxBufferIndex = 0;

	/* Clear RX */
	spi->CMD = USART_CMD_CLEARRX;

	/* Enable interrupts */
	NVIC_ClearPendingIRQ(USART1_RX_IRQn);
	NVIC_EnableIRQ(USART1_RX_IRQn);
	spi->IEN |= USART_IEN_RXDATAV;
}


/**************************************************************************/ /**
 * @brief USART1 IRQ Handler Setup
 * @param receiveBuffer points to where received data is to be stored
 * @param receiveBufferSize indicates the number of bytes to receive
 * @param transmitBuffer points to the data to send
 * @param transmitBufferSize indicates the number of bytes to send
 *****************************************************************************/
void SPI1_setupSlaveInt(char* receiveBuffer, int receiveBufferSize, char* transmitBuffer, int transmitBufferSize)
{
	SPI1_setupRXInt(receiveBuffer, receiveBufferSize);
}


/**************************************************************************/ /**
 * @brief USART1 RX IRQ Handler
 *****************************************************************************/
void USART1_RX_IRQHandler(void)
{
	USART_TypeDef* spi = USART1;
	uint8_t rxdata;

	if (spi->STATUS & USART_STATUS_RXDATAV) {
		/* Reading out data */
		rxdata = spi->RXDATA;

		if (slaveRxBufferIndex < slaveRxBufferSize) {
			/* Store Data */
			slaveRxBuffer[slaveRxBufferIndex] = rxdata;
			slaveRxBufferIndex++;
		}
	}
}
