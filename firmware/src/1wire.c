#include "1wire.h"
#include "config.h"
#include "pt.h"
#include "delay.h"
#include "pins.h"
#include "stm32f1xx.h"
#include <stdbool.h>

bool onewire_reset();
void writebyte(uint8_t byte);
uint8_t readbyte();
void write1();
void write0();
bool read();

extern struct ramconfig_s ramconfig;
extern struct romconfig_s romconfig;
extern uint32_t time;

bool isPresent = false;
static struct pt onewire_pt;

void init_onewire()
{
	PT_INIT(&onewire_pt);

	isPresent = onewire_reset();
	if (isPresent)
	{
		writebyte(ROM_SkipROM);
		writebyte(Command_WriteScratchpad);
		writebyte(0x00); //Th
		writebyte(0x00); //Tl
		writebyte(0x7F); //Config
	}
}

static uint32_t timestamp;

int process_onewire()
{
	PT_BEGIN(&onewire_pt);

	PT_WAIT_UNTIL(&onewire_pt, isPresent);

	//start conversion
	onewire_reset();
	writebyte(ROM_SkipROM);
	writebyte(Command_StartConversion);
	//wait
	GPIOB->CRL = (GPIOB->CRL & 0xFFFFFFF0) | 0x00000003; //выставляем на выходе жесткую 1

	timestamp = time;
	PT_WAIT_UNTIL(&onewire_pt, (time - timestamp) >= 750);

	GPIOB->CRL = (GPIOB->CRL & 0xFFFFFFF0) | 0x00000007;
	//read
	onewire_reset();
	writebyte(ROM_SkipROM);
	writebyte(Command_ReadScratchpad);
	ramconfig.temperatures[0] = (((readbyte() | (readbyte() << 8))
			* romconfig.sensor_corrections[0].mul_corr) >> 8)
			+ romconfig.sensor_corrections[0].add_corr;

	PT_END(&onewire_pt);
}

bool onewire_reset()
{
	//Master Tx Reset pulse
	GPIOB->BSRR = BSRR_RESET(GPIOB_1WIRE);
	timer_delay(480);
	GPIOB->BSRR = BSRR_SET(GPIOB_1WIRE);
	//DS18B20 Waits 15-60µs
	timer_delay(60);
	bool ret = !(GPIOB->IDR & GPIOB_1WIRE);
	//Master Rx
	timer_delay(420);
	//
	return ret;
}

void writebyte(uint8_t byte)
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

uint8_t readbyte()
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

void write1()
{
	//start pulse
	GPIOB->BSRR = BSRR_RESET(GPIOB_1WIRE);
	timer_delay(1);
	GPIOB->BSRR = BSRR_SET(GPIOB_1WIRE);
	//wait for slave read
	timer_delay(60);
}

void write0()
{
	//wait for slave read
	GPIOB->BSRR = BSRR_RESET(GPIOB_1WIRE);
	timer_delay(60);
	//
	GPIOB->BSRR = BSRR_SET(GPIOB_1WIRE);
}

bool read()
{
	//start pulse
	GPIOB->BSRR = BSRR_RESET(GPIOB_1WIRE);
	delay(1);
	GPIOB->BSRR = BSRR_SET(GPIOB_1WIRE);
	//wait
	timer_delay(12);
	//read
	bool ret = (GPIOB->IDR & GPIOB_1WIRE);
	//
	timer_delay(48);
	//
	return ret;
}

