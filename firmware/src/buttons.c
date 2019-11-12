#include <stdbool.h>
#include "pins.h"
#include "stm32f1xx.h"

void process_buttons_normal(uint8_t button_pressed);

extern uint32_t time;
extern int8_t selected;

uint16_t oldbuttons = 0xE000;
uint32_t button1pressedtime;
uint32_t button2pressedtime;
uint32_t button3pressedtime;

void process_buttons(bool connected)
{
	uint16_t buttons = GPIOC->IDR & 0xE000;
	uint8_t button_pressed = 0;

	if (buttons != oldbuttons)
	{
		if (!(buttons & GPIOC_BUTTON1) && (oldbuttons & GPIOC_BUTTON1))
		{
			button1pressedtime = time;
			button_pressed = 1;
		}
		else if (!(buttons & GPIOC_BUTTON2) && (oldbuttons & GPIOC_BUTTON2))
		{
			button2pressedtime = time;
			button_pressed = 2;
		}
		else if (!(buttons & GPIOC_BUTTON3) && (oldbuttons & GPIOC_BUTTON3))
		{
			button3pressedtime = time;
			button_pressed = 3;
		}

		oldbuttons = buttons;
	}

	if ((!(buttons & GPIOC_BUTTON1) && (time - button1pressedtime > 2000))
		|| (!(buttons & GPIOC_BUTTON2) && (time - button2pressedtime > 2000))
		|| (!(buttons & GPIOC_BUTTON3) && (time - button3pressedtime > 2000)))
			regul_stop();

	if(!connected)
	{
		if(button_pressed)
			process_buttons_normal(button_pressed);
	}
	else
	{
		selected = -1;
	}
}

extern int8_t selected;
void process_buttons_normal(uint8_t button_pressed)
{
}
