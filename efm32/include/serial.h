#ifndef SERIAL_H
#define SERIAL_H

#include "bsp.h"
#include "bsp_trace.h"
#include "segmentlcd.h"
#include "setup.h"
#include "uart.h"
#include "em_emu.h"

#define LCD_BUFFER_SIZE SEGMENT_LCD_NUM_BLOCK_COLUMNS + 1

// BNO055
#define BNO055_SYS_STAT_ADDR    0x39
#define BNO055_SYS_TRIGGER_ADDR 0x3F
#define BNO055_PWR_MODE_ADDR    0x3E
#define BNO055_PAGE_ID_ADDR     0x07

#define POWER_MODE_NORMAL   0x00
#define POWER_MODE_LOWPOWER 0x01
#define POWER_MODE_SUSPEND  0x02

#define BNO055_EULER_H_LSB_ADDR 0X1A
#define BNO055_EULER_H_MSB_ADDR 0X1B
#define BNO055_EULER_R_LSB_ADDR 0X1C
#define BNO055_EULER_R_MSB_ADDR 0X1D
#define BNO055_EULER_P_LSB_ADDR 0X1E
#define BNO055_EULER_P_MSB_ADDR 0X1F

#define BNO055_QUATERNION_DATA_W_LSB_ADDR 0X20
#define BNO055_QUATERNION_DATA_W_MSB_ADDR 0X21
#define BNO055_QUATERNION_DATA_X_LSB_ADDR 0X22
#define BNO055_QUATERNION_DATA_X_MSB_ADDR 0X23
#define BNO055_QUATERNION_DATA_Y_LSB_ADDR 0X24
#define BNO055_QUATERNION_DATA_Y_MSB_ADDR 0X25
#define BNO055_QUATERNION_DATA_Z_LSB_ADDR 0X26
#define BNO055_QUATERNION_DATA_Z_MSB_ADDR 0X27

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
#include "em_gpio.h"

/* Defines*/
#define I2C_ADDRESS               (0x28 << 1)
#define I2C_RXBUFFER_SIZE                 10

// Buffers++
extern uint8_t i2c_txBuffer[6];
extern uint8_t i2c_txBufferSize;
extern uint8_t i2c_rxBuffer[I2C_RXBUFFER_SIZE];
extern uint8_t i2c_rxBufferIndex;

// Transmission flags
extern volatile bool i2c_rxInProgress;
extern volatile bool i2c_startTx;

void init_i2c(void);
void performI2CTransfer(uint8_t data[], uint8_t bytes);
void performI2CRead(int8_t reg, uint8_t *buf, uint8_t bytes);
void I2C0_IRQHandler(void);

#endif

