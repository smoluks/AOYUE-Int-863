#pragma once

void process_command(uint8_t* command, uint8_t length);

void transmit_data(uint8_t* data, uint8_t length);
void trasmit_text(const char* text);
void trasmit_byte(uint8_t byte);
void transmit_void();

void transmit_commandnotaccepted();
void transmit_commandbadchannel();
void transmit_commandnotfound();
void transmit_commandbadlength();

