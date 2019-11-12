#include "stm32f1xx.h"

void delay_init()
{
	TIM3->PSC = 72 - 1;
	TIM3->CR1 = TIM_CR1_OPM;
	TIM3->CNT = 0;
	TIM3->SR &= ~TIM_SR_UIF;
	TIM3->EGR |= 0x0001; //для прогрузки PSC
}

void timer_delay(uint16_t uS)
{
	TIM3->ARR = uS;
	//TIM3->CNT = 0;
	TIM3->SR = 0;
	TIM3->CR1 = TIM_CR1_OPM | TIM_CR1_CEN;
	while(!(TIM3->SR & TIM_SR_UIF));
}
