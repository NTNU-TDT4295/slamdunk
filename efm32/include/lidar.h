#ifndef LIDAR_H
#define LIDAR_H

#include "setup.h"
#include "serial.h"

USART_TypeDef* getUart_input(unsigned int uart_channel);
USART_TypeDef* getUart_output(unsigned int uart_channel);
void stop_lidar(unsigned int uart_channel);
void reset_lidar(bool verbose, unsigned int uart_channel);
void health_lidar(bool verbose, unsigned int uart_channel);
void sample_rate_lidar(bool verbose, unsigned int uart_channel);
void init_scan_lidar(bool verbose, unsigned int uart_channel);
void init_lidar(bool verbose, unsigned int uart_channel);
void get_samples_lidar(uint8_t data[], size_t samples, unsigned int uart_channel);
void toggle_lidar_cb(uint8_t pin);

#endif
