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
#include "logic.h"
#include "beeper.h"

int main(void)
{
    initLogic();
	initConfig();

#ifdef HC05
	hc05Init();
#endif

	initBeeper();
	initIndication();
	//initButtons();

	while (1)
	{
		WDT_RESET();

		//processButtons();

		processSensors();

		processIndication();

		processBeeper();

		processLogic();

		processConfig();
	}
}





