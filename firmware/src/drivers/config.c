#include "config.h"
#include "flash.h"

#define TOKEN 0xABCDEF13

static void restoreDefaultConfig();
static uint16_t crc16(uint8_t* data_p, uint8_t length);

config_s config;

void initConfig() {
	uint32_t token = flashReadUInt32(0);
	if (token == TOKEN) {
		flashRead((uint8_t*) &config, sizeof(config));
		if(crc16((uint8_t*) &config, sizeof(config_s) - 2) == config.crc )
		{
		    return;
		}
	}

	restoreDefaultConfig();
	saveConfig();
}

void saveConfig() {
    config.crc = crc16((uint8_t*) &config, sizeof(config_s) - 2);

	flashWrite((uint8_t*) &config, sizeof(config));
}

void restoreDefaultConfig()
{
    config.writeToken = 0xABCDEF13;
    for(uint8_t i = 0; i < SENSOR_COUNT; i++)
    {
        config.sensorCorrections[i].additive = 0;
        config.sensorCorrections[i].multiplicative = 1 << 4;
    }
}

uint16_t crc16(uint8_t* data_p, uint8_t length)
{
    uint8_t x;
    uint16_t crc = 0xFFFF;

    while (length--){
        x = crc >> 8 ^ *data_p++;
        x ^= x >> 4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x <<5)) ^ ((uint16_t)x);
    }
    return crc;
}
