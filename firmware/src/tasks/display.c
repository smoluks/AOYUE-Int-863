#include <sensors.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "stm32f1xx.h"
#include "display.h"
#include "pt.h"
#include "lcd.h"
#include "systick.h"
#include "main.h"
#include "hardwareConfig.h"
#include "logic.h"
#include "config.h"

void update_work_mode();
void update_current_temperature();
void update_target_temperature();
void update_selected_row();

extern work_mode_e work_mode;
extern sensor_s sensors[8];
extern int16_t ambient_themperature;
extern int16_t targets_temperature[SENSOR_COUNT];
extern config_s config;

bool need_update_display = true;
uint8_t selectedRow;

static struct pt indication_pt;
static PT_THREAD(processIndicationInternal(struct pt *pt));

void initIndication()
{
    PT_INIT(&indication_pt);
}

void processIndication()
{
    processIndicationInternal(&indication_pt);
}

void updateDisplay()
{
    need_update_display = true;
}

static PT_THREAD(processIndicationInternal(struct pt *pt))
{
	PT_BEGIN(pt);

	PT_WAIT_UNTIL(pt, need_update_display);

	update_work_mode();

	update_current_temperature();

	update_target_temperature();

	//update_selected_row();

	need_update_display = false;

	PT_END(pt);
}

void update_work_mode()
{
	switch(work_mode)
	{
		case MODE_OFF:
		    displayWriteHalfText("Ready", 0, false);
			break;
		case MODE_HEAT:
		    displayWriteHalfText("Heat", 0, false);
			break;
		case MODE_COLD:
		    displayWriteHalfText("Cool", 0, false);
			break;
	}

	if(getError())
	{
	    char buffer[5];
	    snprintf(buffer, 5, "E%u", getError());
	    displayWriteHalfText(buffer, 0, true);
	}
	else
	    displayWriteHalfText("     ", 0, true);

	//if(selectedRow == 0)
	//	displayWriteChar(17, 123, 0);
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
				snprintf(buffer, 10, "%c: %d%cC   ", firstNumber + i, sensors[i].value >> 4, (char)0xB0);
			} else
			{
				snprintf(buffer, 10, "%c: E%u", firstNumber + i, sensors[i].error);
			}
			displayWriteHalfText(buffer, i + 1, false);
		} else
		{
			displayClearHalf(i + 1, false);
		}
	}
}

void update_target_temperature()
{
	char buffer[10];
	for(uint8_t i = 0; i < 3; i++)
	{
		if(i < SENSOR_COUNT && (config.targets_temperature[i] & 0xF000))
		{
		    snprintf(buffer, 10, "%c %d%cC   ", 16, (config.targets_temperature[i] & 0x3FFF) >> 4, (char)0xB0);
		    displayWriteHalfText(buffer, i + 1, true);

		    //if(selectedRow == i + 1)
		    //	displayWriteChar(17, 123, 1);
		} else
		{
			displayClearHalf(i + 1, true);
		}
	}
}

/*int8_t oldSelectedRow = -1;
void update_selected_row()
{
	if(oldSelectedRow != -1)
		displayWriteChar(0, 123, oldSelectedRow);

	if(selectedRow != -1)
		displayWriteChar(17, 123, selectedRow);

	oldSelectedRow = selectedRow;
}*/
