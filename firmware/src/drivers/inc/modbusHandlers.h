#pragma once

uint16_t getInputRegister(uint16_t address);
uint16_t getHoldingRegister(uint16_t address);
modbus_errors_e setHoldingRegister(uint16_t address, uint16_t value);
