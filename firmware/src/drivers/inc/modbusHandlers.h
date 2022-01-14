#pragma once
#include "hardwareConfig.h"

// ----- modbus registers -----
//input registers
#define INPUT_REG_SENSORCOUNT 0
#define INPUT_REG_ERROR 1
#define INPUT_REG_HC05_ERROR 2

#define INPUT_REGS_SENSORS 4

#define INPUT_REGS_COUNT 20

//holding registers
#define HOLDING_REG_MODE 0
#define HOLDING_REGS_TARGETS 1
#define HOLDING_REGS_DIFFLIMITS 9
#define HOLDING_REGS_CORRECTIONS 0x19
#define HOLDING_REGS_AMBIENT_CORRECTION 0x29
#define HOLDING_REGS_PIDCOEFS 0x2B
#define HOLDING_REGS_BEEPENABLE 0x39

#define HOLDING_REGS_COUNT 58

//coils
#define COILS_COUNT 0

uint16_t getInputRegister(uint16_t address);
uint16_t getHoldingRegister(uint16_t address);
modbus_errors_e setHoldingRegister(uint16_t address, uint16_t value);
