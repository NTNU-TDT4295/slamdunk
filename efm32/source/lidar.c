#include <stddef.h>

#include "lidar.h"

void stop_lidar()
{
	// Stop Lidar scan, this is done before reset to make sure we
	// don't eat too many bytes of data
	uint8_t stop_packet[2];
	stop_packet[0] = 0xA5;
	stop_packet[1] = 0x25;
	uartPutData(1, stop_packet, 2);
	DelayMs(10);
	UART1->CMD = USART_CMD_CLEARRX;
}

void reset_lidar(bool verbose)
{
	// Reset Lidar
	uint8_t reset_packet[2];
	reset_packet[0] = 0xA5;
	reset_packet[1] = 0x40;
	uartPutData(1, reset_packet, 2);

	// Throw away all the garbage mumbo that comes from the lidar
	// on reset
	char recv;
	for (int i = 0; i < 64; ++i) {
		recv = USART_Rx(UART1);
		if (verbose)
			USART_Tx(UART0, recv);
	}
}

void health_lidar(bool verbose)
{
	// Query health of Lidar
	uint8_t health_packet[2];
	health_packet[0] = 0xA5;
	health_packet[1] = 0x52;
	uartPutData(1, health_packet, 2);

	char recv;
	for (int i = 0; i < 10; i++){
		recv = USART_Rx(UART1);
		if (verbose)
			USART_Tx(UART0, recv);
	}
}

void sample_rate_lidar(bool verbose)
{
	// Get sample rate of LIDAR
	uint8_t sample_rate_packet[2];
	sample_rate_packet[0] = 0xA5;
	sample_rate_packet[1] = 0x59;
	uartPutData(1, sample_rate_packet, 2);

	char recv;
	for (int i = 0; i < 11; i++){
		recv = USART_Rx(UART1);
		if (verbose)
			USART_Tx(UART0, recv);
	}
}

void init_scan_lidar(bool verbose)
{
	// Init scan
	DelayMs(10);
	uint8_t scan_packet[2];
	scan_packet[0] = 0xA5;
	scan_packet[1] = 0x20;
	uartPutData(1, scan_packet, 2);

	// Get scan descriptor
	char recv;
	for (int i = 0; i < 7; i++) {
		recv = USART_Rx(UART1);
		if (verbose)
			USART_Tx(UART0, recv);
	}
}

void init_lidar(bool verbose)
{
	stop_lidar();
	reset_lidar(verbose);
	health_lidar(verbose);
}

/* Each sample is 5 bytes */
void get_samples_lidar(uint8_t data[], size_t samples)
{
	init_scan_lidar(false);
	for (size_t i = 0; i < samples*5; ++i) {
		data[i] = USART_Rx(UART1);
	}
	stop_lidar();
}
