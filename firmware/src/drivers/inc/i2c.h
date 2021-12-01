#pragma once

#include "stm32f1xx.h"

#define I2CADDR 0xA0

void i2cInit();
uint8_t i2cRead(uint8_t address);
void i2cWrite(uint8_t address, uint8_t data);
void i2cReadBuffer(uint8_t address, uint8_t* data, uint8_t count);
void i2cWriteBuffer(uint8_t address, uint8_t* data, uint8_t count);
