#include <stdbool.h>
#include "stm32f1xx.h"
#include "1wire.h"
#include "pt.h"
#include "systick.h"
#include "gpio.h"

static void write1();
static void write0();
static bool read();

#define GPIOB_1WIRE  		8

#define ONEWIRE_PULLDOWN() GPIOB->BSRR = BSRR_RESET(1 << GPIOB_1WIRE)

#define ONEWIRE_RELEASE() GPIOB->BSRR = BSRR_SET(1 << GPIOB_1WIRE)

#define ONEWIRE_ISPULLDOWN() (!(GPIOB->IDR & (1 << GPIOB_1WIRE)))

void onewireInit()
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	GPIOB->CRH = (GPIOB-> CRH & 0xFFFFFFF0) | GENERAL_OPEN_DRAIN_FAST;
	GPIOB->ODR |= 1 << GPIOB_1WIRE;
}

bool onewireReset()
{
	//Master Tx Reset pulse
	ONEWIRE_PULLDOWN();
	delay_us(480);
	ONEWIRE_RELEASE();
	//DS18B20 Waits 15-60us
	delay_us(60);
	bool ret = ONEWIRE_ISPULLDOWN();
	//Master Rx
	delay_us(420);
	//
	return ret;
}

bool onewireIsDataReady()
{
	return read();
}

void onewireWriteByte(uint8_t byte)
{
	__disable_irq();
	for (uint8_t i = 0; i < 8; i++)
	{
		if (byte & 0x01)
			write1();
		else
			write0();

		byte >>= 1;
	}
	__enable_irq();
}

uint8_t onewireReadByte()
{
	uint8_t t;

	__disable_irq();
	for (uint8_t i = 0; i < 8; i++)
	{
		t >>= 1;
		t |= (read() << 7);
	}
	__enable_irq();

	return t;
}

static void write1()
{
	//start pulse
	ONEWIRE_PULLDOWN();
	delay_us(1);
	ONEWIRE_RELEASE();
	//wait for slave read
	delay_us(60);
}

static void write0()
{
	//wait for slave read
	ONEWIRE_PULLDOWN();
	delay_us(60);
	//
	ONEWIRE_RELEASE();
}

static bool read()
{
	//start pulse
	ONEWIRE_PULLDOWN();
	delay_us(1);
	ONEWIRE_RELEASE();
	//wait
	delay_us(12);
	//read
	bool ret = !ONEWIRE_ISPULLDOWN();
	//
	delay_us(48);
	//
	return ret;
}
