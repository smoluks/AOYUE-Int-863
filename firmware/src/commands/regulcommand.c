#include "stm32f1xx.h"
#include "command.h"
#include "config.h"
#include "regul.h"
#include "command.h"
#include "string.h"

const char* mainchannelname = "Heaters";
const char* channelnames[] =
{ "Bottom heating" };

extern struct ramconfig_s ramconfig;
extern struct romconfig_s romconfig;

void process_regulcommand(uint8_t* command, uint8_t length)
{
	if (length < 2)
	{
		transmit_commandbadlength();
		return;
	}

	if (command[1] == 0xFF)
	{
		if (length != 2)
		{
			transmit_commandbadlength();
			return;
		}
		//количество
		trasmit_byte(1);
		return;
	} else if (!command[1])
	{
		//общие
		switch (command[2])
		{
		//название
		case 0:
			if (length != 3)
			{
				transmit_commandbadlength();
				return;
			}
			trasmit_text(mainchannelname);
			return;
			//ошибка цифрой
		case 1:
			if (length != 3)
			{
				transmit_commandbadlength();
				return;
			}
			trasmit_byte(0);
			return;
			//ошибка текстом
		case 2:
			if (length != 3)
			{
				transmit_commandbadlength();
				return;
			}
			trasmit_text("No error");
			return;
			//установка режима
		case 0x20:
			if (length != sizeof(ramconfig.heater_targets) + 3)
			{
				transmit_commandbadlength();
				return;
			}
			if (regul_Set_Targets(&command[3]))
				transmit_void();
			else
				transmit_commandnotaccepted();
			return;
			//чтение режима
		case 0x21:
			if (length != 3)
			{
				transmit_commandbadlength();
				return;
			}
			transmit_data(ramconfig.heater_targets,
					sizeof(ramconfig.heater_targets));
			return;
			//установка PID коэффициентов
		case 0x22:
			if (length != sizeof(romconfig.heater_pidcoefs) + 3)
			{
				transmit_commandbadlength();
				return;
			}
			memcpy(romconfig.heater_pidcoefs, &command[3],
					sizeof(romconfig.heater_pidcoefs));
			transmit_void();
			return;
			//чтение PID коэффициентов
		case 0x23:
			if (length != 3)
			{
				transmit_commandbadlength();
				return;
			}
			transmit_data(romconfig.heater_pidcoefs,
					sizeof(romconfig.heater_pidcoefs));
			return;
		default:
			transmit_commandnotfound();
			break;
		}
	} else
	{
		//конкретные
		if (command[1] <= channel_count)
		{
			switch (command[2])
			{
			//название
			case 0:
				if (length != 3)
				{
					transmit_commandbadlength();
					return;
				}
				trasmit_text(channelnames[command[1] - 1]);
				return;
			//ошибка цифрой
			case 1:
				if (length != 3)
				{
					transmit_commandbadlength();
					return;
				}
				trasmit_byte(0);
				return;
			//ошибка текстом
			case 2:
				if (length != 3)
				{
					transmit_commandbadlength();
					return;
				}
				trasmit_text("No error");
				return;
			//установка режима
			case 0x20:
				if (length != sizeof(ramconfig.heater_targets[0]) + 3)
				{
					transmit_commandbadlength();
					return;
				}
				if(regul_Set_Target(&command[3], command[1] - 1))
					transmit_void();
				else
					transmit_commandnotaccepted();
				return;
			//чтение режима
			case 0x21:
				if (length != 3)
				{
					transmit_commandbadlength();
					return;
				}
				transmit_data(&ramconfig.heater_targets[command[1] - 1],
						sizeof(ramconfig.heater_targets[0]));
				return;
			//установка PID коэффициентов
			case 0x22:
				if (length != sizeof(romconfig.heater_pidcoefs[0]) + 3)
				{
					transmit_commandbadlength();
					return;
				}
				memcpy(&romconfig.heater_pidcoefs[command[1] - 1], &command[3],
						sizeof(romconfig.heater_pidcoefs[0]));
				transmit_void();
				return;
			//чтение PID коэффициентов
			case 0x23:
				if (length != 3)
				{
					transmit_commandbadlength();
					return;
				}
				transmit_data(&romconfig.heater_pidcoefs[command[1] - 1],
						sizeof(romconfig.heater_pidcoefs[0]));
				return;
			default:
				transmit_commandnotfound();
				break;
			}
		} else
			transmit_commandbadchannel();
	}
}

