//EFM32 blink test

#ifndef LED_PIN
#define LED_PIN     4
#endif
#ifndef LED_PORT
#define LED_PORT    gpioPortA
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "bsp.h"
#include "bsp_trace.h"

volatile uint32_t msTicks; /* counts 1ms timeTicks */

void Delay(uint32_t dlyTicks);

/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void)
{
    msTicks++;       /* increment counter necessary in Delay()*/
}

/**************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 *****************************************************************************/
void Delay(uint32_t dlyTicks)
{
    uint32_t curTicks;

    curTicks = msTicks;
    while ((msTicks - curTicks) < dlyTicks) ;
}

void recv_char_cb(char c)
{
	switch (c) {
	case 'a':
		BSP_LedToggle(0);
		BSP_LedToggle(1);
		break;
	default:
		break;
	}
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
    CHIP_Init();

    CMU_ClockEnable(cmuClock_GPIO, true);

    /* Setup SysTick Timer for 1 msec interrupts  */
    if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) while (1) ;

    /* Initialize LED driver */
    BSP_LedsInit();
    /* BSP_LedSet(0); */

    /* Setup UART for testing */
    set_recv_callback(&recv_char_cb);
    setup_uart();

    /* Infinite blink loop */
    while (1)
    {
	/* BSP_LedToggle(0); */
	/* BSP_LedToggle(1); */
        Delay(1000);
    }
}



