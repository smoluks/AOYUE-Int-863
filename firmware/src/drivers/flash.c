#include <string.h>
#include "stm32f1xx.h"
#include "stdbool.h"

#define FLASH_KEY1 ((uint32_t)0x45670123)
#define FLASH_KEY2 ((uint32_t)0xCDEF89AB)

#define PAGE_ADDRESS 0x0800FC00

void flashWritePage(uint32_t address, uint8_t *data, uint32_t count);
void flashErasePage(uint32_t address);
void flashUnlock();
void flashLock();

void flashWrite(uint8_t *data, uint32_t count) {

	flashUnlock();

	flashErasePage(PAGE_ADDRESS);

	flashWritePage(PAGE_ADDRESS, data, count);

	flashLock();
}

uint32_t flashReadUInt32(uint32_t offset) {
	return *(uint32_t*) (PAGE_ADDRESS + offset);
}

void flashRead(uint8_t *data, uint32_t count) {
	memcpy(data, PAGE_ADDRESS, count);
}

//page size - 1KB
void flashErasePage(uint32_t address) {
	while (FLASH->SR & FLASH_SR_BSY);

	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR |= FLASH_CR_PER;
	FLASH->AR = address;
	FLASH->CR |= FLASH_CR_STRT;
	while (!(FLASH->SR & FLASH_SR_EOP));

	FLASH->SR = FLASH_SR_EOP;
	FLASH->CR &= ~FLASH_CR_PER;
}

void flashWritePage(uint32_t address, uint8_t *data, uint32_t count) {
	while (FLASH->SR & FLASH_SR_BSY);

	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR |= FLASH_CR_PG;

	uint32_t fadress = (uint32_t)data + count;

	while ((uint32_t)data < fadress) {
		*(uint16_t*) address = *(uint16_t*) data;
		address += 2;
		data += 2;

		while (!(FLASH->SR & FLASH_SR_EOP));
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR &= ~(FLASH_CR_PG);
}

void flashUnlock() {
	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2;
}

void flashLock() {
	FLASH->CR |= FLASH_CR_LOCK;
}
