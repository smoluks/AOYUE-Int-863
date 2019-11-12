#pragma once

#include "error.h"
#include "stm32f1xx.h"
void configs_load();

#define channel_count 1
#define sensor_count 5

#pragma pack(push, 1)

enum target_mode_e
{
	Off = 0,
	Heat = 1,
	Cool = 2
};

struct heater_target_s
{
	enum target_mode_e target_mode;

	int16_t target_by_sensor[sensor_count];
};

struct heater_pidcoefs_s
{
	uint16_t p; //8 бит под дробную часть
	uint16_t i;
	uint16_t d;
};

struct sensor_corrections_s
{
	//аддитивная коррекция коррекция - 4 бита под дробную часть (знаковая!!!!!!!!!!!)
	int16_t add_corr;

	//мультипликативная коррекция *256
	uint16_t mul_corr;
};

struct ramconfig_s
{
	//ошибка всей станции
	enum mainError_e mainerror;

	//значения датчиков температуры - 4 бита под дробную часть
	int16_t temperatures[5];

	//задачи
	struct heater_target_s heater_targets[1];
};

struct romconfig_s
{
	struct sensor_corrections_s sensor_corrections[5];

	struct heater_pidcoefs_s heater_pidcoefs[1];
};

#pragma pack(pop)
