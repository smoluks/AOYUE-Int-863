#include "lcd.h"
#include "font.h"
#include "string.h"
#include "stdbool.h"
#include "pins.h"
#include "delay.h"
#include "stm32f1xx.h"

void send_data16(uint16_t data);
void send_command(uint8_t command);
void send_data(uint8_t data);
uint8_t write_symbol(char c, uint8_t countlimit);


void display_init()
{
	//3uS reset
	GPIOB->BSRR = GPIOB_OLED_RESET << 16; //RESET 0
	delay(3);
	GPIOB->BSRR = GPIOB_OLED_RESET; //RESET 1
	delay(3);

	send_command(SegmentRemap_Inverted);

	send_command(0xA6); //Set Normal/Inverse Display

	send_command(0x8D); //CHARGEPUMP
	send_command(0x14);

	send_command(0xAF); //Display on

	send_command(0x20); //Set Memory Addressing Mode
	send_command(0x00);

	for (int i = 0; i < 1024; i++)
	{
		send_data(0);
	}
}

void display_write_text(char* text, uint8_t page)
{
	send_command(MemoryAddressingMode);
	send_command(MemoryAddressingMode_Vertical);

	send_command(ColumnAddress);
	send_command(0);
	send_command(127);

	send_command(PageAddress);
	send_command(6-(page*2));
	send_command(7-(page*2));

	char c;

	uint8_t columnCount = 0;
	while((c = *text++)!= 0 && columnCount < 128)
	{
		columnCount += write_symbol(c, 128 - columnCount);
	}

	//добить нулями
	int8_t temp = 128 - columnCount;
	while(temp-- > 0)
		send_data16(0x00);
}

void display_write_halftext(char* text, uint8_t page, bool secondHalf)
{
	send_command(MemoryAddressingMode);
	send_command(MemoryAddressingMode_Vertical);

	send_command(ColumnAddress);
	if(secondHalf)
	{
		send_command(64);
		send_command(127);
	}
	else
	{
		send_command(0);
		send_command(63);
	}

	send_command(PageAddress);
	send_command(6-(page*2));
	send_command(7-(page*2));

	char c;
	uint8_t columnCount = 0;
	while((c = *text++)!= 0 && columnCount < 64)
	{
		columnCount += write_symbol(c, 64 - columnCount);
	}

	//добить нулями
	int8_t temp = 64 - columnCount;
	while(temp-- > 0)
		send_data16(0x00);
}

void display_clear_half(uint8_t page, bool secondHalf)
{
	send_command(MemoryAddressingMode);
	send_command(MemoryAddressingMode_Vertical);

	send_command(ColumnAddress);
	if (secondHalf)
	{
		send_command(64);
		send_command(127);
	} else
	{
		send_command(0);
		send_command(63);
	}

	send_command(PageAddress);
	send_command(6 - (page * 2));
	send_command(7 - (page * 2));


	//добить нулями
	int8_t temp = 64;
	while (temp-- > 0)
		send_data16(0x00);
}

void display_write_char(char c, uint8_t position, uint8_t page)
{
	send_command(MemoryAddressingMode);
	send_command(MemoryAddressingMode_Vertical);

	send_command(ColumnAddress);
	send_command(position);
	send_command(127);

	send_command(PageAddress);
	send_command(6 - (page * 2));
	send_command(7 - (page * 2));

	write_symbol(c, 128 - position);
}

void UInt8ToString(char* string, uint16_t value)
{
	string[0] = value / 100 % 10+ 0x30;
	string[1] = value / 10 % 10 + 0x30;
	string[2] = value % 10 + 0x30;
}

uint8_t write_symbol(char c, uint8_t countlimit)
{
	uint16_t currectStick;
	uint8_t i = 0;
	do
	{
		currectStick = font[(uint8_t)c][i++];
		if(currectStick == 0xAAAA)
			send_data16(0x0000);
		else
			send_data16(currectStick);
	}
	while(currectStick!=0 && i<16 && i<countlimit);
	return i;
}

void send_command(uint8_t command)
{
	if (GPIOB->ODR & GPIOB_OLED_DC) //если до этого передавались данные
	{
		while (SPI1->SR & SPI_SR_BSY);

		GPIOB->BSRR = GPIOB_OLED_DC << 16; //DC -> 0
	}

	while (!(SPI1->SR & SPI_SR_TXE));

	SPI1->DR = command;
}

void send_data(uint8_t data)
{
	if (!(GPIOB->ODR & GPIOB_OLED_DC)) //если до этого передавалась команда
	{
		while (SPI1->SR & SPI_SR_BSY);

		GPIOB->BSRR = GPIOB_OLED_DC; //DC -> 1
	}

	while (!(SPI1->SR & SPI_SR_TXE));

	SPI1->DR = data;
}

void send_data16(uint16_t data)
{
	if (!(GPIOB->ODR & GPIOB_OLED_DC)) //если до этого передавалась команда
	{
		while (SPI1->SR & SPI_SR_BSY);

		GPIOB->BSRR = GPIOB_OLED_DC; //DC -> 1
	}

	while (!(SPI1->SR & SPI_SR_TXE));

	SPI1->DR = data;

	while (!(SPI1->SR & SPI_SR_TXE));

	SPI1->DR = data >> 8;
}



