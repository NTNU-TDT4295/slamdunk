#include "bno055.h"
#include "serial.h"
#include "setup.h"
#include "string.h"
#include "gpiointerrupt.h"

void accelerometer_callback(uint8_t pin)
{
	uartPutChar(0, 'f');

	// Write RST_INT in SYS_TRIGGER to reset interrupt
	uint8_t mode[] = "\x3F\x40";
	performI2CTransfer(BNO055_I2C_ADDRESS, mode, 2);
}

void init_bno055()
{
	// Normal power mode
	uint8_t power_mode[] = "\x3E\x00";
	performI2CTransfer(BNO055_I2C_ADDRESS, power_mode, 2);

	Delay(50);

	// Initialize interrupt from accelerometer
	init_accelerometer_int();

	Delay(50);

	// Page (TODO: what does this «page 0x00» even do -- its from
	// Adafruit's driver)
	uint8_t page[] = "\x07\x00";
	performI2CTransfer(BNO055_I2C_ADDRESS, page, 2);

	// Perform self test
	uint8_t self_test[] = "\x3F\x00";
	performI2CTransfer(BNO055_I2C_ADDRESS, self_test, 2);

	Delay(50);

	uint8_t mode[] = "\x3D\x0C";
	performI2CTransfer(BNO055_I2C_ADDRESS, mode, 2);
}

void init_accelerometer_int()
{
	// Also init GPIO interrupts for accelerometer interrupts
	GPIOINT_Init();

	// Port, pin, rising edge, falling edge, enable
	GPIO_PinModeSet(gpioPortD, 1, gpioModeInput, 0);
	GPIO_IntConfig(gpioPortD, 1, true, false, true);
	GPIOINT_CallbackRegister(1, accelerometer_callback);

	// Mask interrupt on INT pin
	uint8_t interrupt_mask[] = "\x0F\x40";
	performI2CTransfer(BNO055_I2C_ADDRESS, interrupt_mask, 2);
	Delay(50);

	// Enable accelerometer interrupts on any movement
	uint8_t interrupt_en[] = "\x10\x40";
	performI2CTransfer(BNO055_I2C_ADDRESS, interrupt_en, 2);
	Delay(50);

	// Set acceleration threshold to about 1g with 2g-range in
	// ACC_CONFIG (~255 LSB)
	uint8_t interrupt_thresh[] = "\x11\x04";
	performI2CTransfer(BNO055_I2C_ADDRESS, interrupt_thresh, 2);
	Delay(50);

	// Enable interrupts on X,Y,Z axes
	uint8_t interrupt_axes[] = "\x12\x1C";
	performI2CTransfer(BNO055_I2C_ADDRESS, interrupt_axes, 2);
	Delay(50);
}

struct euler get_euler_sample()
{
	uint8_t euler_buf[6];
	memset(euler_buf, 0, 6);

	performI2CRead(BNO055_I2C_ADDRESS, BNO055_EULER_H_LSB_ADDR, euler_buf, 6);
	int16_t x = ((int16_t) euler_buf[0]) | (((int16_t) euler_buf[1]) << 8);
	int16_t y = ((int16_t) euler_buf[2]) | (((int16_t) euler_buf[3]) << 8);
	int16_t z = ((int16_t) euler_buf[4]) | (((int16_t) euler_buf[5]) << 8);

	return (struct euler) {
		.x = ((double) x) / 16.0,
		.y = ((double) y) / 16.0,
		.z = ((double) z) / 16.0,
	};
}

struct quaternion get_quaternion_sample()
{
	uint8_t quat_buf[8];
	memset(quat_buf, 0, 8);

	performI2CRead(BNO055_I2C_ADDRESS, BNO055_QUATERNION_DATA_W_LSB_ADDR, quat_buf, 8);
	int16_t w = ((int16_t) quat_buf[0]) | (((int16_t) quat_buf[1]) << 8);
	int16_t x = ((int16_t) quat_buf[2]) | (((int16_t) quat_buf[3]) << 8);
	int16_t y = ((int16_t) quat_buf[4]) | (((int16_t) quat_buf[5]) << 8);
	int16_t z = ((int16_t) quat_buf[6]) | (((int16_t) quat_buf[7]) << 8);

	return (struct quaternion) {
		.w = w,
		.x = x,
		.y = y,
		.z = z,
	};
}

struct accel get_linear_acceleration_sample()
{
	uint8_t accel_buf[6];
	memset(accel_buf, 0, 6);

	performI2CRead(BNO055_I2C_ADDRESS, BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR, accel_buf, 6);
	int16_t x = ((int16_t) accel_buf[0]) | (((int16_t) accel_buf[1]) << 8);
	int16_t y = ((int16_t) accel_buf[2]) | (((int16_t) accel_buf[3]) << 8);
	int16_t z = ((int16_t) accel_buf[4]) | (((int16_t) accel_buf[5]) << 8);

	return (struct accel) {
		.x = x,
		.y = y,
		.z = z,
	};
}
