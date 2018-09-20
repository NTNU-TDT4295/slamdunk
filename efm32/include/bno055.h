#ifndef BNO055_H
#define BNO055_H

#include "stdint.h"

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

#define BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR 0X28
#define BNO055_LINEAR_ACCEL_DATA_X_MSB_ADDR 0X29
#define BNO055_LINEAR_ACCEL_DATA_Y_LSB_ADDR 0X2A
#define BNO055_LINEAR_ACCEL_DATA_Y_MSB_ADDR 0X2B
#define BNO055_LINEAR_ACCEL_DATA_Z_LSB_ADDR 0X2C
#define BNO055_LINEAR_ACCEL_DATA_Z_MSB_ADDR 0X2D

// Why do we need to left shift by 1 here?  The address is contained
// in a byte, but the addressing mode uses only the 7 most significant
// bits, while the LSB is R/W
//
// eg: 0010 1000 becomes 0101 000{R/W} after the shift -- note the
// 7-bit address that is used
#define BNO055_I2C_ADDRESS (0x28 << 1)

struct euler {
	double x;
	double y;
	double z;
};

struct accel {
	int16_t x;
	int16_t y;
	int16_t z;
};

void init_bno055();
struct euler get_euler_sample();
struct accel get_linear_acceleration_sample();

#endif
