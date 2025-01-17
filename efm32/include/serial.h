#ifndef SERIAL_H
#define SERIAL_H

#include <stddef.h>

////////////////
// UART
////////////////

#include "bsp.h"
#include "bsp_trace.h"
#include "setup.h"
#include "em_emu.h"

void init_uart();
void put_uart_simple(int channel, uint8_t *data, size_t length);

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

////////////////
// SPI
////////////////

#define SPI_BAUDRATE 500000
void init_SPI(void);
void SPI_sendBuffer(char* txBuffer, int bytesToSend);

#endif
