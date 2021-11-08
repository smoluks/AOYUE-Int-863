#pragma once

#include "pt.h"
#include "stdbool.h"

#define CONFIG_9BIT 0x1F
#define CONFIG_10BIT 0x3F
#define CONFIG_11BIT 0x5F
#define CONFIG_12BIT 0x7F

bool init18b20();
void process18b20();
