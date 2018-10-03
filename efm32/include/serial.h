#ifndef SERIAL_H
#define SERIAL_H

////////////////
// UART
////////////////

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

void recv_char_cb(char c);
void init_uart();
void echo_uart();

////////////////
// I2C
////////////////

#include "em_device.h"
#include "em_chip.h"
#include "em_i2c.h"
#include "em_rtc.h"
#include "em_cmu.h"
#include "em_gpio.h"

void init_i2c(void);
void performI2CTransfer(uint8_t addr, uint8_t buf[], uint8_t bytes);
void performI2CRead(uint8_t addr, int8_t reg, uint8_t *buf, uint8_t bytes);
void I2C0_IRQHandler(void);

#endif
