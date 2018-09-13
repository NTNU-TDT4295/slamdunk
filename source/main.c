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

    // UART
    init_uart();
    echo_uart();

    // LEDS
    leds_test();

    // LCD
    LCD_test();

    // Interrupts
    interrupt_test();
}
