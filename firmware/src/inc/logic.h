#include "output.h"

typedef enum {
    MODE_OFF = 0,
    MODE_HEAT,
    MODE_COLD,
} work_mode_e;

void initLogic();
void processLogic();
void setMode(work_mode_e mode);
void setTargetTemperature(uint8_t channel, int16_t value);
output_s calculateOutput();
