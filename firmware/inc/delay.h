#pragma once


#define uStime 9

#define delay(us) \
	({  \
	uint32_t i = us * uStime;  \
	while ((i--) != 0);  \
	})

void delay_init();
void timer_delay(uint16_t uS);


