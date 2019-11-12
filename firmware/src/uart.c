#include "stm32f1xx.h"
#include "command.h"
#include "error.h"
#include <stdbool.h>

bool transmit = false;

void uart_read_process(uint8_t data);
void uart_write_process();

enum read_state_enum
{
	RMARKER, RLEN, RPACKET, RCRC
} read_state = RMARKER;

void USART2_IRQHandler()
{
	if (USART2->SR & USART_SR_ORE)
	{
		write_error(ERR_OVERRUN);
	}

	if (USART2->SR & USART_SR_IDLE)
	{
		read_state = RMARKER;
	}

	if (USART2->SR & USART_SR_RXNE)
	{
		uart_read_process(USART2->DR);
	}

	if (transmit && (USART2->SR & USART_SR_TXE))
	{
		uart_write_process();
	}
}


uint8_t length;
uint8_t length_2;
uint8_t receive_buffer[256];
uint8_t* bufferpointer;

void uart_read_process(uint8_t data)
{
	switch (read_state)
	{
	case RMARKER:
		//пинг
		if (data == 0xA8)
			USART2->DR = 0xA8;
		//остальные
		else if (data == 0xAE)
			read_state = RLEN;
		break;
	case RLEN:
		length = data;
		length_2 = data;
		if (length == 0)
		{
			read_state = RCRC;
			break;
		}
		bufferpointer = receive_buffer;
		read_state = RPACKET;
		break;
	case RPACKET:
		*(bufferpointer++) = data;
		if (--length == 0)
			read_state = RCRC;
		break;
	case RCRC:
		//if(data == crc)
		process_command(receive_buffer, length_2);
		read_state = RMARKER;
		break;
	}
}

uint8_t transmit_buffer[256];
uint8_t transmit_pointer;
uint8_t length;

enum transmit_state_enum
{
	TLEN, TPACKET, TCRC
} transmit_state;

void transmit_start(uint8_t len)
{
	transmit_state = TLEN;
	length = len;
	transmit_pointer = 0;
	USART2->DR = 0xAE;
	transmit = true;
	USART2->CR1 |= USART_CR1_TXEIE;
}

void uart_write_process()
{
	switch (transmit_state)
	{
	case TLEN:
		USART2->DR = length;
		if (length)
			transmit_state = TPACKET;
		else
			transmit_state = TCRC;
		break;
	case TPACKET:
		USART2->DR = transmit_buffer[transmit_pointer];
		if (++transmit_pointer == length)
			transmit_state = TCRC;
		break;
	case TCRC:
		transmit = false;
		USART2->DR = 0xAB;
		USART2->CR1 &= ~USART_CR1_TXEIE;
		break;
	}
}

