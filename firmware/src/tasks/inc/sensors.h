#pragma once

#include "stm32f1xx.h"

#define SENSOR_COUNT 3

typedef struct sensor {
    bool isPresent;
    int16_t value;
} sensor_s;

