#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "stm32f1xx.h"

#define WDT_RESET() IWDG->KR=0xAAAA

#define uStime 9

#define delay_us(us) \
	{	\
	uint32_t i = us * uStime;   \
	while ((i--) != 0);  		\
	}

void systickInit();
uint32_t getSystime();
bool isTimeout(uint32_t timestamp, uint32_t delay);
void delay(uint32_t delay);
