#include <stdint.h>
#include <stdbool.h>
#include "em_chip.h"
#include "em_cmu.h"
#include "setup.h"

volatile uint32_t msTicks; /* counts 1ms timeTicks */

/*
 * @brief Interrupt Service Routine for system tick counter
 */
void SysTick_Handler(void)
{
    msTicks++;   /* increment counter necessary in Delay()*/
}

/*
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 */
void Delay(uint32_t dlyTicks)
{
    uint32_t curTicks;

    curTicks = msTicks;
    while ((msTicks - curTicks) < dlyTicks) ;
}

/*
 * Peforms general init functions
 */
void init(void)
{
    CHIP_Init();

    CMU_ClockEnable(cmuClock_GPIO, true);

    /* Setup SysTick Timer for 1 msec interrupts  */
    if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) while (1) ;
}
