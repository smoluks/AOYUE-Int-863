#pragma once

#include <stdint.h>
#include <stdbool.h>

void display_init();
void display_write_text(char* text, uint8_t page);
void display_write_halftext(char* text, uint8_t page, bool secondHalf);
void display_clear_half(uint8_t page, bool secondHalf);
void display_write_char(char c, uint8_t position, uint8_t page);
void UInt8ToString(char* string, uint16_t value);

#define MemoryAddressingMode 0x20
#define MemoryAddressingMode_Horisontal 0x00
#define MemoryAddressingMode_Vertical 0x01
#define MemoryAddressingMode_Page 0x02

#define ColumnAddress 0x21
#define PageAddress 0x22

#define SegmentRemap_Normal 0xA0
#define SegmentRemap_Inverted 0xA1

