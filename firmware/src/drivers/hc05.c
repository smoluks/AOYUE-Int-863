#include <stdbool.h>
#include <string.h>
#include "gpio.h"
#include "systick.h"
#include "uart.h"
#include "hc05.h"
#include "lcd.h"

static bool hc05CheckParms();

void hc05Init() {
	displayWriteText("Configuring BT", 0);
	uart2Init();

	//set hc-05 to command mode
	HC05_MODE_CONF();
	HC05_RESET_ON();
	delay(100u);
	HC05_RESET_OFF();
	delay(5000u);

	hc05CheckParms();

	//set hc-05 to bridge mode
	HC05_MODE_WORK();
	HC05_RESET_ON();
	delay(100u);
	HC05_RESET_OFF();

	uart2GotoModbusMode();
}

static bool hc05CheckParms() {
	static char buffer[100];
	//ping
	bool hc05present = uart2SendAtCommand("AT", buffer);
	if (!hc05present)
		return false;

	hc05present = uart2SendAtCommand(GET_NAME_COMMAND, buffer);
	if (!hc05present)
		return false;
	if (strcmp(buffer, GET_NAME_ANSWER)) {
		hc05present = uart2SendAtCommand(SET_NAME_COMMAND, buffer);
		if (!hc05present || strcmp(buffer, "OK"))
			return false;
	}

	hc05present = uart2SendAtCommand(GET_PSWD_COMMAND, buffer);
	if (!hc05present)
		return false;
	if (strcmp(buffer, GET_PSWD_ANSWER)) {
		hc05present = uart2SendAtCommand(SET_PSWD_COMMAND, buffer);
		if (!hc05present || strcmp(buffer, "OK"))
			return false;
	}

	hc05present = uart2SendAtCommand(GET_UART_COMMAND, buffer);
	if (!hc05present)
		return false;
	if (strcmp(buffer, GET_UART_ANSWER)) {
		hc05present = uart2SendAtCommand(SET_UART_COMMAND, buffer);
		if (!hc05present || strcmp(buffer, "OK"))
			return false;
	}

	return true;
}
