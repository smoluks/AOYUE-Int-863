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
    if(address == INPUT_REG_SENSORCOUNT)
    {
        //sensors count
        return SENSOR_COUNT;
    }
    else if(address == INPUT_REG_HC05_ERROR)
    {
        //bt error
        return bt_error;
    }
    else if(address == INPUT_REG_ERROR)
    {
        //error
        return getError();
    }
    else
    {
        //sensors
        address -= INPUT_REGS_SENSORS;
        uint8_t sensorNumber = address / 2;
        if(sensorNumber >= SENSOR_COUNT)
            return 0;

        if(address & 0x01)
            return sensors[sensorNumber].error;
        else
            return sensors[sensorNumber].value  | (sensors[sensorNumber].isPresent << 15);
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
	    uint8_t sensorNumber = address - HOLDING_REGS_TARGETS;
	    if(sensorNumber >= SENSOR_COUNT)
	        return 0;

		return config.targetTemperatures[address - HOLDING_REGS_TARGETS];
	}

	//speed targets
	if (address < HOLDING_REGS_CORRECTIONS) {
		address = address - HOLDING_REGS_DIFFLIMITS;
		uint8_t sensorNumber = address / 2;
		if(sensorNumber >= SENSOR_COUNT)
		    return 0;

		return address & 0x01 ? config.speedLimits[sensorNumber].coolSpeed : config.speedLimits[sensorNumber].heatSpeed;
	}

	//sensor corrections
	if (address < HOLDING_REGS_AMBIENT_CORRECTION) {
		address = address - HOLDING_REGS_CORRECTIONS;
		uint8_t sensorNumber = address / 2;
		if(sensorNumber >= SENSOR_COUNT)
		    return 0;

		return address & 0x01 ? config.sensorCorrections[sensorNumber].multiplicative : config.sensorCorrections[sensorNumber].additive;
	}

	//ambient correction
	if (address < HOLDING_REGS_PIDCOEFS) {
	    return address & 0x01 ? config.ambientCorrection.multiplicative : config.ambientCorrection.additive;
	}

	//beep enable
	if(address == HOLDING_REGS_BEEPENABLE)
	    return config.beepEnable;

	//pid coefs
	address = address - HOLDING_REGS_PIDCOEFS;
	return *((uint16_t*)config.pidCoef + address);
}

modbus_errors_e setHoldingRegister(uint16_t address, uint16_t value) {
    //mode
    if (address == HOLDING_REG_MODE)
    {
        if(value == 0xb001)
            JumpToBootloader();

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
	        config.speedLimits[address / 2].coolSpeed = value;
	    else
	        config.speedLimits[address / 2].heatSpeed = value;

	    updateConfig();
	    return 0;
	}

	//sensor corrections
	if (address < HOLDING_REGS_AMBIENT_CORRECTION) {
	    address = address - HOLDING_REGS_CORRECTIONS;
	    if(address & 0x01)
	        config.sensorCorrections[address / 2].multiplicative = value;
	    else
	        config.sensorCorrections[address / 2].additive = value;

	    updateConfig();
	    return 0;
	}

	//ambient correction
	if (address < HOLDING_REGS_PIDCOEFS) {
	    if(address & 0x01)
	        config.ambientCorrection.multiplicative = value;
	    else
	        config.ambientCorrection.additive = value;

	    updateConfig();
	    return 0;
	}

	//beep enable
	if(address == HOLDING_REGS_BEEPENABLE)
	{
	    config.beepEnable = value;
	    updateConfig();
	    return 0;
	}

	//pid coefs
	address = address - HOLDING_REGS_PIDCOEFS;
	*((uint16_t*)config.pidCoef + address) = value;

	updateConfig();
	return 0;
}

