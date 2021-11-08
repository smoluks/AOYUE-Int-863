#include <string.h>
#include <stdbool.h>
#include "stm32f1xx.h"
#include "modbus.h"
#include "systick.h"
#include "gpio.h"
#include "uart.h"
#include "error.h"

void uart1Init() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN | RCC_APB2ENR_AFIOEN;

    GPIOA->BSRR |= 0x00000600;
    GPIOA->CRH = (GPIOA->CRH & 0xFFFFF00F)
                      | GPIO10_CRH(INPUT_PULL) //rx
                      | GPIO9_CRH(ALT_PUSH_PULL_FAST); //tx

   uart1SetModbusMode();
}

//38400
void uart1SetHC05ConfigMode()
{
    NVIC_DisableIRQ(USART1_IRQn);
    USART1->BRR = 1875;
    USART1->CR2 = 0x00000000;
    USART1->CR3 = 0x00000000;
    USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

//115200 ODD
void uart1SetModbusMode()
{
    USART1->CR1 &= ~USART_CR1_UE;
	USART1->BRR = 625;
    USART1->CR2 = 0x00000000;
    USART1->CR3 = 0x00000000;
    (void) USART1->SR;
    (void) USART1->DR;
    (void) USART1->DR;
	USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_RXNEIE;// | USART_CR1_IDLEIE;
	NVIC_EnableIRQ(USART1_IRQn);
	NVIC_SetPriority(USART1_IRQn, 0);
}

void USART1_IRQHandler() {
    uint16_t sr = USART1->SR;

	//if (sr & USART_SR_IDLE) {
		//modbusIdleHandler();
	//}

    if (sr & USART_SR_PE) {
        setError(ERR_UART_PARITY);
    }
    if (sr & USART_SR_ORE) {
        setError(ERR_UART_OVERRUN);
    }
    if (sr & USART_SR_NE) {
        setError(ERR_UART_NOISE);
    }

	if (sr & (USART_SR_RXNE | USART_SR_ORE)) {
	    modbusRxHandler();
	}

	if (sr & USART_SR_TXE) {
	    modbusTxHandler();
	}
}
