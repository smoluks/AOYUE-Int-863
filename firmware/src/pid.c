#include "stm32f1xx.h"
#include "stdbool.h"
#include "config.h"

extern struct romconfig_s romconfig;

int32_t pid_istate = 0;
int32_t pid_dstate = 0;
bool pid_first = true;

const int32_t imin = 0 - (1000 << 12);
const int32_t imax = 1000 << 12;

//dt, temp 4 бита под дробную часть
int16_t PID(int16_t dt, int16_t temp)
{
	int32_t out = 0; //12 бит под дробную часть
	//-----P-----
	out = romconfig.heater_pidcoefs[0].p * dt;
    //-----I-----
	pid_istate += dt * romconfig.heater_pidcoefs[0].i;
	if(pid_istate < imin)
		pid_istate = imin;
	if(pid_istate > imax)
			pid_istate = imax;
	out += pid_istate;
	//-----D-----
	if(pid_first)
		pid_first = false;
	else
		out -= romconfig.heater_pidcoefs[0].d * (temp - pid_dstate);
	pid_dstate = temp;
	//
	return out >> 12;
}
