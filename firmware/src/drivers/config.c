#include "config.h"
#include "flash.h"

#define TOKEN 0xABCDEF13

config_s config = {
		.writeToken = TOKEN,
		.sensorCorrections = { { 0, 1 << 4 }, { 0, 1 << 4 }, { 0, 1 << 4 }, },
		.ambientCorrection = { 0, 1 << 4 }

};

void configInit() {
	uint32_t token = flashReadUInt32(0);
	if (token == TOKEN) {
		flashRead((uint8_t*) &config, sizeof(config));
	} else {
		flashWrite((uint8_t*) &config, sizeof(config));
	}
}

void configSave() {
	flashWrite((uint8_t*) &config, sizeof(config));
}
