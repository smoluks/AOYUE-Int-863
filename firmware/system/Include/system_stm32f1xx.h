#pragma once

#include <stdint.h>

#define nop() asm("NOP")

void SystemInit(void);
void StartUart();

