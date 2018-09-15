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
// I2C
////////////////

// \x3D is OPR_MODE_ADDR, \x08 is IMU_PLUS (IMU) mode
uint8_t i2c_txBuffer[] = "\x3D\x08";
uint8_t i2c_txBufferSize = sizeof(i2c_txBuffer);
uint8_t i2c_rxBuffer[I2C_RXBUFFER_SIZE];
uint8_t i2c_rxBufferIndex;

volatile bool i2c_rxInProgress;
volatile bool i2c_startTx;

I2C_TransferReturn_TypeDef I2CStatus;

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

	/* uint8_t data[] = "\x3D\x08"; */
	/* uint8_t dataLen = sizeof(data); */
	uint8_t data[] = "\x3D\x0C";
	uint8_t dataLen = sizeof(data);

	/* Initializing I2C transfer */
	i2cTransfer.addr = I2C_ADDRESS;
	i2cTransfer.flags = I2C_FLAG_WRITE;

	i2cTransfer.buf[0].data = data;
	i2cTransfer.buf[0].len = dataLen;
	i2cTransfer.buf[1].data = i2c_rxBuffer;
	i2cTransfer.buf[1].len = I2C_RXBUFFER_SIZE;

	I2C_TransferInit(I2C0, &i2cTransfer);

	I2C_TransferReturn_TypeDef ret = I2C_Transfer(I2C0);

	/* Sending data */
	while (ret == i2cTransferInProgress) {
		ret = I2C_Transfer(I2C0);
		uartPutChar((uint8_t) ret);
	}

	/* Clearing pin to indicate end of transfer */
	GPIO_PinOutClear(gpioPortC, 0);
}

void performI2CRead(int8_t reg, uint8_t *buf, uint8_t bytes)
{
	/* Transfer structure */
	I2C_TransferSeq_TypeDef i2cTransfer;

	uint8_t regid[1] = { reg };

	// EULER data registers
	// 0x1A to 0x1F, 0x1A is LSB 0x1B is MSB of 16-bit value

	/* Setting pin to indicate transfer */
	GPIO_PinOutSet(gpioPortC, 0);

	/* Initializing I2C transfer */
	i2cTransfer.addr = I2C_ADDRESS;
	i2cTransfer.flags = I2C_FLAG_WRITE_READ;

	i2cTransfer.buf[0].data = regid;
	i2cTransfer.buf[0].len = 1;

	i2cTransfer.buf[1].data = buf;
	i2cTransfer.buf[1].len = bytes;

	I2CStatus = I2C_TransferInit(I2C0, &i2cTransfer);

	I2C_TransferReturn_TypeDef ret = I2C_Transfer(I2C0);

	/* Waiting for data */
	while (I2CStatus == i2cTransferInProgress) {
		I2CStatus = I2C_Transfer(I2C0);
	}

	/* Clearing pin to indicate end of transfer */
	GPIO_PinOutClear(gpioPortC, 0);
}

void I2C0_IRQHandler(void)
{
	;
}

void RTC_IRQHandler(void)
{
	;
}
