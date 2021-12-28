#include <hardwareConfig.h>
#include <output.h>
#include "stm32f1xx.h"
#include "lcd.h"
#include "swo.h"
#include "max31856.h"
#include "adc.h"
#include "18b20.h"
#include "1wire.h"
#include "systick.h"
#include "uart.h"
#include "modbus.h"
#include "error.h"
#include "i2c.h"

static void systemCoreClockUpdate();

void systemInit() {
    uint16_t count = 0;
    while(--count)
    {
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
    }

    //----- CLK -----
	RCC->APB2ENR = RCC_APB2ENR_AFIOEN;
    AFIO->MAPR = AFIO_MAPR_SWJ_CFG_JTAGDISABLE; //disable JTAG

	//
#ifndef DEBUG_MODE
	//iwdg
	IWDG->KR = 0x5555;
	IWDG->PR = 7;
	IWDG->RLR = 1000 * 40 / 256;
	IWDG->KR = 0xAAAA;
	IWDG->KR = 0xCCCC;*/
#endif

	//
	__enable_irq();
	systickInit();
	displayInit();
    uart1Init();
    modbusInit();
    i2cInit();
    //
    systemCoreClockUpdate();
    //
    displayWriteText("Sensors configuring", 0);
#ifdef DS18B20
	onewireInit();
    init18b20();
#endif

#ifdef ANALOG_TC
	adcInit();
#endif

#ifdef MAX31856
	max31856Init();
#endif

    crossZeroInit();

	displayWriteText("Starting...", 0);
}

void systemCoreClockUpdate() {
	//start HSE
	displayWriteText("Starting HSE", 0);
    uint8_t count = 0;
	RCC->CR = RCC_CR_HSEON;
	while (!(RCC->CR & RCC_CR_HSERDY) && ++count < 250);
	if(count == 250)
	{
	    setError(ERR_CRYSTAL);
	    return;
	}

	//configure
	FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_1;
	RCC->CFGR = RCC_CFGR_PLLMULL9 | RCC_CFGR_PLLSRC | RCC_CFGR_ADCPRE_DIV6 | RCC_CFGR_PPRE2_DIV1 | RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_HPRE_DIV1;

	//start PLL
	displayWriteText("Starting PLL", 0);
    count = 0;
	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY) && ++count < 100);
	if(count == 100)
	{
	    setError(ERR_PLL);
        return;
	}

	//Switch to PLL
	displayWriteText("Switch to PLL", 0);
	count = 0;
	RCC->CFGR = RCC->CFGR | RCC_CFGR_SW_PLL;
	while (((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)  && ++count < 100);
	if(count == 100)
	{
	    setError(ERR_PLL);
        return;
	}
}
