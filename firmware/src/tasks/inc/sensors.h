#pragma once

#include "stm32f1xx.h"
#include "error.h"
#include "hardwareConfig.h"

typedef struct sensor {
    bool isPresent;
    dallasTemperature value;
    sensor_error_t error;
} sensor_s;

void processSensors(void);
