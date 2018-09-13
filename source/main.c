//EFM32 blink test

#include "setup.h"
#include "leds.h"
#include "lcd.h"
#include "interrupt.h"
#include "serial.h"
#include <string.h>

int main(void)
{
    init();

    // Interrupts (works with LCD)
    /* interrupt_test(); */

    // UART
    init_uart();

    // LEDS, (disabled for now, as they collide with UART)
    /* leds_test(); */

    for (;;);
}
