#include "stm32f1xx.h"
#include "config.h"
#include "command.h"

extern struct ramconfig_s ramconfig;

char* main_getErrorText(enum mainError_e error);

const char* name = "INT863-based IR Heater";

void process_maincommand(uint8_t* command, uint8_t length)
{
	if (length != 2)
	{
		transmit_commandbadlength();
		return;
	}

	switch (command[1])
	{
	//название
	case 0:
		trasmit_text(name);
		return;
	//код ошибки
	case 1:
		trasmit_byte(ramconfig.mainerror);
		return;
	//текст ошибки
	case 2:
		trasmit_text(main_getErrorText(ramconfig.mainerror));
		return;
	default:
		transmit_commandnotfound();
		break;
	}
}

char* main_getErrorText(enum mainError_e error)
{
	switch (error)
	{
	case ERR_NOERROR:
		return "No error";
		break;
	case ERR_CRYSTAL:
		return "Crystal error";
		break;
	case ERR_PLL:
		return "PLL error";
		break;
	case ERR_OVERRUN:
		return "UART overrun error";
		break;
	}
	return "";
}
