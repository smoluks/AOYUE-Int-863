#include "configuration.h"
#include <sensors.h>
#include "stm32f1xx.h"
#include "pt.h"
#include "main.h"
#include "max31856.h"

static int16_t getAmbient();
static sensor_s getChannel(uint8_t channel);

extern sensor_s s18b20;
extern sensor_s analog_termocouples[3];
extern sensor_s max31856_termocouples[MAX31856_COUNT];
extern sensor_s max31856_ambient;

//18b20 format, 12 integer bits, 4 fractional bits
sensor_s sensors[SENSOR_COUNT];
int16_t ambient_themperature;

extern bool need_update_display;

void processSensors()
{
  int16_t newambient = getAmbient ();
  if (newambient != ambient_themperature)
  {
	ambient_themperature = newambient;
	need_update_display = true;
  }

  for (int i = 0; i < SENSOR_COUNT; i++)
  {
	sensor_s newtemperature = getChannel (i);

	if ((sensors[i].isPresent != newtemperature.isPresent) || sensors[i].value != newtemperature.value)
	{
	  sensors[i] = newtemperature;
	  need_update_display = true;
	}
  }
}

static int16_t getAmbient()
{
  if (s18b20.isPresent)
	return s18b20.value;

  return 21 << 4;
}

static sensor_s getChannel(uint8_t channel)
{
#ifdef MAX31856
  if(max31856_termocouples[channel].isPresent)
    {
	  return max31856_termocouples[channel];
    }
#endif

  sensor_s result = analog_termocouples[channel];
  if (result.isPresent)
  {
	result.value += ambient_themperature;
  }

  return result;
}
