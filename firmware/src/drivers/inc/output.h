#pragma once

void crossZeroInit();
void emergencyDisableAll();

void EXTI9_5_IRQHandler();
void TIM1_IRQHandler();
void TIM2_IRQHandler();
void TIM3_IRQHandler();

typedef struct
{
    uint16_t channel1;
    uint16_t channel2;
} output_s;
