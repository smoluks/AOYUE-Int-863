#include "pins.h"
#include "lcd.h"
#include "stm32f1xx.h"
#include "stm32f1xx_it.h"

void NMI_Handler(void)
{
	GPIOB->BSRR = BSRR_SET(GPIOB_LED_RED) | BSRR_RESET(GPIOB_LED_GREEN) | BSRR_RESET(GPIOB_OUT1);
	display_write_text("NMI", 0);
	while (1);
}

void HardFault_Handler(void)
{
	GPIOB->BSRR = BSRR_SET(GPIOB_LED_RED) | BSRR_RESET(GPIOB_LED_GREEN) | BSRR_RESET(GPIOB_OUT1);
	display_write_text("HardFault", 0);
	while (1);
}

void MemManage_Handler(void)
{
	GPIOB->BSRR = BSRR_SET(GPIOB_LED_RED) | BSRR_RESET(GPIOB_LED_GREEN) | BSRR_RESET(GPIOB_OUT1);
	display_write_text("MemManage", 0);
	while (1);
}

void BusFault_Handler(void)
{
	GPIOB->BSRR = BSRR_SET(GPIOB_LED_RED) | BSRR_RESET(GPIOB_LED_GREEN) | BSRR_RESET(GPIOB_OUT1);
	display_write_text("BusFault", 0);
	while (1);
}

void UsageFault_Handler(void)
{
	GPIOB->BSRR = BSRR_SET(GPIOB_LED_RED) | BSRR_RESET(GPIOB_LED_GREEN) | BSRR_RESET(GPIOB_OUT1);
	display_write_text("UsageFault", 0);
	while (1);
}


void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}


void PendSV_Handler(void)
{
}

