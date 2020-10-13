#pragma once

#include "stm32f1xx.h"

#define SENSOR_COUNT 3

typedef enum {
	MODE_OFF = 0,
	MODE_HEAT,
	MODE_COLD,
} work_mode_e;

void process_sensor(uint8_t channel, uint32_t value);
void process_ambient(uint16_t value);
uint8_t getOutputLevel();
