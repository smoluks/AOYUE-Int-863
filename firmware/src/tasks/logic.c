#include "stm32f1xx.h"
#include "logic.h"
#include "pt.h"

work_mode_e work_mode;

uint8_t getHeatValue();
uint8_t getColdValue();

//18b20 format, 12 integer bits, 4 fractional bits
uint16_t sensors_themperature[SENSOR_COUNT];
uint16_t ambient_themperature = 25 << 4;
uint16_t targets_temperature[SENSOR_COUNT];

uint8_t value;

void process_sensor(uint8_t channel, uint32_t value)
{
	value = value >> 4;

	value += ambient_themperature;

	sensors_themperature[channel] = value > 0xFFFF ? 0xFFFF : value;
}

void process_ambient(uint16_t value)
{
	ambient_themperature = value;
}

uint8_t getOutputLevel()
{
	switch(work_mode)
	{
		case MODE_HEAT:
			value = getHeatValue();
			break;
		case MODE_COLD:
			value = getColdValue();
			break;
		default:
			return 0;
	}

	return value;
}

uint8_t getHeatValue()
{
	for(int i = 0; i < SENSOR_COUNT; i++)
	{
		if(sensors_themperature[i] >= targets_temperature[i])
			return 0;
	}

	return 100;
}

uint8_t getColdValue()
{
	for(int i = 0; i < SENSOR_COUNT; i++)
	{
		if(sensors_themperature[i] >= targets_temperature[i])
			return 0;
	}

	return 100;
}
