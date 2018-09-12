#include "bsp.h"
#include "bsp_trace.h"
#include "setup.h"
void recv_char_cb(char c)
{
	switch (c) {
	case 'a':
		BSP_LedToggle(0);
		BSP_LedToggle(1);
		break;
	default:
		break;
	}
}
void uart_test(void)
{
    /* Setup UART for testing */
    set_recv_callback(&recv_char_cb);
    setup_uart();
}
