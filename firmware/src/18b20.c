#include "18b20.h"
#include "config.h"
#include "stm32f1xx.h"
#include <stdbool.h>

extern struct ramconfig_s ramconfig;

bool isPresent = false;

bool onewire_reset();
void writebyte(uint8_t byte);
uint8_t readbyte();
void write1();
void write0();
bool read();

void onewire_init()
{
	isPresent = onewire_reset();
	if (isPresent)
	{
		isPresent = true;
		writebyte(ROM_SkipROM);
		writebyte(0x4E);
		writebyte(0x00);
		writebyte(0x00);
		writebyte(0x7F);
	}
}

void onewire_readTemp()
{
	if (isPresent)
	{
		//start conversion
		onewire_reset();
		writebyte(ROM_SkipROM);
		writebyte(Command_StartConversion);
		//wait
		GPIOB->CRL = (GPIOB->CRL & 0xFFFFFFF0) | 0x00000003;
		uint32_t i = 750 * 1000 * uS;
		while ((i--) != 0)
		{
		}
		GPIOB->CRL = (GPIOB->CRL & 0xFFFFFFF0) | 0x00000007;
		//read
		onewire_reset();
		writebyte(ROM_SkipROM);
		writebyte(Command_ReadScratchpad);
		ramconfig.temperatures[0] = readbyte() | (readbyte() << 8);
	}
}

bool onewire_reset()
{
	//Master Tx Reset pulse
	GPIOB->BSRR = 0x00010000;
	uint16_t i = 480 * uS;
	while ((i--) != 0)
	{
	}
	GPIOB->BSRR = 0x00000001;
	//DS18B20 Waits 15-60µs
	i = 60 * uS;
	while ((i--) != 0)
	{
	}
	bool ret = !(GPIOB->IDR & 0x00000001);
	//Master Rx
	i = 420 * uS;
	while ((i--) != 0)
	{
	}
	//
	return ret;
}

void writebyte(uint8_t byte)
{
	for (int i = 0; i < 8; i++)
	{
		if (byte & 0x01)
			write1();
		else
			write0();

		byte >>= 1;
	}
}

uint8_t readbyte()
{
	uint8_t t;

	for (int i = 0; i < 8; i++)
	{
		t >>= 1;
		t |= (read() << 7);
	}

	return t;
}

void write1()
{
	//start pulse
	GPIOB->BSRR = 0x00010000;
	uint16_t i = 1 * uS;
	while ((i--) != 0)
	{
	}
	GPIOB->BSRR = 0x00000001;
	//wait for slave read
	i = 60 * uS;
	while ((i--) != 0)
	{
	}
}

void write0()
{
	//wait for slave read
	GPIOB->BSRR = 0x00010000;
	uint16_t i = 60 * uS;
	while ((i--) != 0)
	{
	}
	//
	GPIOB->BSRR = 0x00000001;
}

bool read()
{
	//start pulse
	GPIOB->BSRR = 0x00010000;
	uint16_t i = 1 * uS;
	while ((i--) != 0)
	{
	}
	GPIOB->BSRR = 0x00000001;
	//wait
	i = 12 * uS;
	while ((i--) != 0)
	{
	}
	//read
	bool ret = (GPIOB->IDR & 0x00000001);
	//
	i = 48 * uS;
	while ((i--) != 0)
	{
	}
	//
	return ret;
}

