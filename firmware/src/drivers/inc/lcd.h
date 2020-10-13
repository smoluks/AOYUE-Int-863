#pragma once

#include <stdint.h>
#include <stdbool.h>

void displayInit();
void displayWriteText(char* text, uint8_t page);
void displayWriteHalfText(char* text, uint8_t page, bool secondHalf);
void displayClearHalf(uint8_t page, bool secondHalf);
void displayWriteChar(char c, uint8_t position, uint8_t page);
void UInt8ToString(char* string, uint16_t value);

#define MemoryAddressingMode 0x20
#define MemoryAddressingMode_Horisontal 0x00
#define MemoryAddressingMode_Vertical 0x01
#define MemoryAddressingMode_Page 0x02

#define ColumnAddress 0x21
#define PageAddress 0x22

#define SegmentRemap_Normal 0xA0
#define SegmentRemap_Inverted 0xA1

