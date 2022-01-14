#include "config.h"
#include "flash.h"
#include "i2c.h"
#include "string.h"
#include "lcd.h"
#include "pt.h"
#define TOKEN 0xABCDEF13

static void restoreDefaultConfig();
static uint16_t crc16(uint8_t* data_p, uint8_t length);
static PT_THREAD(processConfigInternal(struct pt *pt));

config_s config;
config_s config_backup;
bool config_need_save = false;

void initConfig() {
    displayWriteText("Read config", 0);

    i2cReadBuffer(0, (uint8_t*) &config, sizeof(config));

	if (config.writeToken != TOKEN || crc16((uint8_t*) &config, sizeof(config) - 2) != config.crc)
	{
	    restoreDefaultConfig();
	    saveConfig();
	}

	memcpy((uint8_t*) &config_backup, (uint8_t*) &config, sizeof(config));
}

static struct pt config_pt;
void processConfig()
{
    processConfigInternal(&config_pt);
}

void saveConfig() {
    config.crc = crc16((uint8_t*) &config, sizeof(config) - 2);

     i2cWriteBuffer(0, (uint8_t*) &config, sizeof(config));
	//flashWrite((uint8_t*) &config, sizeof(config));
}

void restoreDefaultConfig()
{
    memset((uint8_t*) &config, 0, sizeof(config));

    config.writeToken = TOKEN;

    for(uint8_t i = 0; i < SENSOR_COUNT; i++)
    {
        config.sensorCorrections[i].additive = 0;
        config.sensorCorrections[i].multiplicative = 1 << 4;
    }
    config.ambientCorrection.additive = 0;
    config.ambientCorrection.multiplicative = 1 << 4;

    for(uint8_t i = 0; i < OUT_COUNT; i++)
    {
        config.pidCoef[i].P = 100;
        config.pidCoef[i].PHeatDiff = 2;
        config.pidCoef[i].PColdDiff = 2;
        config.pidCoef[i].I = 0;
        config.pidCoef[i].Imin = -250;
        config.pidCoef[i].Imax = 250;
        config.pidCoef[i].D = 10;
    }

    config.beepEnable = 1;
}

void updateConfig()
{
    config_need_save = true;
}

static PT_THREAD(processConfigInternal(struct pt *pt))
{
    PT_BEGIN(pt);

    PT_WAIT_UNTIL(pt, config_need_save);
    config_need_save = false;

    uint8_t addr = 0;
    uint8_t* newConfig = (uint8_t*) &config;
    uint8_t* oldConfig = (uint8_t*) &config_backup;

    while(addr < sizeof(config) - 2)
    {
        if(*newConfig != *oldConfig)
        {
            i2cWrite(addr, *newConfig);
            *oldConfig = *newConfig;
        }

        addr++;
        newConfig++;
        oldConfig++;
    }

    config.crc = crc16((uint8_t*) &config, sizeof(config) - 2);
    i2cWrite(sizeof(config) - 2, config.crc);
    i2cWrite(sizeof(config) - 1, config.crc >> 8);

    //i2cWriteBuffer(0, (uint8_t*) &config, sizeof(config));

    PT_END(pt);
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
