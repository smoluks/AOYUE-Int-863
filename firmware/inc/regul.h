#pragma once

#include <stdbool.h>

void EXTI4_IRQHandler(void);
void TIM4_IRQHandler(void);
void ADC_Complete();

int process_regul(bool connected);
bool regul_Set_Targets(struct heater_target_s targets[]);
bool regul_Set_Target(struct heater_target_s *target, uint8_t channel);
void regul_stop();
