#include "hardwareConfig.h"
#include "sensors.h"
#include "stm32f1xx.h"
#include "pt.h"
#include "main.h"
#include "max31856.h"
#include "18b20.h"

void processSensors()
{
#ifdef MAX31856
    max31856Read();
#endif

#ifdef DS18B20
   process18b20();
#endif

#ifdef ANALOG_TC
   max31856Read();
#endif

}

