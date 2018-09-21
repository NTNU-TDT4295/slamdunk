#ifndef SONAR_H
#define SONAR_H

#include "em_device.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_rtc.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "gpiointerrupt.h"

void sonar_callback(uint8_t pin);
void init_sonar();
void trigger_sonar();

#endif
