#include "stm32f1xx.h"
#include "modbus.h"
#include "logic.h"
#include "config.h"

extern work_mode_e work_mode;
extern uint16_t sensors_themperature[SENSOR_COUNT];
extern uint16_t ambient_themperature;
extern config_s config;
extern uint16_t targets_temperature[SENSOR_COUNT];
extern uint8_t value;

uint16_t getInputRegister(uint16_t address) {
	if (address < SENSOR_COUNT)
		return sensors_themperature[address];
	switch (address) {
		case 0x08:
			return ambient_themperature;
		case 0x09:
			return value;
		default:
			return 0;
	}
}

uint16_t getHoldingRegister(uint16_t address) {

	if (address < SENSOR_COUNT)
		return targets_temperature[address];

	if (address >= 0x10 && address < 0x10 + SENSOR_COUNT * 2) {
		address = address - 0x10;
		return address % 2 ? config.speedLimits[address / 2].cold : config.speedLimits[address / 2].heat;
	}

	if (address >= 0x20 && address < 0x20 + SENSOR_COUNT * 2) {
		address = address - 0x20;
		return address % 2 ? config.sensorCorrections[address / 2].multiplicative : config.sensorCorrections[address / 2].additive;
	}

	switch (address) {
		case 0x08:
			return work_mode;
		case 0x30:
			return config.ambientCorrection.additive;
		case 0x31:
			return config.ambientCorrection.multiplicative;
		default:
			return 0;
	}
}

modbus_errors_e setHoldingRegister(uint16_t address, uint16_t value) {

	if (address < SENSOR_COUNT) {
		if (value > 400)
			return ILLEGAL_DATA_VALUE;

		targets_temperature[address] = value;
		return 0;
	}

	if (address >= 0x10 && address < 0x10 + SENSOR_COUNT * 2) {
		if (value > 255)
			return ILLEGAL_DATA_VALUE;

		address = address - 0x10;
		if (address % 2)
			config.speedLimits[address / 2].cold = value;
		else
			config.speedLimits[address / 2].heat = value;

		configSave();
		return 0;
	}

	if (address >= 0x20 && address < 0x20 + SENSOR_COUNT * 2) {
		if (value > 255)
			return ILLEGAL_DATA_VALUE;

		address = address - 0x20;
		if (address % 2)
			config.sensorCorrections[address / 2].multiplicative = value;
		else
			config.sensorCorrections[address / 2].additive = value;

		configSave();
		return 0;
	}

	switch (address) {
		case 0x08:
			work_mode = value;
			break;
		case 0x30:
			config.ambientCorrection.additive = value;
			configSave();
			break;
		case 0x31:
			config.ambientCorrection.multiplicative = value;
			configSave();
			break;
	}

	return 0;
}

