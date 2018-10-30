#include "interrupt.h"

#define RTC_FREQ    32768

/*
 * @brief  Gpio setup. Setup button pins to trigger falling edge interrupts.
 *  Register callbacks for that interrupts.
 */
void init_GPIO(void)
{
    /* Initialize GPIO interrupt dispatcher */
    GPIOINT_Init();

    /* Configure PC11 as input */
    GPIO_PinModeSet(gpioPortC, 11, gpioModeInput, 0);

    /* Set falling edge interrupt for both ports */
    GPIO_IntConfig(gpioPortC, 11, false, true, true);

    /* Register callbacks before setting up and enabling pin interrupt. */
	GPIOINT_CallbackRegister(11, toggle_lidar_cb);

}

/*
 * @brief Enables LFACLK and selects LFXO as clock source for RTC
 *        Sets up the RTC to generate an interrupt every minute.
 */
void rtcSetup(void)
{
    /* Enable LE domain registers */
    CMU_ClockEnable(cmuClock_CORELE, true);

    /* Enable LFXO as LFACLK in CMU. This will also start LFXO */
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);

    /* Set a clock divisor of 32 to reduce power conumption. */
    CMU_ClockDivSet(cmuClock_RTC, cmuClkDiv_32);

    /* Enable RTC clock */
    CMU_ClockEnable(cmuClock_RTC, true);

    /* Initialize RTC */
    RTC_Init_TypeDef rtcInit = RTC_INIT_DEFAULT;

    rtcInit.enable   = false;  /* Do not start RTC after initialization is complete. */
    rtcInit.debugRun = false;  /* Halt RTC when debugging. */
    rtcInit.comp0Top = true;   /* Wrap around on COMP0 match. */
    RTC_Init(&rtcInit);

    /* Interrupt every minute */
    //RTC_CompareSet(0, ((RTC_FREQ / 32) * 60) - 1 );
    /* Interrupt every second */
    RTC_CompareSet(0, (RTC_FREQ / 32) - 1 );

    /* Enable interrupt */
    NVIC_EnableIRQ(RTC_IRQn);
    RTC_IntEnable(RTC_IEN_COMP0);

    /* Start Counter */
    RTC_Enable(true);
}

void interrupt_test()
{
    /* Ensure core frequency has been updated */
    SystemCoreClockUpdate();

    /* Init LCD with no voltage boost */
    SegmentLCD_Init(false);

    /* Setup RTC to generate an interrupt every minute */
    rtcSetup();

    /* Setup GPIO with interrupts to serve the pushbuttons */
    gpioSetup();
}
