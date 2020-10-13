#include <string.h>
#include <stdbool.h>
#include "stm32f1xx.h"
#include "modbus.h"
#include "systick.h"

static uint32_t timestamp;

void uart2Init() {
	GPIOA->CRL |= 0x00008BB8;
	GPIOA->BSRR = 0x00000009;
	//38400
	USART2->BRR = 938;
	USART2->CR2 = 0x00000000;
	USART2->CR3 = 0x00000000;
	USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

void uart2GotoModbusMode() {
	//115200 ODD
	USART2->BRR = 313;
	USART2->CR1 |= USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_RXNEIE | USART_CR1_TCIE | USART_CR1_IDLEIE;
	NVIC_EnableIRQ(USART2_IRQn);
}

bool uart2SendAtCommand(char *command, char *buffer) {
	//clean old
	(void) USART2->DR;
	(void) USART2->DR;

	//send command
	do {
		char c = *command++;
		if (!c)
		{
			while (!(USART2->SR & USART_SR_TXE));
			USART2->DR = 0x0D;

			while (!(USART2->SR & USART_SR_TXE));
			USART2->DR = 0x0A;

			break;
		}

		while (!(USART2->SR & USART_SR_TXE));
		USART2->DR = c;

	} while (1);

	//read
	timestamp = getSystime();
	do {
		while (!(USART2->SR & USART_SR_RXNE) && !isTimeout(timestamp, 5000))
		{
			WDT_RESET();
		}

		if(isTimeout(timestamp, 5000))
			return false;

		char c = USART2->DR;
		if (c == 0x0D)
			break;

		if(c != 0x0A)
			*buffer++ = c;

	} while(1);

	*buffer = 0;

	return true;
}

void USART2_IRQHandler() {
	if (USART2->SR & USART_SR_IDLE) {
		modbusIdleHandler();
	}

	if (USART2->SR & (USART_SR_RXNE | USART_SR_ORE)) {
		modbusRxHandler();
	}

	if (USART2->SR & USART_SR_TC) {
		modbusTcHandler();
	}

	USART2->SR = 0;
}
