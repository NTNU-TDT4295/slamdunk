//EFM32 blink test

#include "setup.c"
#include "lcd.c"
#include "leds.c"
#include "interrupt.c"
#include "serial.h"
#include <string.h>

int main(void)
{
    init();

    /* leds_test(); */

    /* BSP_LedsInit(); */
    /* BSP_LedSet(1); */

    /* for(;;); */

    /* for(;;); */

    uart_test();

    char tmp;

    for (;;) {
        /* uartPutChar('a'); */
        /* Delay(50); */

        tmp = uartGetChar();

        if (tmp) {
            BSP_LedToggle(0);
            uartPutChar(tmp + 2);
        }
    }
}
