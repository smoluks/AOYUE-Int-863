#pragma once

void uart2Init();
void uart2GotoModbusMode();
bool uart2SendAtCommand(char *command, char *buffer);

void USART2_IRQHandler();
