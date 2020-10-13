#include <stdio.h>
#include "stm32f1xx.h"
#include "lcd.h"
#include "pt.h"
#include "18b20.h"
#include "display.h"
#include "hc05.h"
#include "buttons.h"
#include "systick.h"

static struct pt indication_pt;
static struct pt w18b20_pt;

int main(void)
{
	PT_INIT(&indication_pt);
	PT_INIT(&w18b20_pt);

	configInit();
	hc05Init();
	initButtons();
	init18b20();

	while (1)
	{
		WDT_RESET();

		process18b20(&w18b20_pt);

		processButtons();

		process_indication(&indication_pt);
	}
}





