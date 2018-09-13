#include "lcd.h"

void LCD_test()
{
	/* Enable LCD without voltage boost */
    SegmentLCD_Init(false);

    /* Infinite loop with test pattern. */
    while (1)
    {
        /* Enable all segments */
        SegmentLCD_AllOn();
        Delay(500);

        /* Disable all segments */
        SegmentLCD_AllOff();

        /* Write a number */
        for (int i = 0; i < 10; i++)
        {
            SegmentLCD_Number(i * 1111);
            Delay(200);
        }
        /* Write some text */
        SegmentLCD_Write("Silicon");
        Delay(500);
        SegmentLCD_Write("Labs");
        Delay(500);
        SegmentLCD_Write("Giant");
        Delay(500);
        SegmentLCD_Write("Gecko");
        Delay(1000);

        SegmentLCD_AllOff();

        /* Test segments */
        SegmentLCD_Symbol(LCD_SYMBOL_GECKO, 1);
        SegmentLCD_Symbol(LCD_SYMBOL_ANT, 1);
        SegmentLCD_Symbol(LCD_SYMBOL_PAD0, 1);
        SegmentLCD_Symbol(LCD_SYMBOL_PAD1, 1);
        SegmentLCD_Symbol(LCD_SYMBOL_EFM32, 1);
        SegmentLCD_Symbol(LCD_SYMBOL_MINUS, 1);
        SegmentLCD_Symbol(LCD_SYMBOL_COL3, 1);
        SegmentLCD_Symbol(LCD_SYMBOL_COL5, 1);
        SegmentLCD_Symbol(LCD_SYMBOL_COL10, 1);
        SegmentLCD_Symbol(LCD_SYMBOL_DEGC, 1);
        SegmentLCD_Symbol(LCD_SYMBOL_DEGF, 1);
        SegmentLCD_Symbol(LCD_SYMBOL_DP2, 1);
        SegmentLCD_Symbol(LCD_SYMBOL_DP3, 1);
        SegmentLCD_Symbol(LCD_SYMBOL_DP4, 1);
        SegmentLCD_Symbol(LCD_SYMBOL_DP5, 1);
        SegmentLCD_Symbol(LCD_SYMBOL_DP6, 1);
        SegmentLCD_Symbol(LCD_SYMBOL_DP10, 1);

        SegmentLCD_Battery(0);
        SegmentLCD_Battery(1);
        SegmentLCD_Battery(2);
        SegmentLCD_Battery(3);
        SegmentLCD_Battery(4);

        SegmentLCD_ARing(0, 1);
        SegmentLCD_ARing(1, 1);
        SegmentLCD_ARing(2, 1);
        SegmentLCD_ARing(3, 1);
        SegmentLCD_ARing(4, 1);
        SegmentLCD_ARing(5, 1);
        SegmentLCD_ARing(6, 1);
        SegmentLCD_ARing(7, 1);

        Delay(1000);
    }
}
