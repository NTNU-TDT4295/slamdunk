#ifndef LED_PIN
#define LED_PIN     4
#endif
#ifndef LED_PORT
#define LED_PORT    gpioPortA
#endif

#include "setup.h"
#include "bsp.h"
#include "bsp_trace.h"

void leds_test(void)
{
    /* Initialize LED driver */
    BSP_LedsInit();
    BSP_LedSet(1);

    /* Infinite blink loop */
    while (1)
    {
        BSP_LedToggle(0);
        BSP_LedToggle(1);
        Delay(1000);
    }
}
