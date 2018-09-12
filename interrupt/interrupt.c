#include <stdint.h>
#include <stdbool.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "gpiointerrupt.h"

/**************************************************************************//**
 * @brief  Gpio callback
 * @param  pin - pin which triggered interrupt
 *****************************************************************************/
void gpioCallback(uint8_t pin)
{
  if (pin == 9)
  {
    BSP_LedToggle(1);
  }
  else if (pin == 10)
  {
    BSP_LedToggle(0);
  }
}

/**************************************************************************//**
 * @brief  Gpio setup. Setup button pins to trigger falling edge interrupts.
 *  Register callbacks for that interrupts.
 *****************************************************************************/
void gpioSetup(void)
{
  /* Initialize GPIO interrupt dispatcher */
  GPIOINT_Init();

  /* Configure PB9 and PB10 as input */
  GPIO_PinModeSet(gpioPortB, 9, gpioModeInput, 0);
  GPIO_PinModeSet(gpioPortB, 10, gpioModeInput, 0);

  /* Register callbacks before setting up and enabling pin interrupt. */
  GPIOINT_CallbackRegister(9, gpioCallback);
  GPIOINT_CallbackRegister(10, gpioCallback);

  /* Set falling edge interrupt for both ports */
  GPIO_IntConfig(gpioPortB, 9, false, true, true);
  GPIO_IntConfig(gpioPortB, 10, false, true, true);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
void interrupt_test(void)
{
  gpioSetup();

  /* Initialize LED driver */
  BSP_LedsInit();

  /* Infinite loop */
  while (1);
}
