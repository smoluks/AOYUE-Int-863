#include "stm32f1xx.h"
#include "sensors.h"
#include "gpio.h"
#include "logic.h"
#include "hardwareConfig.h"

void crossZeroInit() {
    // cross-zero
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    GPIOB->BSRR = BSRR_SET(BIT9);
    GPIOB->CRH = (GPIOB->CRH & 0xFFFFFF0F) | GPIO9_CRH(INPUT_PULL);

    AFIO->EXTICR[2] = AFIO_EXTICR3_EXTI9_PB; //portb
    EXTI->IMR = 0x00000200;
    EXTI->RTSR = 0x00000200;
    EXTI->FTSR = 0x00000200;
    NVIC_EnableIRQ(EXTI9_5_IRQn);

    //delay timer - 9ms
    TIM3->SR &= ~(TIM_SR_UIF);
    TIM3->DIER = TIM_DIER_UIE;
    TIM3->PSC = 72 - 1;
    TIM3->ARR = 9900;
    TIM3->CR1 = TIM_CR1_ARPE | TIM_CR1_OPM;
    NVIC_EnableIRQ(TIM3_IRQn);
}

static volatile bool processed = true;
void EXTI9_5_IRQHandler() {
    EXTI->PR |= EXTI_PR_PR9;

    TIM3->CNT = 0;
    TIM3->CR1 |= TIM_CR1_CEN;

    //if old cz not processed - error
#ifdef DEBUG_MODE
    if (!processed) {
        setError(ERR_POWER_FREQ_HIGH);
    }
#endif

    processed = false;
}

void TIM3_IRQHandler() {
    TIM3->SR &= ~TIM_SR_UIF;

    MakeOutput();

    processed = true;
}
