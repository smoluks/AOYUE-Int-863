#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "stm32f1xx.h"
#include "display.h"
#include "pt.h"
#include "logic.h"
#include "lcd.h"
#include "systick.h"

void update_work_mode();
void update_current_temperature();
void update_target_temperature();
void update_selected_row();

extern work_mode_e work_mode;
extern uint16_t sensors_themperature[SENSOR_COUNT];
extern uint16_t targets_temperature[SENSOR_COUNT];
extern int8_t selectedRow;

work_mode_e old_work_mode = -1;
uint16_t old_sensors_themperature[SENSOR_COUNT];
uint16_t old_targets_temperature[SENSOR_COUNT] = {-1, -1, -1};
int8_t oldSelectedRow = -1;

PT_THREAD(process_indication(struct pt *pt))
{
	PT_BEGIN(pt);

	PT_WAIT_UNTIL(pt, work_mode != old_work_mode ||
			memcmp(sensors_themperature, old_sensors_themperature, sizeof(sensors_themperature)) ||
			memcmp(targets_temperature, old_targets_temperature, sizeof(targets_temperature)) ||
			oldSelectedRow != selectedRow
			);

	update_work_mode();

	update_current_temperature();

	update_target_temperature();

	update_selected_row();

	PT_END(pt);
}

void update_work_mode()
{
	if(work_mode == old_work_mode)
		return;

	switch(work_mode)
	{
		case MODE_OFF:
			displayWriteText("Ready", 0);
			break;
		case MODE_HEAT:
			displayWriteText("Heat", 0);
			break;
		case MODE_COLD:
			displayWriteText("Cool", 0);
			break;
	}

	if(selectedRow == 0)
		displayWriteChar(17, 123, 0);

	old_work_mode = work_mode;
}

void update_current_temperature()
{
	char buffer[10];
	char firstNumber = 'A';
	for(uint8_t i = 0; i < 3; i++)
	{
		if(i < SENSOR_COUNT)
		{
			if(sensors_themperature[i] == old_sensors_themperature[i])
				continue;

			snprintf(buffer, 10, "%c: %d%cC", firstNumber + i, sensors_themperature[i] >> 4, (char)0xB0);
			displayWriteHalfText(buffer, 1 + i, false);

			old_sensors_themperature[i] = sensors_themperature[i];
		} else
		{
			displayClearHalf(1 + i, false);
		}
	}

}

void update_target_temperature()
{
	char buffer[10];
	for(uint8_t i = 0; i < 3; i++)
	{
		if(i < SENSOR_COUNT)
		{
			if(targets_temperature[i] == old_targets_temperature[i])
				continue;

			snprintf(buffer, 10, "%c %d%cC", 16, (targets_temperature[i] & 0x3FFF) >> 4, (char)0xB0);
			displayWriteHalfText(buffer, 1 + i, true);

			if(selectedRow == i + 1)
				displayWriteChar(17, 123, i + 1);

			old_targets_temperature[i] = targets_temperature[i];
		} else
		{
			displayClearHalf(1 + i, false);
		}
	}
}

void update_selected_row()
{
	if(oldSelectedRow == selectedRow)
		return;

	if(oldSelectedRow != -1)
		displayWriteChar(0, 123, oldSelectedRow);

	if(selectedRow != -1)
		displayWriteChar(17, 123, selectedRow);

	oldSelectedRow = selectedRow;
}
