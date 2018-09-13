#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "em_device.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_rtc.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "gpiointerrupt.h"
#include "segmentlcd.h"

void interrupt_test();

#endif
