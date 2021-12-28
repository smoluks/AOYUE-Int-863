#include <sensors.h>
#include "stm32f1xx.h"
#include "modbus.h"
#include "config.h"
#include "main.h"
#include "hardwareConfig.h"
#include "modbusHandlers.h"
#include "hc05.h"
#include "logic.h"
#include "bootloader.h"

extern work_mode_e work_mode;
extern sensor_s sensors[SENSOR_COUNT];
extern config_s config;
extern uint16_t targets_temperature[SENSOR_COUNT];
extern bt_error_t bt_error;
extern bool need_update_display;

uint16_t getInputRegister(uint16_t address) {
    if(address == INPUT_REG_HC05_ERROR)
    {
        return bt_error;
    }
    else if(address == INPUT_REG_ERROR)
    {
        return getError();
    }
    else if(address & 0x01)
	{
	    return sensors[address >> 1].error | (sensors[address >> 1].isPresent << 15);
	}
	else
	{
	    return sensors[address >> 1].value;
	}
}

uint16_t getHoldingRegister(uint16_t address) {
    //mode
    if (address == HOLDING_REG_MODE)
    {
        return work_mode;
    }

    //targets
	if (address <  HOLDING_REGS_DIFFLIMITS)
	{
		return config.targets_temperature[address - HOLDING_REGS_TARGETS];
	}

	//speed targets
	if (address < HOLDING_REGS_CORRECTIONS) {
		address = address - HOLDING_REGS_DIFFLIMITS;
		return address & 0x01 ? config.speedLimits[address / 2].cool_speed : config.speedLimits[address / 2].heat_speed;
	}

	//sensor corrections
	if (address < HOLDING_REGS_COUNT) {
		address = address - HOLDING_REGS_CORRECTIONS;
		return address & 0x01 ? config.sensorCorrections[address / 2].multiplicative : config.sensorCorrections[address / 2].additive;
	}

	return 0;
}

modbus_errors_e setHoldingRegister(uint16_t address, uint16_t value) {
    //mode
    if (address == HOLDING_REG_MODE)
    {
        if(value > 2)
            return ILLEGAL_DATA_VALUE;

        setMode(value);
        return 0;
    }

    //targets
    if (address < HOLDING_REGS_DIFFLIMITS)
    {
        if ((value & 0X0FFF) > (MAXTEMP << 4))
            return ILLEGAL_DATA_VALUE;

        setTargetTemperature(address - HOLDING_REGS_TARGETS, value);

        return 0;
    }

    //speed targets
	if (address < HOLDING_REGS_CORRECTIONS) {
	    if (value > 255)
	        return ILLEGAL_DATA_VALUE;

	    address = address - HOLDING_REGS_DIFFLIMITS;
	    if(address & 0x01)
	        config.speedLimits[address / 2].cool_speed = value;
	    else
	        config.speedLimits[address / 2].heat_speed = value;

	    updateConfig();
	    return 0;
	}

	//sensor corrections
	if (address < HOLDING_REGS_COUNT) {
	    address = address - HOLDING_REGS_CORRECTIONS;
	    if(address & 0x01)
	        config.sensorCorrections[address / 2].multiplicative = value;
	    else
	        config.sensorCorrections[address / 2].additive = value;

	    updateConfig();
	    return 0;
	}

	if(address == HOLDING_REGS_GOTOBOOTLOADER)
	{
	    if(value == 0xb001)
	        JumpToBootloader();
	    else return ILLEGAL_DATA_VALUE;
	}

	return ILLEGAL_DATA_ADDRESS;
}

