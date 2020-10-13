#include "stm32f1xx.h"
#include "lcd.h"
#include "font.h"
#include "string.h"
#include "stdbool.h"
#include "gpio.h"
#include "systick.h"

static void sendData16(uint16_t data);
static void sendCommand(uint8_t command);
static void sendData(uint8_t data);
static uint8_t writeSymbol(char c, uint8_t countlimit);

void displayInit()
{
	//3uS reset
	GPIOB->BSRR = GPIOB_OLED_RESET << 16; //RESET 0
	delay(3);
	GPIOB->BSRR = GPIOB_OLED_RESET; //RESET 1
	delay(3);

	sendCommand(SegmentRemap_Inverted);

	sendCommand(0xA6); //Set Normal/Inverse Display

	sendCommand(0x8D); //CHARGEPUMP
	sendCommand(0x14);

	sendCommand(0xAF); //Display on

	sendCommand(0x20); //Set Memory Addressing Mode
	sendCommand(0x00);

	for (int i = 0; i < 1024; i++)
	{
		sendData(0);
	}

	displayWriteText("Loading...", 0);
}

void displayWriteText(char* text, uint8_t page)
{
	sendCommand(MemoryAddressingMode);
	sendCommand(MemoryAddressingMode_Vertical);

	sendCommand(ColumnAddress);
	sendCommand(0);
	sendCommand(127);

	sendCommand(PageAddress);
	sendCommand(6-(page*2));
	sendCommand(7-(page*2));

	char c;

	uint8_t columnCount = 0;
	while((c = *text++)!= 0 && columnCount < 128)
	{
		columnCount += writeSymbol(c, 128 - columnCount);
	}

	//������ ������
	int8_t temp = 128 - columnCount;
	while(temp-- > 0)
		sendData16(0x00);
}

void displayWriteHalfText(char* text, uint8_t page, bool secondHalf)
{
	sendCommand(MemoryAddressingMode);
	sendCommand(MemoryAddressingMode_Vertical);

	sendCommand(ColumnAddress);
	if(secondHalf)
	{
		sendCommand(64);
		sendCommand(127);
	}
	else
	{
		sendCommand(0);
		sendCommand(63);
	}

	sendCommand(PageAddress);
	sendCommand(6-(page*2));
	sendCommand(7-(page*2));

	char c;
	uint8_t columnCount = 0;
	while((c = *text++)!= 0 && columnCount < 64)
	{
		columnCount += writeSymbol(c, 64 - columnCount);
	}

	int8_t temp = 64 - columnCount;
	while(temp-- > 0)
		sendData16(0x00);
}

void displayClearHalf(uint8_t page, bool secondHalf)
{
	sendCommand(MemoryAddressingMode);
	sendCommand(MemoryAddressingMode_Vertical);

	sendCommand(ColumnAddress);
	if (secondHalf)
	{
		sendCommand(64);
		sendCommand(127);
	} else
	{
		sendCommand(0);
		sendCommand(63);
	}

	sendCommand(PageAddress);
	sendCommand(6 - (page * 2));
	sendCommand(7 - (page * 2));


	//������ ������
	int8_t temp = 64;
	while (temp-- > 0)
		sendData16(0x00);
}

void displayWriteChar(char c, uint8_t position, uint8_t page)
{
	sendCommand(MemoryAddressingMode);
	sendCommand(MemoryAddressingMode_Vertical);

	sendCommand(ColumnAddress);
	sendCommand(position);
	sendCommand(127);

	sendCommand(PageAddress);
	sendCommand(6 - (page * 2));
	sendCommand(7 - (page * 2));

	writeSymbol(c, 128 - position);
}

static uint8_t writeSymbol(char c, uint8_t countlimit)
{
	uint16_t currectStick;
	uint8_t i = 0;
	do
	{
		currectStick = font[(uint8_t)c][i++];
		if(currectStick == 0xAAAA)
			sendData16(0x0000);
		else
			sendData16(currectStick);
	}
	while(currectStick!=0 && i<16 && i<countlimit);
	return i;
}

static void sendCommand(uint8_t command)
{
	if (GPIOB->ODR & GPIOB_OLED_DC) //���� �� ����� ������������ ������
	{
		while (SPI1->SR & SPI_SR_BSY);

		GPIOB->BSRR = GPIOB_OLED_DC << 16; //DC -> 0
	}

	while (!(SPI1->SR & SPI_SR_TXE));

	SPI1->DR = command;
}

static void sendData(uint8_t data)
{
	if (!(GPIOB->ODR & GPIOB_OLED_DC)) //���� �� ����� ������������ �������
	{
		while (SPI1->SR & SPI_SR_BSY);

		GPIOB->BSRR = GPIOB_OLED_DC; //DC -> 1
	}

	while (!(SPI1->SR & SPI_SR_TXE));

	SPI1->DR = data;
}

static void sendData16(uint16_t data)
{
	if (!(GPIOB->ODR & GPIOB_OLED_DC)) //���� �� ����� ������������ �������
	{
		while (SPI1->SR & SPI_SR_BSY);

		GPIOB->BSRR = GPIOB_OLED_DC; //DC -> 1
	}

	while (!(SPI1->SR & SPI_SR_TXE));

	SPI1->DR = data;

	while (!(SPI1->SR & SPI_SR_TXE));

	SPI1->DR = data >> 8;
}



