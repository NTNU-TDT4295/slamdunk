//EFM32 blink test

#include "setup.c"
#include "lcd.c"
#include "leds.c"

int main(void)
{
    init();

    LCD_test();
    leds_test();

}
