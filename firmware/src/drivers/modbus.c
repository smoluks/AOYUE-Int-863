#include "stm32f1xx.h"
#include "modbus.h"
#include "modbusHandlers.h"

static volatile uint8_t outputbuffer[MODBUS_TRANSMIT_BUFFER_SIZE];
static volatile uint8_t outputhandle = 0;
static volatile uint8_t inputbuffer[MODBUS_RECEIVE_BUFFER_SIZE];
static volatile uint8_t inputhandle = 0;

void readInputRegisters();
void readHoldingRegisters();
void writeHoldingRegister();
//void read_coils();
void sendError(modbus_command_e func, modbus_errors_e error) ;
void addCrc();
bool checkCrc();

void modbusInit()
{
    //timer for timeout
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    //3.5 Sym - 315uS
    TIM4->SR &= ~(TIM_SR_UIF);
    TIM4->DIER = TIM_DIER_UIE;
    TIM4->PSC = 72-1;
    TIM4->ARR = 315;
    TIM4->CR1 = TIM_CR1_ARPE | TIM_CR1_OPM;
    NVIC_EnableIRQ(TIM4_IRQn);
}

//process packet
void TIM4_IRQHandler()
{
    TIM4->SR &= ~TIM_SR_UIF;
    (void) USART1->DR;

	if (!inputhandle
	        || (inputbuffer[0] && inputbuffer[0] != MODBUS_ADDRESS)
	        || !checkCrc())
	{
	    inputhandle = 0;
		return;
	}

	switch (inputbuffer[1]) {
		//case READ_COILS: {
		//		read_coils();
		//		break;
		//	}
		case READ_INPUT_REGISTERS:
			readInputRegisters();
			break;
		case READ_HOLDING_REGISTERS:
			readHoldingRegisters();
			break;
		case WRITE_SINGLE_REGISTER:
			writeHoldingRegister();
			break;
		default:
			sendError(inputbuffer[1], ILLEGAL_FUNCTION);
	}
	addCrc();

	modbusTxHandler();
	inputhandle = 0;
}

void modbusTxHandler() {
	if (outputhandle) {
	    USART1->CR1 |= USART_CR1_TXEIE;
		USART1->DR = outputbuffer[--outputhandle];
	} else
	{
	    USART1->CR1 &= ~USART_CR1_TXEIE;
	}
}

//all input save to buffer
void modbusRxHandler() {
	uint8_t i = USART1->DR;

	if (inputhandle < MODBUS_RECEIVE_BUFFER_SIZE) {
		inputbuffer[inputhandle++] = i;
	}

	TIM4->CNT = 0;
	TIM4->CR1 |= TIM_CR1_CEN;
}

/*void read_coils() {
 uint16_t startaddress = (inputbuffer[2] << 8) + inputbuffer[3];
 uint16_t count = (inputbuffer[4] << 8) + inputbuffer[5];

 if (!count || (startaddress + count > COILS_COUNT)) {
 send_error(READ_COILS, ILLEGAL_DATA_ADDRESS);
 return;
 }

 outputhandle = 2;
 //outputbuffer[outputhandle++] = (button1_pressed << COIL_BUTTON1) | (button2_pressed << COIL_BUTTON2);
 outputbuffer[outputhandle++] = 1;
 outputbuffer[outputhandle++] = READ_COILS;
 outputbuffer[outputhandle++] = MODBUS_ADDRESS;
 }*/

void readInputRegisters() {

	uint16_t startaddress = (inputbuffer[2] << 8) + inputbuffer[3];
	uint16_t count = (inputbuffer[4] << 8) + inputbuffer[5];

	if (!count || (startaddress + count > INPUT_REGS_COUNT)) {
		sendError(READ_INPUT_REGISTERS, ILLEGAL_DATA_ADDRESS);
		return;
	}

	outputhandle = 2;
	for (int16_t i = startaddress + count - 1; i >= startaddress; i--) {
		uint16_t value = getInputRegister(startaddress + i);

		outputbuffer[outputhandle++] = value;
		outputbuffer[outputhandle++] = value >> 8;
	}

	outputbuffer[outputhandle++] = count * 2;
	outputbuffer[outputhandle++] = READ_INPUT_REGISTERS;
	outputbuffer[outputhandle++] = MODBUS_ADDRESS;
}

void readHoldingRegisters() {

	uint16_t startaddress = (inputbuffer[2] << 8) + inputbuffer[3];
	uint16_t count = (inputbuffer[4] << 8) + inputbuffer[5];

	if (!count || (startaddress + count > HOLDING_REGS_COUNT)) {
		sendError(READ_HOLDING_REGISTERS, ILLEGAL_DATA_ADDRESS);
		return;
	}

	outputhandle = 2;
	for (int16_t i = startaddress + count - 1; i >= startaddress; i--) {
		uint16_t value = getHoldingRegister(startaddress + i);

		outputbuffer[outputhandle++] = value;
		outputbuffer[outputhandle++] = value >> 8;
	}

	outputbuffer[outputhandle++] = count * 2;
	outputbuffer[outputhandle++] = READ_HOLDING_REGISTERS;
	outputbuffer[outputhandle++] = MODBUS_ADDRESS;
}

void writeHoldingRegister() {

	uint16_t address = (inputbuffer[2] << 8) + inputbuffer[3];
	uint16_t value = (inputbuffer[4] << 8) + inputbuffer[5];

	if (address >= HOLDING_REGS_COUNT) {
		sendError(WRITE_SINGLE_REGISTER, ILLEGAL_DATA_ADDRESS);
		return;
	}

	modbus_errors_e error = setHoldingRegister(address, value);
	if(error){
		sendError(WRITE_SINGLE_REGISTER, error);
		return;
	}

	outputhandle = 2;
	outputbuffer[outputhandle++] = value;
	outputbuffer[outputhandle++] = value >> 8;
	outputbuffer[outputhandle++] = address;
	outputbuffer[outputhandle++] = address >> 8;
	outputbuffer[outputhandle++] = WRITE_SINGLE_REGISTER;
	outputbuffer[outputhandle++] = MODBUS_ADDRESS;
}

void sendError(modbus_command_e func, modbus_errors_e error) {
	outputbuffer[2] = error;
	outputbuffer[3] = func | 0x80;
	outputbuffer[4] = MODBUS_ADDRESS;
	outputhandle = 5;
}

void addCrc() {
	uint16_t crc = 0xFFFF;
	uint8_t i, j;

	for (j = outputhandle - 1; j > 1; j--) {
		crc ^= outputbuffer[j];
		for (i = 0; i < 8; i++) {
			if (crc & 0x0001) {
				crc >>= 1;
				crc ^= 0xA001;
			} else {
				crc >>= 1;
			}
		}
	}
	outputbuffer[0] = crc >> 8;
	outputbuffer[1] = crc;
}

bool checkCrc() {
	uint16_t crc = 0xFFFF;

	uint8_t i, j = 0;
	for (j = 0; j < inputhandle - 2; j++) {
		crc = ((crc ^ inputbuffer[j]) | 0xFF00) & (crc | 0x00FF);
		for (i = 0; i < 8; i++) {
			if (crc & 0x0001) {
				crc >>= 1;
				crc ^= 0xA001;
			} else {
				crc >>= 1;
			}
		}
	}
	return ((inputbuffer[inputhandle - 2] == (uint8_t) crc) && (inputbuffer[inputhandle - 1] == (uint8_t) (crc >> 8)));
}
