#include <stddef.h>

#include "lidar.h"

USART_TypeDef* getUart_input(unsigned int uart_channel)
{
    if (uart_channel == 0){
        return UART0;
    } else {
        return UART1;
    }
}

USART_TypeDef* getUart_output(unsigned int uart_channel)
{
    if (uart_channel == 0){
        return UART1;
    } else {
        return UART0;
    }
}

void stop_lidar(unsigned int uart_channel)
{
	// Stop Lidar scan, this is done before reset to make sure we
	// don't eat too many bytes of data
	uint8_t stop_packet[2];
	stop_packet[0] = 0xA5;
	stop_packet[1] = 0x25;
	uartPutData(uart_channel, stop_packet, 2);
	DelayMs(10);
	getUart_input(uart_channel)->CMD = USART_CMD_CLEARRX;
}

void reset_lidar(bool verbose, unsigned int uart_channel)
{
	// Reset Lidar
	uint8_t reset_packet[2];
	reset_packet[0] = 0xA5;
	reset_packet[1] = 0x40;
	uartPutData(uart_channel, reset_packet, 2);

	// Throw away all the garbage mumbo that comes from the lidar
	// on reset
	char recv;
	for (int i = 0; i < 64; ++i) {
		recv = USART_Rx(getUart_input(uart_channel));
		if (verbose)
			USART_Tx(getUart_output(uart_channel), recv);
	}
}

void health_lidar(bool verbose, unsigned int uart_channel)
{
	// Query health of Lidar
	uint8_t health_packet[2];
	health_packet[0] = 0xA5;
	health_packet[1] = 0x52;
	uartPutData(uart_channel, health_packet, 2);

	char recv;
	for (int i = 0; i < 10; i++){
		recv = USART_Rx(getUart_input(uart_channel));
		if (verbose)
			USART_Tx(getUart_output(uart_channel), recv);
	}
}

void sample_rate_lidar(bool verbose, unsigned int uart_channel)
{
	// Get sample rate of LIDAR
	uint8_t sample_rate_packet[2];
	sample_rate_packet[0] = 0xA5;
	sample_rate_packet[1] = 0x59;
	uartPutData(uart_channel, sample_rate_packet, 2);

	char recv;
	for (int i = 0; i < 11; i++){
		recv = USART_Rx(getUart_input(uart_channel));
		if (verbose)
			USART_Tx(getUart_output(uart_channel), recv);
	}
}

void init_scan_lidar(bool verbose, unsigned int uart_channel)
{
	// Init scan
	DelayMs(10);
	uint8_t scan_packet[2];
	scan_packet[0] = 0xA5;
	scan_packet[1] = 0x20;
	uartPutData(uart_channel, scan_packet, 2);

	// Get scan descriptor
	char recv;
	for (int i = 0; i < 7; i++) {
		recv = USART_Rx(getUart_input(uart_channel));
		if (verbose)
			USART_Tx(getUart_output(uart_channel), recv);
	}
}

void init_lidar(bool verbose, unsigned int uart_channel)
{
	GPIO_PinModeSet(gpioPortF, 8, gpioModePushPull, 1); //motoctl
	stop_lidar(uart_channel);
	reset_lidar(verbose, uart_channel);
	health_lidar(verbose, uart_channel);
}

/* Each sample is 5 bytes */
void get_samples_lidar(uint8_t data[], size_t samples, unsigned int uart_channel)
{
	init_scan_lidar(false, uart_channel);
	for (size_t i = 0; i < samples*5; ++i) {
		data[i] = USART_Rx(getUart_input(uart_channel));
	}
	stop_lidar(uart_channel);
}
