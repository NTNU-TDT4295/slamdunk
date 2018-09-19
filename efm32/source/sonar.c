#include "sonar.h"
#include "lcd.h"
#include "setup.h"
#include <stdio.h>

void sonar_callback(uint8_t pin)
{
	// Time the amount of microseconds the pin is high
	uint32_t microsecond = TIMER_FREQ / 1000000;
	uint32_t elapsed = 0;

	TIMER0->CNT = 0;
	TIMER0->CMD = TIMER_CMD_START;
	while (GPIO_PinInGet(gpioPortD, 2));
	TIMER0->CMD = TIMER_CMD_STOP;
	elapsed = TIMER0->CNT;

	char str[8];
	snprintf(str, 8, "%d", (int) elapsed);
	SegmentLCD_Write(str);
}

void init_sonar()
{
	// Interrupt dispatcher
	GPIOINT_Init();

	// PD2 - echo, PD3 - trig
	GPIO_PinModeSet(gpioPortD, 2, gpioModeInput, 0);
	GPIO_PinModeSet(gpioPortD, 3, gpioModePushPull, 0);

	// Port, pin, rising edge, falling edge, enable
	GPIO_IntConfig(gpioPortD, 2, true, false, true);

	// IRQ handlers are defined in gpiointerrupt.c to dispatch
	// callbacks so that we don't scatter definitions
	GPIOINT_CallbackRegister(2, sonar_callback);
}

void trigger_sonar()
{
	GPIO_PinOutClear(gpioPortD, 3);
	DelayUs(2);
	GPIO_PinOutSet(gpioPortD, 3);
	DelayUs(10);
	GPIO_PinOutClear(gpioPortD, 3);

	// Check echo -- this is interrupt driven for now (TODO)
}
