#include "stm32f1xx.h"
#include "uart.h"
#include <string.h>
#include "maincommands.h"
#include "regulcommands.h"
#include "termocommands.h"

void transmit_commandbadlength();

void process_command(uint8_t* command, uint8_t length)
{
	if (length == 0)
	{
		transmit_commandbadlength();
		return;
	}

	switch(command[0])
	{
	case 0:
		process_maincommand(command, length);
		break;
	case 1:
		process_termocommand(command, length);
		break;
	case 2:
		process_regulcommand(command, length);
		break;
	default:
		transmit_commandnotfound();
		break;
	}
}

extern uint8_t transmit_buffer[];

void transmit_data(uint8_t* data, uint8_t length)
{
	memcpy(&transmit_buffer[0], data, length);
	transmit_start(length);
}

void trasmit_text(char* text)
{
	uint8_t length = strlen(text);
	memcpy(transmit_buffer, text, length);
	transmit_start(length);
}

void trasmit_byte(uint8_t byte)
{
	transmit_buffer[0] = byte;
	transmit_start(1);
}

void transmit_void()
{
	transmit_start(0);
}

void transmit_commandnotaccepted()
{
	USART2->DR = 0xAA;
}

void transmit_commandbadchannel()
{
	USART2->DR = 0xAB;
}

void transmit_commandnotfound()
{
	USART2->DR = 0xAC;
}

void transmit_commandbadlength()
{
	USART2->DR = 0xAD;
}

