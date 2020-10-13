#pragma once

void flashWrite(uint8_t *data, uint32_t count);
void flashRead(uint8_t *data, uint32_t count);
uint32_t flashReadUInt32(uint32_t offset);
