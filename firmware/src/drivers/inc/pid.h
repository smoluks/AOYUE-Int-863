#pragma once

uint8_t calculateNext();

typedef enum {
	MODE_OFF = 0,
	MODE_HEAT,
	MODE_COLD,
} work_mode_e;
