#include "stm32f1xx.h"
#include "pid.h"
#include "sensors.h"
#include "main.h"
#include "hardwareConfig.h"

uint8_t getHeatValue();
uint8_t getColdValue();

work_mode_e work_mode;
int16_t targets_temperature[8];

extern sensor_s sensors[8];

//PID calculator
uint8_t calculateNext()
{
	switch(work_mode)
	{
		case MODE_HEAT:
			return getHeatValue();
		case MODE_COLD:
			return getColdValue();
		default:
			return 0;
	}
}

uint8_t getHeatValue()
{
	for(int i = 0; i < SENSOR_COUNT; i++)
	{
		if(sensors[i].value >= targets_temperature[i])
			return 0;
	}

	return 100;
}

uint8_t getColdValue()
{
	for(int i = 0; i < SENSOR_COUNT; i++)
	{
		if(sensors[i].value >= targets_temperature[i])
			return 0;
	}

	return 100;
}
