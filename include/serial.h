#ifndef SERIAL_H
#define SERIAL_H

#include "bsp.h"
#include "bsp_trace.h"
#include "segmentlcd.h"
#include "setup.h"
#include "uart.h"
#include "em_emu.h"

#define LCD_BUFFER_SIZE SEGMENT_LCD_NUM_BLOCK_COLUMNS + 1

enum key_kode {
	ENTER = 13,
	BACKSPACE = 8
};

void init_uart(void);
void uart_test(void);
void recv_char_cb(char c);




#include "em_device.h"
#include "em_chip.h"
#include "em_i2c.h"
#include "em_rtc.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"

/* Defines*/
#define I2C_ADDRESS                     0xE2
#define I2C_RXBUFFER_SIZE                 10

// Buffers++
extern uint8_t i2c_txBuffer[6];
extern uint8_t i2c_txBufferSize;
extern uint8_t i2c_rxBuffer[I2C_RXBUFFER_SIZE];
extern uint8_t i2c_rxBufferIndex;

// Transmission flags
extern volatile bool i2c_rxInProgress;
extern volatile bool i2c_startTx;


void enableI2cSlaveInterrupts(void);
void init_i2c(void);
void performI2CTransfer(void);
void I2C0_IRQHandler(void);
void receiveI2CData(void);

#endif

