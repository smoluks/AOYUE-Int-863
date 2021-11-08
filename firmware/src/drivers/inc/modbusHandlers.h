#pragma once
#include "hardwareConfig.h"

// ----- modbus registers -----
//input registers
// 0,2,4,.. - sensors value
// 1,3,5,.. - sensors error | (isPresent << 15)
#define INPUT_REG_ERROR SENSOR_COUNT*2
#define INPUT_REG_HC05_ERROR INPUT_REG_ERROR+1

#define INPUT_REGS_COUNT INPUT_REG_HC05_ERROR+1

//holding registers
#define HOLDING_REG_MODE 0
#define HOLDING_REGS_TARGETS 1
#define HOLDING_REGS_DIFFLIMITS HOLDING_REGS_TARGETS + SENSOR_COUNT
#define HOLDING_REGS_CORRECTIONS HOLDING_REGS_DIFFLIMITS + SENSOR_COUNT * 2

#define HOLDING_REGS_COUNT HOLDING_REGS_CORRECTIONS + SENSOR_COUNT * 2

//coils
#define COILS_COUNT 0

uint16_t getInputRegister(uint16_t address);
uint16_t getHoldingRegister(uint16_t address);
modbus_errors_e setHoldingRegister(uint16_t address, uint16_t value);
