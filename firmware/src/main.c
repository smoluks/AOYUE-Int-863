#include "1wire.h"
#include "main.h"
#include "lcd.h"
#include "config.h"
#include "pins.h"
#include "delay.h"
#include "pt.h"
#include "error.h"
#include "regul.h"
#include "buttons.h"
#include "stm32f1xx.h"
#include "system_stm32f1xx.h"

int process_indication();

extern struct ramconfig_s ramconfig;

bool connect = false; //подключен ли комп
uint32_t time = 0;
static struct pt indication_pt;

//systick 1ms
void SysTick_Handler()
{
	time++;
}

int main(void)
{
	display_init();
	display_write_text("Loading...", 0);

	//загрузка настроек
	configs_load();

	//настройка датчиков 1Wire
	init_onewire();

	//ошибки при запуске
	if (ramconfig.mainerror)
	{
		write_error(ramconfig.mainerror);
		while (1){}
	}

	//модем в режиме настройки?
	GPIOB->BSRR = BSRR_SET(GPIOB_BTRESET); //clear bt reset
	if(GPIOB->IDR & GPIOB_BTMODE)
	{
		display_write_text("HC-05 config", 0);

		while(GPIOB->IDR & GPIOB_BTMODE);

		GPIOB->BSRR = BSRR_RESET(GPIOB_BTRESET); //set bt reset
		delay(5 * 1000);
		GPIOB->BSRR = BSRR_SET(GPIOB_BTRESET); //clear bt reset
	}

	StartUart();

	PT_INIT(&indication_pt);

	GPIOB->BSRR = BSRR_SET(GPIOB_LED_GREEN);

	while (1)
	{
		if(!connect && (GPIOB->IDR & GPIOB_BTLED2))
		{
			connect = true;
		}
		else if (connect && !(GPIOB->IDR & GPIOB_BTLED2))
		{
			connect = false;
		}

		process_onewire();

		//индикация
		process_indication();

		process_regul(connect);

		process_buttons(connect);
	}
}

static uint32_t timestamp;
int8_t selected = -1;

int process_indication()
{
	PT_BEGIN(&indication_pt);

	timestamp = time;
	PT_WAIT_UNTIL(&indication_pt, (time - timestamp) >= 500);

	if (ramconfig.heater_targets[0].target_mode == Heat)
		display_write_text("Heat", 0);
	else if (ramconfig.heater_targets[0].target_mode == Cool)
		display_write_text("Cool", 0);
	else if (connect)
		display_write_text("Connected", 0);
	else
		display_write_text("Ready", 0);

	char temp[] = "A: xxx°C";
	UInt8ToString(temp + 3, ramconfig.temperatures[1] >> 4);
	display_write_halftext(temp, 1, false);

	temp[0] = 'B';
	UInt8ToString(temp + 3, ramconfig.temperatures[2] >> 4);
	display_write_halftext(temp, 2, false);

	temp[0] = 'C';
	UInt8ToString(temp + 3, ramconfig.temperatures[3] >> 4);
	display_write_halftext(temp, 3, false);

	if (ramconfig.heater_targets[0].target_mode != Off)
	{
		if (ramconfig.heater_targets[0].target_by_sensor[1] != 0)
		{
			UInt8ToString(temp + 3,
					ramconfig.heater_targets[0].target_by_sensor[1] >> 4);
			display_write_halftext(temp + 3, 1, true);
		}

		if (ramconfig.heater_targets[0].target_by_sensor[2] != 0)
		{
			UInt8ToString(temp + 3,
					ramconfig.heater_targets[0].target_by_sensor[2] >> 4);
			display_write_halftext(temp + 3, 2, true);
		}

		if (ramconfig.heater_targets[0].target_by_sensor[3] != 0)
		{
			UInt8ToString(temp + 3,
					ramconfig.heater_targets[0].target_by_sensor[3] >> 4);
			display_write_halftext(temp + 3, 3, true);
		}
	}
	else
	{
		display_clear_half(1, true);
		display_clear_half(2, true);
		display_clear_half(3, true);
	}

	if(selected>=0)
	{
		display_write_char(0x1F, 123, selected);
	}

	PT_END(&indication_pt);
}


