#pragma once
#include "stm32f1xx.h"

//12 integer bits, 4 fractional bits as 18b20 output
typedef uint16_t dallasTemperature;

#define SENSOR_COUNT 5
#define SENSOR_AMBIENT 0
#define SENSOR_A 1
#define SENSOR_B 2
#define SENSOR_C 3
#define SENSOR_D 4

#define MAX31856
#ifdef MAX31856
	#define MAX31856_0_CHANNEL SENSOR_A
	#define MAX31856_1_CHANNEL SENSOR_B
	#define MAX31856_2_CHANNEL SENSOR_C

	#define MAX31856_0_TYPE MAX31856_CR1_TYPE_K
	#define MAX31856_1_TYPE MAX31856_CR1_TYPE_K
	#define MAX31856_2_TYPE MAX31856_CR1_TYPE_K

    //#define MAX31856_TAKE_AMBIENT
#endif

#define DS18B20
#ifdef DS18B20
    #define DS18B20_CHANNEL SENSOR_AMBIENT

    #define DS18B20_FAKECHECK
#endif

//#define ANALOG_TC

#define HC05
#ifdef HC05
    #define FAKE_HC05_BC3
#endif
