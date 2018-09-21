#include <stdint.h>
#include <stdbool.h>
#include "em_chip.h"
#include "em_cmu.h"
#include "em_timer.h"
#include "em_device.h"
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

void DelayMs(int ms)
{
    uint32_t endValue = ms * RTC_FREQ / 1000;
    RTC->CNT = 0;

    RTC->CTRL |= RTC_CTRL_EN;
    while (RTC->CNT < endValue);
    RTC->CTRL &= ~RTC_CTRL_EN;
}

void DelayUs(int us)
{
    uint32_t endValue = us * (TIMER_FREQ / 1000000);
    TIMER0->CNT = 0;

    TIMER0->CMD = TIMER_CMD_START;
    while (TIMER0->CNT < endValue);
    TIMER0->CMD = TIMER_CMD_STOP;
}

/*
 * Peforms general init functions
 */
void init(void)
{
    CHIP_Init();

    CMU_ClockEnable(cmuClock_CORELE, true);
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
    CMU_ClockEnable(cmuClock_RTC, true);
    CMU_ClockEnable(cmuClock_TIMER0, true);
    CMU_ClockEnable(cmuClock_GPIO, true);

    /* Setup SysTick Timer for 1 msec interrupts  */
    if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) while (1) ;
}
