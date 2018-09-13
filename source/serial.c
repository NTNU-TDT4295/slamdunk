#include <stdio.h>

#include "serial.h"

void recv_char_cb(char c)
{
	static char str[LCD_BUFFER_SIZE];
	static int idx = 0;

	switch (c) {
	case BACKSPACE:
		if (idx != 0) {
			str[--idx] = 0;
		}
		break;
	case ENTER:
		for (size_t i = 0; i < LCD_BUFFER_SIZE; ++i) {
			str[i] = 0;
			idx = 0;
		}
		break;
    default:
        uartPutChar(c);
        break;
	}

	if ('a' <= c && c <= 'z') {
		str[idx] = c - 32;
		idx = idx == SEGMENT_LCD_NUM_BLOCK_COLUMNS
			? SEGMENT_LCD_NUM_BLOCK_COLUMNS
			: idx + 1;
	}

	SegmentLCD_Write(str);
}

void init_uart(void)
{
	/* LCD for proof of concept, remove later */
	SegmentLCD_Init(false);

    /* Setup UART for testing */
    set_recv_callback(&recv_char_cb);
    setup_uart();
}

void echo_uart()
{
    char rx_data;
    /* Echo RX to TX indefinitely */
    for (;;) {
        rx_data = uartGetChar();

        if (rx_data) {
            uartPutChar(rx_data);
        }
    }
}
