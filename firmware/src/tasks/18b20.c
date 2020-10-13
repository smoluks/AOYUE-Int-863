#include <stdbool.h>
#include "stm32f1xx.h"
#include "18b20.h"
#include "systick.h"
#include "1wire.h"
#include "pt.h"
#include "logic.h"

static bool isPresent = false;
static uint32_t timestamp;

bool init18b20()
{
	isPresent = onewireReset();
	if (isPresent)
	{
		onewireWriteByte(ROM_SkipROM);
		onewireWriteByte(Command_WriteScratchpad);
		onewireWriteByte(0x00); //Th
		onewireWriteByte(0x00); //Tl
		onewireWriteByte(0x7F); //Config
		return true;
	}

	return false;
}

PT_THREAD(process18b20(struct pt *pt))
{
	PT_BEGIN(pt);

	PT_WAIT_UNTIL(pt, isPresent || init18b20());

	timestamp = getSystime();
	PT_WAIT_UNTIL(pt, isTimeout(timestamp, 1000));

	//start conversion
	onewireReset();
	onewireWriteByte(ROM_SkipROM);
	onewireWriteByte(Command_StartConversion);

	//wait
	PT_WAIT_UNTIL(pt, onewireIsDataReady());

	//read
	onewireReset();
	onewireWriteByte(ROM_SkipROM);
	onewireWriteByte(Command_ReadScratchpad);
	uint8_t lsb = onewireReadByte();
	process_ambient((onewireReadByte() << 8) | lsb);

	PT_END(pt);
}
