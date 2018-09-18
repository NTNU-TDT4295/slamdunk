#include "bno055.h"
#include "serial.h"
#include "setup.h"

void init_bno055()
{
	// Normal power mode
	uint8_t power_mode[] = "\x3E\x00";
	performI2CTransfer(BNO055_I2C_ADDRESS, power_mode, 2);

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

	Delay(50);
}
