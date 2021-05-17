#include "18b20.h"
#include "systick.h"
#include "1wire.h"
#include "pt.h"
#include "sensors.h"

sensor_s s18b20;

bool init18b20() {
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
		//it's fake
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
		//it's fake
	}

	//---configure---
	if (!onewireReset()) {
		return false;
	}
	onewireWriteByte(ROM_SkipROM);
	onewireWriteByte(Command_WriteScratchpad);
	onewireWriteByte(0x00); //Th
	onewireWriteByte(0x00); //Tl
	onewireWriteByte(CONFIG_12BIT); //Config
	return true;

	return false;
}

static uint32_t timestamp;
PT_THREAD(process18b20(struct pt *pt)) {
	PT_BEGIN(pt);

	PT_WAIT_UNTIL(pt, s18b20.isPresent || init18b20());

		timestamp = getSystime();
		PT_WAIT_UNTIL(pt, isTimeout(timestamp, 10000));

		//start conversion
		if (!onewireReset()) {
			s18b20.isPresent = false;
			PT_RESTART(pt);
		}

		onewireWriteByte(ROM_SkipROM);
		onewireWriteByte(Command_StartConversion);

		//wait
		PT_WAIT_UNTIL(pt, onewireIsDataReady());

		//read
		if (!onewireReset()) {
			s18b20.isPresent = false;
			PT_RESTART(pt);
		}
		onewireWriteByte(ROM_SkipROM);
		onewireWriteByte(Command_ReadScratchpad);

		uint8_t lsb = onewireReadByte();
		s18b20.value = (onewireReadByte() << 8) | lsb;
		s18b20.isPresent = true;

	PT_END(pt);
}
