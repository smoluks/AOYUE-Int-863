#include "stm32f1xx.h"
#include "stdbool.h"
#include "config.h"
#include "adc.h"
#include "pins.h"
#include "pid.h"
#include "regul.h"

extern struct ramconfig_s ramconfig;

int16_t level = 0; //уровень нагревателя (max 100)
bool commandfromuart = false;

//прерывание по cross-zero
void EXTI4_IRQHandler(void)
{
	//управление
	if (level > 0)
		GPIOB->BSRR = BSRR_SET(GPIOB_LED_RED) | BSRR_SET(GPIOB_OUT1);
	else
		GPIOB->BSRR = BSRR_RESET(GPIOB_LED_RED) | BSRR_RESET(GPIOB_OUT1);

	TIM4->CR1 |= TIM_CR1_CEN; //запуск таймера задержки

	EXTI->PR |= EXTI_PR_PR4;
}

void TIM4_IRQHandler(void)
{
	adc_start(); //запуск АЦП
	TIM4->SR &= ~TIM_SR_UIF;
}

//вызывается по окончанию оцифровки
void ADC_Complete()
{
	if(ramconfig.heater_targets[0].target_mode == Off)
		level = 0;

	else if (ramconfig.heater_targets[0].target_mode == Heat)
	{
		uint16_t temp;
		int16_t dt;
		bool found = false;
		bool first = true;
		for (int i = 0; i < 5; i++) {
			if (ramconfig.heater_targets[0].target_by_sensor[i]) //если этот канал задан
			{
				found = true;
				if (first || ((ramconfig.heater_targets[0].target_by_sensor[i] - ramconfig.temperatures[i]) < dt)) //если ошибка меньше текущей
				{
					first = false;
					//переключаемся на этот канал
					dt = ramconfig.heater_targets[0].target_by_sensor[i] - ramconfig.temperatures[i];
					temp = ramconfig.temperatures[i];
				}
			}
		}

		if(found)
			level = PID(dt, temp);
		else
			level = 0;
	}
}

bool regul_Set_Targets(struct heater_target_s targets[])
{
	for(int i = 0; i<channel_count; i++)
	{
		regul_Set_Target(&targets[i], i);
	}
	return true;
}

bool regul_Set_Target(struct heater_target_s *target, uint8_t channel)
{
	if(channel || target->target_mode > 2)
		return false;

	ramconfig.heater_targets[channel] = *target;
	commandfromuart = (target->target_mode != Off);

	return true;
}

int process_regul(bool connected)
{
	if(commandfromuart && !connected)
		ramconfig.heater_targets[0].target_mode = Off;

	return 0;
}

void regul_stop()
{
	GPIOB->BSRR = BSRR_RESET(GPIOB_LED_RED) | BSRR_RESET(GPIOB_OUT1);
	ramconfig.heater_targets[0].target_mode = Off;
}


