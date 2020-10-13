#pragma once

#include <stdint.h>
#include <stdbool.h>

#define ROM_ReadROM 0x33
#define ROM_MatchROM 0x55
#define ROM_SearchROM 0xF0
#define ROM_AlarmSearch 0xEC
#define ROM_SkipROM 0xCC

#define Command_StartConversion 0x44
#define Command_CopyScratchpad 0x48
#define Command_ReadPowerSupply 0xB4
#define Command_RecallE 0xB8
#define Command_ReadScratchpad 0xBE
#define Command_WriteScratchpad 0x4E

bool onewireReset();
void onewireWriteByte(uint8_t byte);
uint8_t onewireReadByte();
bool onewireIsDataReady();
