#include "bsp.h"
#include "bsp_trace.h"
#include "setup.h"
#include "uart.h"

void recv_char_cb(char c)
{
    switch (c) {
    case 'q':
        return;
    default:
        uartPutChar(c);
        break;
    }
}

void init_uart(void)
{
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
