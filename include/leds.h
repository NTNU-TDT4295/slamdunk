#ifndef LEDS_H
#define LEDS_H

#include "setup.h"
#include "bsp.h"
#include "bsp_trace.h"

#ifndef LED_PIN
#define LED_PIN     4
#endif
#ifndef LED_PORT
#define LED_PORT    gpioPortA
#endif

void leds_init();
void leds_test();

#endif
