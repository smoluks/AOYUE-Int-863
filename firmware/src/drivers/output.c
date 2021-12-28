#include "stm32f1xx.h"
#include "sensors.h"
#include "gpio.h"
#include "logic.h"
#include "hardwareConfig.h"
#include "phaseTable.h"
#include "systick.h"
#include "output.h"
#include "beeper.h"
#include "config.h"

#define OUT1_ON() GPIOB->BSRR = BSRR_SET(BIT5)
#define OUT1_OFF() GPIOB->BSRR = BSRR_RESET(BIT5)

#define OUT2_ON() GPIOB->BSRR = BSRR_RESET(BIT4)
#define OUT2_OFF() GPIOB->BSRR = BSRR_SET(BIT4)

#define BOTH_OFF() GPIOB->BSRR = BSRR_SET(BIT4) | BSRR_RESET(BIT5)

uint16_t channel1 = 0;
uint16_t channel2 = 0;

extern config_s config;

// cross-zero logic
void crossZeroInit() {
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN;

    GPIOB->BSRR = BSRR_SET(BIT9);
    GPIOB->CRH = (GPIOB->CRH & 0xFFFFFF0F) | GPIO9_CRH(INPUT_PULL);
    GPIOB->CRL = (GPIOB->CRL & 0xFF00FFFF) | GPIO5_CRL(GENERAL_PUSH_PULL_FAST)  //ch1
            | GPIO4_CRL(GENERAL_PUSH_PULL_FAST); //ch2

    //eint
    AFIO->EXTICR[2] = AFIO_EXTICR3_EXTI9_PB; //portb
    EXTI->IMR |= 1 << 9;
    EXTI->RTSR |= 1 << 9;
    EXTI->FTSR |= 1 << 9;
    NVIC_SetPriority(EXTI9_5_IRQn, 1);
    NVIC_EnableIRQ(EXTI9_5_IRQn);

    //ch1 timer - 3,6MHz - 36000 tick per phase
    TIM1->PSC = 20 - 1;
    TIM1->CR1 = TIM_CR1_OPM;
    TIM1->ARR = 36000;
    TIM1->EGR |= TIM_EGR_UG;
    delay_us(1);
    TIM1->SR &= ~(TIM_SR_UIF);
    TIM1->DIER = TIM_DIER_UIE;
    NVIC_SetPriority(TIM1_UP_IRQn, 1);
    NVIC_EnableIRQ(TIM1_UP_IRQn);

    //ch2 timer - 3,6MHz - 36000 tick per phase
    TIM2->PSC = 20 - 1;
    TIM2->CR1 = TIM_CR1_OPM;
    TIM2->ARR = 36000;
    TIM2->EGR |= TIM_EGR_UG;
    delay_us(1);
    TIM2->SR &= ~(TIM_SR_UIF);
    TIM2->DIER = TIM_DIER_UIE;
    NVIC_SetPriority(TIM2_IRQn, 1);
    NVIC_EnableIRQ(TIM2_IRQn);

    //calc timer - 9ms
    TIM3->SR &= ~(TIM_SR_UIF);
    TIM3->DIER = TIM_DIER_UIE;
    TIM3->PSC = 72 - 1;
    TIM3->ARR = 9000;
    TIM3->CR1 = TIM_CR1_ARPE | TIM_CR1_OPM;
    NVIC_SetPriority(TIM3_IRQn, 1);
    NVIC_EnableIRQ(TIM3_IRQn);
}

inline void emergencyDisableAll()
{
    NVIC_DisableIRQ(EXTI9_5_IRQn);
    BOTH_OFF();
    beep();
}

//cross-zero
static volatile bool processed = true;
static uint32_t timestamp;
static uint32_t freq;
void EXTI9_5_IRQHandler() {
    EXTI->PR |= EXTI_PR_PR9;

    freq = getSystime() - timestamp;
    timestamp = getSystime();

    config.targets_temperature[1] = 0xC000 | (freq << 4);

    BOTH_OFF();

    //if old cz not processed - error
#ifdef DEBUG_MODE
    if (!processed) {
        setError(ERR_POWER_FREQ_HIGH);
    }
#endif

    //---first---
    TIM1->CR1 &= ~TIM_CR1_CEN;
    TIM1->CNT = 0;
    if (channel1 >= 1024)
    {
        OUT1_ON();
    } else if (channel1 > 0)
    {
        //start on timer
        TIM1->SR &= ~TIM_SR_UIF;
        TIM1->ARR = phaseTable[channel1];
        TIM1->CR1 |= TIM_CR1_CEN;
    }

    //---second---
    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM2->CNT = 0;
    if (channel2 >= 1024)
    {
        OUT2_ON();
    } else if (channel2 > 0)
    {
        //start on timer
        TIM2->SR &= ~TIM_SR_UIF;
        TIM2->ARR = phaseTable[channel2];
        TIM2->CR1 |= TIM_CR1_CEN;
    }

    //---calc trigger---
    TIM3->CNT = 0;
    TIM3->CR1 |= TIM_CR1_CEN;

    processed = false;
}

//channel1 en
void TIM1_UP_IRQHandler() {
    TIM1->SR &= ~TIM_SR_UIF;

    OUT1_ON();
}

//channel2 en
void TIM2_IRQHandler() {
    TIM2->SR &= ~TIM_SR_UIF;

    OUT2_ON();
}

void TIM3_IRQHandler() {
    TIM3->SR &= ~TIM_SR_UIF;

    calculateOutput();

    processed = true;
}
