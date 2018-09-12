//EFM32 blink test

#include "setup.c"
#include "lcd.c"
#include "leds.c"
#include "interrupt.c"

int main(void)
{
    init();

    //leds_test();
    interrupt_test();
    //LCD_test();

}
