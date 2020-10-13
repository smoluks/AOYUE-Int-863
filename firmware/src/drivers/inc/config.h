#pragma once

#include <stdint.h>
#include "logic.h"

#pragma pack(push, 1)

typedef struct
{
	int8_t heat;
	int8_t cold;
} tempspeed_s;


//4 integer bits, 4 fractional bits
typedef struct
{
	int16_t additive;
	int16_t multiplicative;
} correction_s;

//padding to 2 bytes
typedef struct
{
	uint32_t writeToken;
	tempspeed_s speedLimits[SENSOR_COUNT];
	correction_s sensorCorrections[SENSOR_COUNT];
	correction_s ambientCorrection;
} config_s;

#pragma pack(pop)

void configInit();
void configSave();
