#include "gpio.h"

void init_GPIO(void)
{
    CMU_ClockEnable(cmuClock_GPIO, true);

    /* Initialize GPIO interrupt dispatcher */
    GPIOINT_Init();

    /* Configure PC11 as input */
    GPIO_PinModeSet(gpioPortC, 11, gpioModeInput, 0);

    /* Set falling edge interrupt for both ports */
    GPIO_IntConfig(gpioPortC, 11, false, true, true);

    /* Register callbacks before setting up and enabling pin interrupt. */
    GPIOINT_CallbackRegister(11, toggle_lidar_cb);
}

