#include "hardwareConfig.h"
#include "stm32f1xx.h"
#include "lcd.h"
#include "pt.h"
#include "18b20.h"
#include "display.h"
#include "hc05.h"
#include "buttons.h"
#include "systick.h"
#include "max31856.h"
#include "sensors.h"
#include "config.h"

static struct pt indication_pt;

int main(void)
{
	PT_INIT(&indication_pt);

	initConfig();

#ifdef HC05
	hc05Init();
#endif

	//initButtons();

	while (1)
	{
		WDT_RESET();

		//processButtons();

		processSensors();

		process_indication(&indication_pt);
	}
}





