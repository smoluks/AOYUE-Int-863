#include <stdint.h>
#include <stdbool.h>
#include "systick.h"

volatile uint32_t systime = 0;

void systickInit()
{
	SysTick->LOAD = 72000000UL / 1000 - 1;
	SysTick->VAL = 72000000UL / 1000 - 1;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

//SysTick Interrupt
void SysTick_Handler(void)
{
	systime++;
}

uint32_t getSystime()
{
	return systime;
}

bool isTimeout(uint32_t timestamp, uint32_t delay)
{
	uint32_t delta = systime - timestamp;
	if(delta & 0x80000000)
		return false;

	return delta >= delay;
}

void delay(uint32_t delay)
{
	volatile uint32_t timestamp = systime;

	while(!isTimeout(timestamp, delay)){
		WDT_RESET();
	}
}
