#include <stdint.h>
#include <stdbool.h>
#include "em_device.h"
#include "segmentlcd.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_rtc.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "gpiointerrupt.h"

#define RTC_FREQ    32768

/* Initial setup to 12:00 */
uint32_t hours   = 12;
uint32_t minutes = 0;

/*
 * @brief  Gpio callback
 * @param  pin - pin which triggered interrupt
 */
void gpioCallback(uint8_t pin)
{
    if(pin == 9)
        hours = (hours + 1) % 24;
    else if (pin == 10)
        minutes = (minutes + 1) % 60;
}

/*
 * @brief  Gpio setup. Setup button pins to trigger falling edge interrupts.
 *  Register callbacks for that interrupts.
 */
void gpioSetup(void)
{
  /* Initialize GPIO interrupt dispatcher */
  GPIOINT_Init();

  /* Configure PB9 and PB10 as input */
  GPIO_PinModeSet(gpioPortB, 9, gpioModeInput, 0);
  GPIO_PinModeSet(gpioPortB, 10, gpioModeInput, 0);

  /* Set falling edge interrupt for both ports */
  GPIO_IntConfig(gpioPortB, 9, false, true, true);
  GPIO_IntConfig(gpioPortB, 10, false, true, true);

  /* Register callbacks before setting up and enabling pin interrupt. */
  GPIOINT_CallbackRegister(9, gpioCallback);
  GPIOINT_CallbackRegister(10, gpioCallback);
}

void RTC_IRQHandler(void)
{
  /* Clear interrupt source */
  RTC_IntClear(RTC_IFC_COMP0);

  /* Increase time by one minute */
  minutes++;
  if (minutes > 59)
  {
    minutes = 0;
    hours++;
    if (hours > 23)
    {
      hours = 0;
    }
  }
}

/*
 * @brief Enables LFACLK and selects LFXO as clock source for RTC
 *        Sets up the RTC to generate an interrupt every minute.
 */
void rtcSetup(void)
{
  RTC_Init_TypeDef rtcInit = RTC_INIT_DEFAULT;

  /* Enable LE domain registers */
  CMU_ClockEnable(cmuClock_CORELE, true);

  /* Enable LFXO as LFACLK in CMU. This will also start LFXO */
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);

  /* Set a clock divisor of 32 to reduce power conumption. */
  CMU_ClockDivSet(cmuClock_RTC, cmuClkDiv_32);

  /* Enable RTC clock */
  CMU_ClockEnable(cmuClock_RTC, true);

  /* Initialize RTC */
  rtcInit.enable   = false;  /* Do not start RTC after initialization is complete. */
  rtcInit.debugRun = false;  /* Halt RTC when debugging. */
  rtcInit.comp0Top = true;   /* Wrap around on COMP0 match. */
  RTC_Init(&rtcInit);

  /* Interrupt every minute */
  RTC_CompareSet(0, ((RTC_FREQ / 32) * 60 ) - 1 );

  /* Enable interrupt */
  NVIC_EnableIRQ(RTC_IRQn);
  RTC_IntEnable(RTC_IEN_COMP0);

  /* Start Counter */
  RTC_Enable(true);
}
void clockLoop(void)
{
  while (1)
  {
    SegmentLCD_Number(hours * 100 + minutes);
    EMU_EnterEM2(true);
  }
}

void interrupt_test(void)
{
  /* Ensure core frequency has been updated */
  SystemCoreClockUpdate();

  /* Init LCD with no voltage boost */
  SegmentLCD_Init(false);

  /* Setup RTC to generate an interrupt every minute */
  rtcSetup();

  /* Setup GPIO with interrupts to serve the pushbuttons */
  gpioSetup();

  /* Main function loop */
  clockLoop();
}
