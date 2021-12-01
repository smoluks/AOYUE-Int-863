#include "stm32f1xx.h"
#include "core_cm3.h"
#include "bootloader.h"
#include <stdio.h>
#include <stdbool.h>

typedef volatile unsigned long  vu32;
typedef volatile unsigned short vu16;
typedef volatile unsigned char  vu8;

#define MAIN_APP_ADDR 0x1FFFF000

void JumpToBootloader()
{
    typedef  void (*pFunction)(void);
    pFunction   Jump_To_Application;
    uint32_t   JumpAddress;

    __disable_irq();//��������� ����������

    //GPIO
    GPIOA->CRH = 0;
    GPIOA->CRL = 0;
    GPIOB->CRH = 0;
    GPIOB->CRL = 0;
    GPIOC->CRH = 0;
    GPIOC->CRL = 0;

    //��������� ��� ���������� � NVIC
    NVIC->ICER[0] = 0xFFFFFFFF;
    NVIC->ICER[1] = 0xFFFFFFFF;
    NVIC->ICER[2] = 0xFFFFFFFF;

    //������� ��� pending bit
    NVIC->ICPR[0] = 0xFFFFFFFF;
    NVIC->ICPR[1] = 0xFFFFFFFF;
    NVIC->ICPR[2] = 0xFFFFFFFF;

    //Peripheral risit enable (�� ������� reserved bits)
    RCC->APB1RSTR = 0x3E7EC83F;
    RCC->APB2RSTR = 0x00005E7D;
    //Peripheral  risit disable
    RCC->APB1RSTR = 0;
    RCC->APB2RSTR = 0;
    //Peripheral  disable clock
    RCC->APB1ENR = 0;
    RCC->APB2ENR = 0;

    //Systysk
    SysTick->CTRL = 0;
    SysTick->VAL = 0;

    //
    RCC->CFGR = 0;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);

    JumpAddress = *(uint32_t*)(MAIN_APP_ADDR + 4);
    Jump_To_Application = (pFunction) JumpAddress;

    //NVIC_SetVectorTable( MAIN_APP_ADDR, 0 );

    __set_MSP(*(vu32*) MAIN_APP_ADDR);
    Jump_To_Application();
}
