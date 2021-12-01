#pragma once

#include <stdint.h>
#include "sensors.h"
#include "hardwareConfig.h"

#pragma pack(push, 1)

typedef struct
{
	int8_t heat; //degree by seconds
	int8_t cold;
} tempspeed_s;

typedef struct
{
    dallasTemperature additive;
	dallasTemperature multiplicative;
} correction_s;

//padding to 2 bytes
typedef struct
{
	uint32_t writeToken;
	int16_t targets_temperature[SENSOR_COUNT];
	tempspeed_s speedLimits[SENSOR_COUNT];
	correction_s sensorCorrections[SENSOR_COUNT];
	uint16_t crc;
} config_s;

#pragma pack(pop)

void initConfig();
void processConfig();
void saveConfig();
void updateConfig();
