#include "leds.h"

void leds_init()
{
    BSP_LedsInit();
    BSP_LedSet(1);
}

void leds_test()
{
    leds_init();

    /* Infinite blink loop */
    while (1)
    {
        BSP_LedToggle(0);
        BSP_LedToggle(1);
        Delay(1000);
    }
}
