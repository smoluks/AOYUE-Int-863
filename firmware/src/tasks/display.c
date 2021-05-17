#include <sensors.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "stm32f1xx.h"
#include "display.h"
#include "pt.h"
#include "lcd.h"
#include "systick.h"
#include "pid.h"
#include "main.h"

void update_work_mode();
void update_current_temperature();
void update_target_temperature();
void update_selected_row();

extern work_mode_e work_mode;
extern int8_t selectedRow;
extern sensor_s sensors[SENSOR_COUNT];
extern int16_t ambient_themperature;
extern int16_t targets_temperature[SENSOR_COUNT];

bool need_update_display = true;

PT_THREAD(process_indication(struct pt *pt))
{
	PT_BEGIN(pt);

	PT_WAIT_UNTIL(pt, need_update_display);

	update_work_mode();

	update_current_temperature();

	update_target_temperature();

	update_selected_row();

	PT_END(pt);
}

void update_work_mode()
{
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
}

void update_current_temperature()
{
	char buffer[10];
	char firstNumber = 'A';
	for(uint8_t i = 0; i < 3; i++)
	{
		if(i < SENSOR_COUNT)
		{
			if(sensors[i].isPresent)
			{
				snprintf(buffer, 10, "%c: %d%cC", firstNumber + i, sensors[i].value >> 4, (char)0xB0);
			} else
			{
				snprintf(buffer, 10, "%c: Error", firstNumber + i);
			}
			displayWriteHalfText(buffer, 1 + i, false);
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
			snprintf(buffer, 10, "%c %d%cC", 16, targets_temperature[i] >> 4, (char)0xB0);
			displayWriteHalfText(buffer, 1 + i, true);

			if(selectedRow == i + 1)
				displayWriteChar(17, 123, i + 1);
		} else
		{
			displayClearHalf(1 + i, false);
		}
	}
}

int8_t oldSelectedRow = -1;
void update_selected_row()
{
	if(oldSelectedRow != -1)
		displayWriteChar(0, 123, oldSelectedRow);

	if(selectedRow != -1)
		displayWriteChar(17, 123, selectedRow);

	oldSelectedRow = selectedRow;
}
