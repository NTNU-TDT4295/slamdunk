#include <stdio.h>
#include "serial.h"

////////////////
// UART
////////////////

void init_uart()
{
	USART_InitAsync_TypeDef uartInit = USART_INITASYNC_DEFAULT;

	/* Enable clock for HF peripherals */
	CMU_ClockEnable(cmuClock_HFPER, true);

	/* Enable clock for USART module */
	CMU_ClockEnable(cmuClock_UART0, true);
	CMU_ClockEnable(cmuClock_UART1, true);

	/* Enable clock for GPIO module (required for pin configuration) */
	CMU_ClockEnable(cmuClock_GPIO, true);

	// LIDAR
	GPIO_PinModeSet(gpioPortF, 6, gpioModePushPull, 1); //TX
	GPIO_PinModeSet(gpioPortF, 7, gpioModeInput, 0);	//RX

	// Debug pins
	GPIO_PinModeSet(gpioPortF, 10, gpioModePushPull, 1); //TX
	GPIO_PinModeSet(gpioPortF, 11, gpioModeInput, 0);	//RX


	/* Prepare struct for initializing UART in asynchronous mode*/
	uartInit.enable = usartDisable;		/* Don't enable UART upon intialization */
	uartInit.refFreq = 0;				/* Provide information on reference frequency. When set to 0, the reference frequency is */
	uartInit.baudrate = 115200;			/* Baud rate */
	uartInit.oversampling = usartOVS16; /* Oversampling. Range is 4x, 6x, 8x or 16x */
	uartInit.databits = usartDatabits8; /* Number of data bits. Range is 4 to 10 */
	uartInit.parity = usartNoParity;	/* Parity mode */
	uartInit.stopbits = usartStopbits1; /* Number of stop bits. Range is 0 to 2 */
	uartInit.mvdis = false;				/* Disable majority voting */
	uartInit.prsRxEnable = false;		/* Enable USART Rx via Peripheral Reflex System */
	/* uartInit.prsRxCh      = usartPrsRxCh0;  	/\* Select PRS channel if enabled *\/ */

	/* Initialize USART with uartInit struct */
	USART_InitAsync(UART0, &uartInit);
	USART_InitAsync(UART1, &uartInit);

	// TX and RX for both locations
	UART0->ROUTE = UART_ROUTE_RXPEN | UART_ROUTE_TXPEN | UART_ROUTE_LOCATION_LOC0;
	UART1->ROUTE = UART_ROUTE_RXPEN | UART_ROUTE_TXPEN | UART_ROUTE_LOCATION_LOC1;

	/* Enable UART */
	USART_Enable(UART0, usartEnable);
	USART_Enable(UART1, usartEnable);
}

void put_uart_simple(int channel, uint8_t* data, size_t length)
{
	for (size_t i = 0; i < length; ++i) {
		if (channel == 0)
			USART_Tx(UART0, data[i]);
		else
			USART_Tx(UART1, data[i]);
	}
}

////////////////
// I2C
////////////////

void init_i2c(void)
{
	CMU_ClockEnable(cmuClock_I2C1, true);

	// Using default settings
	I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;

	/* Using PB11 (SDA) and PB12 (SCL) */
	GPIO_PinModeSet(gpioPortB, 11, gpioModeWiredAndPullUpFilter, 1);
	GPIO_PinModeSet(gpioPortB, 12, gpioModeWiredAndPullUpFilter, 1);

	// Setting up PC0 to indicate transfer direction
	GPIO_PinModeSet(gpioPortC, 0, gpioModePushPull, 0);

	/* Enable pins at location 1 */
	I2C1->ROUTE = I2C_ROUTE_SDAPEN | I2C_ROUTE_SCLPEN | I2C_ROUTE_LOCATION_LOC1;

	/* Initializing the I2C */
	I2C_Init(I2C1, &i2cInit);
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
	I2C_TransferInit(I2C1, &i2cTransfer);
	while (I2C_Transfer(I2C1) == i2cTransferInProgress) {
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
	I2C_TransferInit(I2C1, &i2cTransfer);
	while (I2C_Transfer(I2C1) == i2cTransferInProgress) {
		;
	}

	/* Clearing pin to indicate end of transfer */
	GPIO_PinOutClear(gpioPortC, 0);
}

void I2C0_IRQHandler(void)
{
	;
}


////////////////
// SPI
////////////////


// Master, txonly. USART1, location 1
void init_SPI(void)
{
	CMU_ClockEnable(cmuClock_USART1, true);

	USART_InitSync_TypeDef usartInit = USART_INITSYNC_DEFAULT;

	// Init
	usartInit.enable = usartEnableTx;
	usartInit.baudrate = SPI_BAUDRATE;
	usartInit.databits = usartDatabits8;
	usartInit.master = true;
	usartInit.msbf = true;				   // Matches current FPGA implementation
	usartInit.clockMode = usartClockMode0; // Idle low, sample on rising edge
	USART_InitSync(USART1, &usartInit);

	// Automatic CS
	USART1->CTRL |= USART_CTRL_AUTOCS;

	// Enable SPI tx
	/* USART_Enable(USART1, usartEnableTx); */

	// https://www.silabs.com/documents/public/data-sheets/efm32gg-datasheet.pdf (Rev. 2.0)
	// p. 380
	/* IO configuration (USART1, Location #1, master send setup) */
	GPIO_PinModeSet(gpioPortD, 0, gpioModePushPull, 0); // MOSI
	GPIO_PinModeSet(gpioPortD, 2, gpioModePushPull, 0); // Clock
	GPIO_PinModeSet(gpioPortD, 3, gpioModePushPull, 0); // CS

	USART1->ROUTE = USART_ROUTE_TXPEN
					| USART_ROUTE_CLKPEN
					| USART_ROUTE_CSPEN
					| USART_ROUTE_LOCATION_LOC1;
}

// USART1, Location 1
void SPI_sendBuffer(char* txBuffer, int bytesToSend)
{
	USART_TypeDef* spi = USART1;

	// Send data
	for (int i = 0; i < bytesToSend; ++i) {
		// TXBL: set when data moved from TX buffer, new byte may be written
		while (!(spi->STATUS & USART_STATUS_TXBL)) // Busy wait, cheaper than irq
			;

		// Write data
		if (txBuffer != 0) {
			spi->TXDATA = *txBuffer;
			++txBuffer;
		} else {
			spi->TXDATA = 0;
		}
	}

	// Wait for last txd byte
	// TXC: set when all available TX data (shift reg && buffer)
	while (!(spi->STATUS & USART_STATUS_TXC)) // Busy wait, cheaper than irq
		;
}
