#ifndef SERIAL_H
#define SERIAL_H

#include "bsp.h"
#include "bsp_trace.h"
#include "segmentlcd.h"
#include "setup.h"
#include "uart.h"

#define LCD_BUFFER_SIZE SEGMENT_LCD_NUM_BLOCK_COLUMNS + 1

enum key_kode {
	ENTER = 13,
	BACKSPACE = 8
};

void init_uart(void);
void uart_test(void);
void recv_char_cb(char c);

#endif
