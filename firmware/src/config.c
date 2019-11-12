#include "config.h"
#include "string.h"

struct ramconfig_s ramconfig;
struct romconfig_s romconfig;

void init_ramconfig();
void read_romconfig();

void configs_load()
{
	init_ramconfig();

	read_romconfig();
}

void init_ramconfig()
{
	ramconfig.mainerror = 0;
	ramconfig.temperatures[0] = 25 << 4;
	ramconfig.temperatures[1] = 25 << 4;
	ramconfig.temperatures[2] = 25 << 4;
	ramconfig.temperatures[3] = 25 << 4;
	ramconfig.temperatures[4] = 25 << 4;

	ramconfig.heater_targets[0].target_mode = 0;
	memset(ramconfig.heater_targets[0].target_by_sensor, 0, 10);
}

void read_romconfig()
{
	//аддитивные коррекции (4 дробных бита как температура) (знаковые!!!!!!)
	romconfig.sensor_corrections[0].add_corr = 0;
	romconfig.sensor_corrections[1].add_corr = 0;
	romconfig.sensor_corrections[2].add_corr = 0;
	romconfig.sensor_corrections[3].add_corr = 0;
	romconfig.sensor_corrections[4].add_corr = 0;

	//мультипликативные коррекции *256
	romconfig.sensor_corrections[0].mul_corr = 256; //1
	romconfig.sensor_corrections[1].mul_corr = 240;
	romconfig.sensor_corrections[2].mul_corr = 245;
	romconfig.sensor_corrections[3].mul_corr = 240;
	romconfig.sensor_corrections[4].mul_corr = 240;

	//PID коэффициенты *256
	romconfig.heater_pidcoefs[0].p = 1 << 5;
	romconfig.heater_pidcoefs[0].i = 2;//4;
	romconfig.heater_pidcoefs[0].d = 0;//1 << 5;
}
