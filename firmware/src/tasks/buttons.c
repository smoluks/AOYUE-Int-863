#include <stdbool.h>
#include "stm32f1xx.h"
#include "gpio.h"
#include "pt.h"
#include "logic.h"
#include "systick.h"

PT_THREAD(processTimeout(struct pt *pt));
PT_THREAD(processMenu(struct pt *pt));
PT_THREAD(processUp(struct pt *pt));
PT_THREAD(processDown(struct pt *pt));
void up();
void down();

int8_t selectedRow = -1;

static uint32_t timestamp;
static struct pt ptTimeout, ptMenu, ptUp, ptDown;

extern work_mode_e work_mode;
extern uint16_t targets_temperature[SENSOR_COUNT];

void initButtons()
{
	PT_INIT(&ptTimeout);
	PT_INIT(&ptMenu);
	PT_INIT(&ptUp);
	PT_INIT(&ptDown);
}

void processButtons() {

	processTimeout(&ptTimeout);
	processMenu(&ptMenu);
	processUp(&ptUp);
	processDown(&ptDown);
}

PT_THREAD(processTimeout(struct pt *pt)) {
	PT_BEGIN(pt);

	PT_WAIT_UNTIL(pt, selectedRow != -1 && isTimeout(timestamp, 15000));

	selectedRow = -1;

	PT_END(pt);
}

PT_THREAD(processMenu(struct pt *pt)) {
	PT_BEGIN(pt);

	PT_WAIT_UNTIL(pt, IS_BUTTON_MENU_PRESSED());

	selectedRow++;
	if (selectedRow > SENSOR_COUNT)
		selectedRow = 0;

	timestamp = getSystime();

	PT_WAIT_WHILE(pt, IS_BUTTON_MENU_PRESSED());

	PT_END(pt);
}

PT_THREAD(processUp(struct pt *pt)) {
	PT_BEGIN(pt);

	PT_WAIT_UNTIL(pt, IS_BUTTON_UP_PRESSED());

	up();

	timestamp = getSystime();
	PT_WAIT_WHILE(pt, IS_BUTTON_UP_PRESSED() && !isTimeout(timestamp, 2000));
	while(IS_BUTTON_UP_PRESSED())
	{
		up();

		timestamp = getSystime();
		PT_WAIT_UNTIL(pt, !IS_BUTTON_UP_PRESSED() || isTimeout(timestamp, 20));
	}

	PT_END(pt);
}

PT_THREAD(processDown(struct pt *pt)) {
	PT_BEGIN(pt);

	PT_WAIT_UNTIL(pt, IS_BUTTON_DOWN_PRESSED());

	down();

	timestamp = getSystime();
	PT_WAIT_WHILE(pt, IS_BUTTON_DOWN_PRESSED() && !isTimeout(timestamp, 2000));
	while(IS_BUTTON_DOWN_PRESSED())
	{
		down();

		timestamp = getSystime();
		PT_WAIT_UNTIL(pt, !IS_BUTTON_DOWN_PRESSED() || isTimeout(timestamp, 20));
	}

	PT_END(pt);
}

void up()
{
	switch(selectedRow)
	{
		case 0:
			if(work_mode != MODE_COLD)
				work_mode++;
			break;
		case 1:
		case 2:
		case 3:
			if(targets_temperature[selectedRow - 1] <= 399 << 4)
				targets_temperature[selectedRow - 1] += 1 << 4;
			else
				targets_temperature[selectedRow - 1] = 400 << 4;
			break;
	}
}

void down()
{
	switch(selectedRow)
	{
		case 0:
			if(work_mode != MODE_OFF)
				work_mode--;
			break;
		case 1:
		case 2:
		case 3:
			if(targets_temperature[selectedRow - 1] >= 1 << 4)
				targets_temperature[selectedRow - 1] -= 1 << 4;
			else
				targets_temperature[selectedRow - 1] = 0;
			break;
	}
}

