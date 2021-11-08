#pragma once

#include "stm32f1xx.h"

#define BIT0 1<<0
#define BIT1 1<<1
#define BIT2 1<<2
#define BIT3 1<<3
#define BIT4 1<<4
#define BIT5 1<<5
#define BIT6 1<<6
#define BIT7 1<<7
#define BIT8 1<<8
#define BIT9 1<<9
#define BIT10 1<<10
#define BIT11 1<<11
#define BIT12 1<<12
#define BIT13 1<<13
#define BIT14 1<<14
#define BIT15 1<<15



#define GPIOB_LED_RED       1 << 7
#define GPIOB_LED_GREEN     1 << 9
#define GPIOB_BTLED2 		1 << 11

#define GPIOB_OUT1		  	1 << 13
#define GPIOB_OUT2  		1 << 14
#define GPIOB_USB_EN 		1 << 15

#define GPIOC_BUTTON_UP		1 << 15
#define GPIOC_BUTTON_MENU	1 << 14
#define GPIOC_BUTTON_DOWN	1 << 13

#define BSRR_SET(i) i
#define BSRR_RESET(i) (i << 16)

//gpio modes
#define GENERAL_PUSH_PULL_FAST 0x3
#define GENERAL_OPEN_DRAIN_FAST 0x7
#define ALT_PUSH_PULL_FAST 0xB
#define ALT_OPEN_DRAIN_FAST 0xF
#define ANALOG 0
#define INPUT_FLOATING 0x4
#define INPUT_PULL 0x8

//gpio offsets
#define GPIO0_CRL(x) (x << (0 * 4))
#define GPIO1_CRL(x) (x << (1 * 4))
#define GPIO2_CRL(x) (x << (2 * 4))
#define GPIO3_CRL(x) (x << (3 * 4))
#define GPIO4_CRL(x) (x << (4 * 4))
#define GPIO5_CRL(x) (x << (5 * 4))
#define GPIO6_CRL(x) (x << (6 * 4))
#define GPIO7_CRL(x) (x << (7 * 4))
#define GPIO8_CRH(x) (x << (0 * 4))
#define GPIO9_CRH(x) (x << (1 * 4))
#define GPIO10_CRH(x) (x << (2 * 4))
#define GPIO11_CRH(x) (x << (3 * 4))
#define GPIO12_CRH(x) (x << (4 * 4))
#define GPIO13_CRH(x) (x << (5 * 4))
#define GPIO14_CRH(x) (x << (6 * 4))
#define GPIO15_CRH(x) (x << (7 * 4))

#define LED_SETRED() GPIOB->BSRR = BSRR_RESET(GPIOB_LED_GREEN) | BSRR_SET(GPIOB_LED_RED)
#define LED_SETGREEN() GPIOB->BSRR = BSRR_SET(GPIOB_LED_GREEN) | BSRR_RESET(GPIOB_LED_RED)


#define IS_BUTTON_UP_PRESSED() (!(GPIOC->IDR & GPIOC_BUTTON_UP))
#define IS_BUTTON_MENU_PRESSED() (!(GPIOC->IDR & GPIOC_BUTTON_MENU))
#define IS_BUTTON_DOWN_PRESSED() (!(GPIOC->IDR & GPIOC_BUTTON_DOWN))

#define OUT1_ON() GPIOB->BSRR = BSRR_SET(GPIOB_OUT1)
#define OUT1_OFF() GPIOB->BSRR = BSRR_RESET(GPIOB_OUT1)

void setIOBits(GPIO_TypeDef* port, uint16_t mask);
void resetIOBits(GPIO_TypeDef* port, uint16_t mask);
