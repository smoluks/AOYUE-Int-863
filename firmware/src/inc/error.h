#pragma once

typedef enum
{
	ERR_NOERROR = 0,
	ERR_CRYSTAL = 1,
	ERR_PLL = 2,
	ERR_UART_PARITY = 3,
	ERR_UART_OVERRUN = 4,
	ERR_UART_NOISE = 5,
	ERR_I2C = 6,
	ERR_POWER_FREQ_HIGH = 7,
	ERR_NO_POWER_FREQ = 8,
	ERR_HARDFAULT = 9,
} error_t;

typedef enum
{
    SERR_NOERROR = 0,
    SERR_NOTCONFIGURED = 1,
    SERR_NO_SENSOR = 2,
    SERR_OPEN = 3,
    SERR_OVUV = 4,
    SERR_TCLOW = 5,
    SERR_TCHIGH = 6,
    SERR_CJLOW = 7,
    SERR_CJHIGH = 8,
    SERR_TC_RANGE = 9,
    SERR_CJ_RANGE = 0x0A,
    SERR_NOCHIP = 0x0B,
    SERR_FAKE = 0x0C,
} sensor_error_t;

void setError(error_t err);
error_t getError();
