#include "18b20.h"
#include "systick.h"
#include "1wire.h"
#include "pt.h"
#include "sensors.h"
#include "hardwareConfig.h"

#ifdef DS18B20

static PT_THREAD(process18b20Internal(struct pt *pt));

extern sensor_s sensors[SENSOR_COUNT];
static struct pt w18b20_pt;

bool init18b20() {
#ifdef DS18B20_FAKECHECK
	//---check fake---
	//https://github.com/cpetrich/counterfeit_DS18B20
	// check serial
	if (!onewireReset()) {
		return false;
	}
	onewireWriteByte(ROM_ReadROM);

	uint8_t serial[8];
	for (uint8_t i = 0; i < 8; i++) {
		serial[i] = onewireReadByte();
	}
	if (serial[0] != 0x28 || serial[5] != 0x00 || serial[6] != 0x00) {
	    sensors[DS18B20_CHANNEL].error = SERR_FAKE;
	    sensors[DS18B20_CHANNEL].isPresent = false;
	    return false;
	}

	// check scratchpad
	if (!onewireReset()) {
		return false;
	}
	onewireWriteByte(ROM_SkipROM);
	onewireWriteByte(Command_ReadScratchpad);

	uint8_t scratchpad[8];
	for (uint8_t i = 0; i < 8; i++) {
		scratchpad[i] = onewireReadByte();
	}
	if (scratchpad[0] != 0x50 || scratchpad[1] != 0x05 || serial[4] != 0x7F
			|| serial[5] != 0xFF || serial[6] != 0x0C || serial[7] != 0x10) {
	    sensors[DS18B20_CHANNEL].error = SERR_FAKE;
	    sensors[DS18B20_CHANNEL].isPresent = false;
	    return false;
	}
#endif

	//---configure---
	if (!onewireReset()) {
		return false;
	}
	onewireWriteByte(ROM_SkipROM);
	onewireWriteByte(Command_WriteScratchpad);
	onewireWriteByte(0x00); //Th
	onewireWriteByte(0x00); //Tl
	onewireWriteByte(CONFIG_12BIT); //Config

    PT_INIT(&w18b20_pt);
	return true;
}

void process18b20()
{
    process18b20Internal(&w18b20_pt);
}

static uint32_t timestamp;
PT_THREAD(process18b20Internal(struct pt *pt)) {
	PT_BEGIN(pt);

	PT_WAIT_UNTIL(pt, sensors[DS18B20_CHANNEL].isPresent || init18b20());

	timestamp = getSystime();
	PT_WAIT_UNTIL(pt, isTimeout(timestamp, 10000));

	//start conversion
	if (!onewireReset()) {
	    sensors[DS18B20_CHANNEL].isPresent = false;
		PT_RESTART(pt);
	}

	onewireWriteByte(ROM_SkipROM);
	onewireWriteByte(Command_StartConversion);

	//wait
	PT_WAIT_UNTIL(pt, onewireIsDataReady());

	//read
	if (!onewireReset()) {
	    sensors[DS18B20_CHANNEL].isPresent = false;
		PT_RESTART(pt);
	}
	onewireWriteByte(ROM_SkipROM);
	onewireWriteByte(Command_ReadScratchpad);

	uint8_t lsb = onewireReadByte();
	sensors[DS18B20_CHANNEL].value = (onewireReadByte() << 8) | lsb;
	sensors[DS18B20_CHANNEL].isPresent = true;
	sensors[DS18B20_CHANNEL].error = SERR_NOERROR;

	PT_END(pt);
}

#endif
