#include "stm32f1xx.h"
#include "config.h"
#include "command.h"
#include <string.h>

const char* maintermoname = "Temperature sensors";

const char* termonames[] =
{ "Cold-junction digital sensor", "Heater thermocouple",
		"External thermocouple A", "External thermocouple B",
		"External thermocouple C" };

extern struct ramconfig_s ramconfig;
extern struct romconfig_s romconfig;

void process_termocommand(uint8_t* command, uint8_t length)
{
	if(length<2)
	{
		transmit_commandbadlength();
		return;
	}

	if (command[1] == 0xFF)
	{
		if(length!=2)
		{
			transmit_commandbadlength();
			return;
		}
		//количество
		trasmit_byte(sensor_count);
		return;
	}
	else if (!command[1])
	{
		//общие
		switch (command[2])
		{
		case 0:
			if(length!=3)
			{
				transmit_commandbadlength();
				return;
			}
			trasmit_text(maintermoname);
			return;
		case 1:
			if(length!=3)
			{
				transmit_commandbadlength();
				return;
			}
			trasmit_byte(0);
			return;
		case 2:
			if(length!=3)
			{
				transmit_commandbadlength();
				return;
			}
			trasmit_text("No error");
			return;
		//get temperatures
		case 0x10:
			if (length != 3)
			{
				transmit_commandbadlength();
				return;
			}
			transmit_data(ramconfig.temperatures, sizeof(ramconfig.temperatures));
			return;
		//set correction
		case 0x20:
			if (length != sizeof(romconfig.sensor_corrections)+3)
			{
				transmit_commandbadlength();
				return;
			}
			memcpy(romconfig.sensor_corrections, &command[3], sizeof(romconfig.sensor_corrections));
			transmit_void();
			return;
		//get correction
		case 0x21:
			if (length != 3)
			{
				transmit_commandbadlength();
				return;
			}
			transmit_data(romconfig.sensor_corrections, sizeof(romconfig.sensor_corrections));
			return;
		default:
			transmit_commandnotfound();
			break;
		}
	}
	else
	{
		//конкретные
		if (command[1] <= sensor_count)
		{
			switch (command[2])
			{
			case 0:
				if (length != 3)
				{
					transmit_commandbadlength();
					return;
				}
				trasmit_text(termonames[command[1] - 1]);
				return;
			case 1:
				if (length != 3)
				{
					transmit_commandbadlength();
					return;
				}
				trasmit_byte(0);
				return;
			case 2:
				if (length != 3)
				{
					transmit_commandbadlength();
					return;
				}
				trasmit_text("No error");
				return;
			//get temperatures
			case 0x10:
				if (length != 3)
				{
					transmit_commandbadlength();
					return;
				}
				transmit_data(&ramconfig.temperatures[command[1] - 1], 2);
				return;
			//set correction
			case 0x20:
				if (length != sizeof(struct sensor_corrections_s)+3)
				{
					transmit_commandbadlength();
					return;
				}
				memcpy(&romconfig.sensor_corrections[command[1] - 1], &command[3], sizeof(struct sensor_corrections_s));
				transmit_void();
				return;
			//get correction
				case 0x21:
				if (length != 3)
				{
					transmit_commandbadlength();
					return;
				}
				transmit_data(&romconfig.sensor_corrections[command[1] - 1], sizeof(struct sensor_corrections_s));
				return;
			}
		}
		else transmit_commandbadchannel();
	}
}

