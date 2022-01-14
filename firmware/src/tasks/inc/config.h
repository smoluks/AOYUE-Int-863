#pragma once

#include <stdint.h>
#include "sensors.h"
#include "hardwareConfig.h"

#pragma pack(push, 1)

typedef struct
{
    lilDallasTemperature heatSpeed; //by seconds
    lilDallasTemperature coolSpeed; //by seconds
} tempspeed_s;

typedef struct
{
    dallasTemperature additive;
	dallasTemperature multiplicative;
} correction_s;

typedef struct
{
    int16_t P;
    int16_t PHeatDiff;
    int16_t PColdDiff;
    int16_t I;
    int16_t Imin;
    int16_t Imax;
    int16_t D;
} pid_s;

//padding to 2 bytes for 16-bit flash
typedef struct
{
	uint32_t writeToken;
	int16_t targetTemperatures[SENSOR_COUNT];
	tempspeed_s speedLimits[SENSOR_COUNT];
	correction_s sensorCorrections[SENSOR_COUNT];
	correction_s ambientCorrection;
	pid_s pidCoef[OUT_COUNT];
	uint16_t beepEnable;
	uint16_t crc;
} config_s;

#pragma pack(pop)

void initConfig();
void processConfig();
void saveConfig();
void updateConfig();
