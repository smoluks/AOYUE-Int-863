#include <stdbool.h>
#include <string.h>
#include "gpio.h"
#include "systick.h"
#include "uart.h"
#include "hc05.h"
#include "lcd.h"
#include "hardwareConfig.h"

#ifdef HC05

#define HC05_MODE_MODE GPIO11_CRH(GENERAL_PUSH_PULL_FAST)
#define HC05_MODE_CONF() GPIOA->BSRR = BSRR_SET(BIT11)
#define HC05_MODE_WORK() GPIOA->BSRR = BSRR_RESET(BIT11)

#ifdef FAKE_HC05_BC3
    //it's for fake chip with small chip (bluecore3?) and inverted reset
    #define HC05_MODE_RESET GPIO8_CRH(GENERAL_PUSH_PULL_FAST)
    #define HC05_RESET_ON() GPIOA->BSRR = BSRR_SET(BIT8)
    #define HC05_RESET_OFF() GPIOA->BSRR = BSRR_RESET(BIT8)
#else
    #define HC05_MODE_RESET GPIO8_CRH(GENERAL_OPEN_DRAIN_FAST)
    #define HC05_RESET_ON() GPIOA->BSRR = BSRR_RESET(BIT8)
    #define HC05_RESET_OFF() GPIOA->BSRR = BSRR_SET(BIT8)
#endif

static void checkParms();
static void setError(uint8_t err);
static bool sendAtCommand(char *command, char *buffer);

bt_error_t bt_error;

void hc05Init() {
	displayWriteText("Configuring BT", 0);

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	GPIOA->CRH = (GPIOA->CRH & 0xFFFF0FF0)
	                      | HC05_MODE_MODE //mode
	                      | HC05_MODE_RESET; //reset

	//set hc-05 to command mode
	HC05_MODE_CONF();
	HC05_RESET_ON();
	delay(100u);
	HC05_RESET_OFF();
	//delay(5000u);

	uart1SetHC05ConfigMode();
	checkParms();

	//set hc-05 to bridge mode
	HC05_MODE_WORK();
	//HC05_RESET_ON();
	//delay(100u);
	//HC05_RESET_OFF();

	uart1SetModbusMode();
}

static void checkParms() {
	static char buffer[100];

	//ping
	uint8_t count = 50;
	while(count--)
	{
	    if(sendAtCommand("AT", buffer))
	        break;
	}
	if (!count)
	{
	    setError(HC05ERROR_NOANSWER);
	    return;
	}

	//name
	if (!sendAtCommand(GET_NAME_COMMAND, buffer))
	{
	    setError(HC05ERROR_CONFIGURING_ERROR);
	} else if (strcmp(buffer, GET_NAME_ANSWER))
	{
	    setError(HC05ERROR_NOTCONFIGURED_ERROR);
		if (!sendAtCommand(SET_NAME_COMMAND, buffer) || strcmp(buffer, "OK"))
		    setError(HC05ERROR_CONFIGURING_ERROR);
	}

	//password
	if (!sendAtCommand(GET_PSWD_COMMAND, buffer))
	{
	    setError(HC05ERROR_CONFIGURING_ERROR);
	} else if (strcmp(buffer, GET_PSWD_ANSWER) && strcmp(buffer, GET_PSWD_ANSWER2))
	{
        setError(HC05ERROR_NOTCONFIGURED_ERROR);
		if (!sendAtCommand(SET_PSWD_COMMAND, buffer) || strcmp(buffer, "OK"))
		    if (!sendAtCommand(SET_PSWD_COMMAND2, buffer) || strcmp(buffer, "OK"))
		        setError(HC05ERROR_CONFIGURING_ERROR);
	}

	//uart params
	if (!sendAtCommand(GET_UART_COMMAND, buffer))
	{
	    setError(HC05ERROR_CONFIGURING_ERROR);
	} else if(strcmp(buffer, GET_UART_ANSWER))
	{
        setError(HC05ERROR_NOTCONFIGURED_ERROR);
		if (!sendAtCommand(SET_UART_COMMAND, buffer) || strcmp(buffer, "OK"))
		    setError(HC05ERROR_CONFIGURING_ERROR);
	}

	sendAtCommand(RESET_COMMAND, buffer);
}

static void setError(bt_error_t err)
{
    if(bt_error < err)
    {
        bt_error = err;
    }
}

static uint32_t timestamp;
static bool sendAtCommand(char *command, char *buffer) {
    //clean old
    (void) USART1->DR;
    (void) USART1->DR;

    //send command
    do {
        char c = *command++;
        if (!c)
        {
            while (!(USART1->SR & USART_SR_TXE));
            USART1->DR = 0x0D;

            while (!(USART1->SR & USART_SR_TXE));
            USART1->DR = 0x0A;

            break;
        }

        while (!(USART1->SR & USART_SR_TXE));
        USART1->DR = c;

    } while (1);

    //read
    timestamp = getSystime();
    do {
        while (!(USART1->SR & USART_SR_RXNE) && !isTimeout(timestamp, 100))
        {
            WDT_RESET();
        }

        if(isTimeout(timestamp, 100))
            return false;

        char c = USART1->DR;
        if (c == 0x0D)
            break;

        if(c != 0x0A)
            *buffer++ = c;

    } while(1);

    *buffer = 0;

    return true;
}

#endif
