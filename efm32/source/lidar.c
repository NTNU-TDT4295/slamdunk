#include <stddef.h>

#include "lidar.h"

bool enable_lidar = false;

USART_TypeDef *uart_channel(unsigned int channel)
{
	if (channel == 0) {
		return UART0;
	} else {
		return UART1;
	}
}

void stop_lidar(unsigned int channel)
{
	// Stop Lidar scan, this is done before reset to make sure we
	// don't eat too many bytes of data
	uint8_t stop_packet[2];
	stop_packet[0] = 0xA5;
	stop_packet[1] = 0x25;
	put_uart_simple(channel, stop_packet, 2);

	DelayMs(1);

	uart_channel(channel)->CMD = USART_CMD_CLEARRX;
	GPIO_PinModeSet(gpioPortF, 8, gpioModePushPull, 0); //motoctl
}

void toggle_lidar_cb(uint8_t pin)
{
	if (!enable_lidar) {
		init_lidar(true, 0);
		init_scan_lidar(true, 0);

		GPIO_PinModeSet(gpioPortC, 9, gpioModePushPull, 1);
		DelayMs(100);
		enable_lidar = true;
	} else {
		stop_lidar(0);

		GPIO_PinModeSet(gpioPortC, 9, gpioModePushPull, 0);
		DelayMs(100);
		enable_lidar = false;
	}
}

void reset_lidar(bool verbose, unsigned int channel)
{
	// Reset Lidar
	uint8_t reset_packet[2];
	reset_packet[0] = 0xA5;
	reset_packet[1] = 0x40;
	put_uart_simple(channel, reset_packet, 2);

	DelayMs(2);

	// Throw away all the garbage mumbo that comes from the lidar
	// on reset -- not according to spec
	uart_channel(channel)->CMD = USART_CMD_CLEARRX;
	char recv;
	for (int i = 0; i < 64; ++i) {
		recv = USART_Rx(uart_channel(channel));
		if (verbose)
			USART_Tx(uart_channel(1), recv);
	}
}

void health_lidar(bool verbose, unsigned int channel)
{
	// Query health of Lidar
	uint8_t health_packet[2];
	health_packet[0] = 0xA5;
	health_packet[1] = 0x52;
	put_uart_simple(channel, health_packet, 2);

	char recv;
	for (int i = 0; i < 10; i++) {
		recv = USART_Rx(uart_channel(channel));
		if (verbose)
			USART_Tx(uart_channel(1), recv);
	}
}

void sample_rate_lidar(bool verbose, unsigned int channel)
{
	// Get sample rate of LIDAR
	uint8_t sample_rate_packet[2];
	sample_rate_packet[0] = 0xA5;
	sample_rate_packet[1] = 0x59;
	put_uart_simple(channel, sample_rate_packet, 2);

	char recv;
	for (int i = 0; i < 11; i++) {
		recv = USART_Rx(uart_channel(channel));
		if (verbose)
			USART_Tx(uart_channel(1), recv);
	}
}

void init_scan_lidar(bool verbose, unsigned int channel)
{
	GPIO_PinModeSet(gpioPortF, 8, gpioModePushPull, 1); //motoctl
	DelayMs(100);

	// Init scan
	uint8_t scan_packet[2];
	scan_packet[0] = 0xA5;
	scan_packet[1] = 0x20;
	put_uart_simple(channel, scan_packet, 2);

	// Get scan descriptor
	char recv;
	for (int i = 0; i < 7; i++) {
		GPIO_PinModeSet(gpioPortC, 8, gpioModePushPull, 1);
		recv = USART_Rx(uart_channel(channel));
		GPIO_PinModeSet(gpioPortC, 8, gpioModePushPull, 0);

		if (verbose)
			USART_Tx(uart_channel(1), recv);
	}
}

void init_lidar(bool verbose, unsigned int channel)
{
	reset_lidar(verbose, channel);
}
