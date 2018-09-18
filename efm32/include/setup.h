#ifndef SETUP_H
#define SETUP_H

#include <stdint.h>
#include <stdbool.h>
#include "em_chip.h"
#include "em_cmu.h"

// counter for Delay ticks to sleep
volatile uint32_t msTicks;
void Delay(uint32_t dlyTicks);
void SysTick_Handler(void);

void init(void);

#endif
