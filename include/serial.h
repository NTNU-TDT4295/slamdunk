#ifndef SERIAL_H
#define SERIAL_H

#include "bsp.h"
#include "bsp_trace.h"
#include "setup.h"
#include "uart.h"

void recv_char_cb(char c);
void init_uart(void);
void echo_uart();

#endif
