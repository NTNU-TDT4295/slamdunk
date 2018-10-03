#include <stdio.h>
#include "serial.h"

////////////////
// UART
////////////////

// Currently registered UART callback -- it really just prints to the
// LCD for now
void recv_char_cb(char c)
{

    //char cc;
    //cc = uartGetChar();
    ////uartPutChar(0,cc);
    //USART_Tx(UART0, cc);

    //lidar_data[lidar_i] = cc;
    //if (lidar_i == 4){
    ////    uartPutChar(0,lidar_data[0]);
    ////    uartPutChar(0,lidar_data[1]);
    ////    uartPutChar(0,lidar_data[2]);
    ////    uartPutChar(0,lidar_data[3]);
    ////    uartPutChar(0,lidar_data[4]);
    //    //USART_Tx(UART0, lidar_data[0]);
    //    //USART_Tx(UART0, lidar_data[1]);
    //    //USART_Tx(UART0, lidar_data[2]);
    //    //USART_Tx(UART0, lidar_data[3]);
    //    //USART_Tx(UART0, lidar_data[4]);
    //    USART_Tx(UART0, '\xff');
    //    //DelayMs(1000);
    //}
    //lidar_i++;
    //lidar_i = lidar_i % 5;

	//static char str[LCD_BUFFER_SIZE];
	//static int idx = 0;

	//switch (c) {
	//case BACKSPACE:
	//	if (idx != 0) {
	//		str[--idx] = 0;
	//	}
	//	break;
	//case ENTER:
	//	for (size_t i = 0; i < LCD_BUFFER_SIZE; ++i) {
	//		str[i] = 0;
	//		idx = 0;
	//	}
	//	break;
	//default:
	//	uartPutChar(0, c);
	//	break;
	//}

	//if ('a' <= c && c <= 'z') {
	//	str[idx] = c - 32;
	//	idx = idx == SEGMENT_LCD_NUM_BLOCK_COLUMNS
	//		? SEGMENT_LCD_NUM_BLOCK_COLUMNS
	//		: idx + 1;
	//}

	//SegmentLCD_Write(str);
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
			uartPutChar(0, rx_data);
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
void performI2CTransfer(uint8_t addr, uint8_t *buf, uint8_t bytes)
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

void performI2CRead(uint8_t addr, int8_t reg, uint8_t *buf, uint8_t bytes)
{
	/* Transfer structure */
	I2C_TransferSeq_TypeDef i2cTransfer;

	uint8_t regid[1] = { reg };

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
