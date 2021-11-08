#include "stm32f1xx.h"
#include "sensors.h"
#include "gpio.h"
#include "pid.h"

static uint8_t cycle;
static uint8_t value;

static uint16_t oneweight;
static uint16_t zeroweight;

// cross-zero
void EXTI4_IRQHandler()
{
	if(value >= 100)
		OUT1_ON();
	else if(value == 0)
		OUT1_OFF();
	else
	{
		if(oneweight <= zeroweight)
		{
			//enable
			OUT1_ON();
			oneweight += (100 - value);
		}
		else
		{
			//disable
			OUT1_OFF();
			zeroweight += value;
		}
	}

	cycle++;
	if(cycle == 100)
	{
		cycle = 0;
		zeroweight = 0;
		oneweight = 0;
		value = calculateNext();
	}

	EXTI->PR |= EXTI_PR_PR4;
}

