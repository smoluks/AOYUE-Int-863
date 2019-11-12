#include "error.h"
#include "config.h"
#include "pins.h"
#include "lcd.h"

extern struct ramconfig_s ramconfig;

void write_error(enum mainError_e error)
{
	GPIOB->BSRR = BSRR_RESET(GPIOB_LED_GREEN) | BSRR_SET(GPIOB_LED_RED);

	ramconfig.mainerror = error;

	char* errorstring = "ERROR: xxx";

	switch(error)
	{
		case ERR_CRYSTAL:
			display_write_text("ERR_CRYSTAL", 0);
			break;
		case ERR_PLL:
			display_write_text("ERR_PLL", 0);
			break;
		case ERR_OVERRUN:
			display_write_text("ERR_OVERRUN", 0);
			break;
		default:
			UInt8ToString(errorstring+7, (uint8_t)error);
			display_write_text(errorstring, 0);
	}
}



