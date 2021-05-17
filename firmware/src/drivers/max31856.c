#include "gpio.h"
#include "max31856.h"
#include "sensors.h"

#define CS0_DOWN() GPIOB->BSRR = BSRR_RESET(1 << 0)
#define CS0_UP() GPIOB->BSRR = BSRR_SET(1 << 0)
#define CS1_DOWN() GPIOB->BSRR = BSRR_RESET(1 << 1)
#define CS1_UP() GPIOB->BSRR = BSRR_SET(1 << 1)
#define CS2_DOWN() GPIOB->BSRR = BSRR_RESET(1 << 2)
#define CS2_UP() GPIOB->BSRR = BSRR_SET(1 << 2)

static void max31856InitInternal(uint8_t chip);
static void max31856ReadInternal(uint8_t chip);

static uint8_t readRegister(uint8_t chip, uint8_t regAddress);
static void writeRegister(uint8_t chip, uint8_t regAddress, uint8_t value);
static uint16_t readWordRegister(uint8_t chip, uint8_t regAddress);

inline static void csDown(uint8_t chip);
inline static void csUp(uint8_t chip);
static uint8_t spiByteExchange(uint8_t data);

sensor_s max31856_termocouples[MAX31856_COUNT];
sensor_s max31856_ambient;

void max31856Init()
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_SPI1EN;
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
  SPI1->CR1 = SPI_CR1_SSI | SPI_CR1_SSM | SPI_CR1_SPE | SPI_CR1_MSTR | SPI_CR1_BR_1 | SPI_CR1_BR_0 | SPI_CR1_CPOL | SPI_CR1_CPHA; // | SPI_CR1_DFF;

  //spi
  GPIOA->BSRR = 0x00000040;
  GPIOA->CRL = (GPIOA->CRL & 0x000FFFFF) | (ALT_PUSH_PULL_FAST << (5 * 4)) | //mosi
  (INPUT_PULL << (6 * 4)) | //miso
  (ALT_PUSH_PULL_FAST << (7 * 4)); //sck

  //cs0-2
  GPIOB->BSRR = 0x00000007;
  GPIOB->CRL = (GPIOB->CRL & 0xFFFFF000) | (GENERAL_PUSH_PULL_FAST << (0 * 4)) | //cs0
  (GENERAL_PUSH_PULL_FAST << (1 * 4)) |  //cs1
  (GENERAL_PUSH_PULL_FAST << (2 * 4));  //cs2

  for (uint8_t i = 0; i < MAX31856_COUNT; i++)
  {
	max31856InitInternal(i);
  }
}

static void max31856InitInternal(uint8_t chip)
{
  writeRegister (chip, MAX31856_MASK_REG, 0);
  writeRegister (chip, MAX31856_CR0, MAX31856_CR0_CMODE | MAX31856_CR0_OCFAULT0 | MAX31856_CR0_5060Hz);
  writeRegister (chip, MAX31856_CR1, MAX31856_CR1_16SAMPLES | MAX31856_CR1_TYPE_K);
}

void max31856Read()
{
  for (uint8_t i = 0; i < MAX31856_COUNT; i++)
  {
	max31856ReadInternal(i);
  }
}

static void max31856ReadInternal(uint8_t chip)
{
  uint8_t errors = readRegister(chip, MAX31856_SR_REG);
  if(errors)
  {
	max31856_termocouples[chip].isPresent = false;
  }
  else
  {
	max31856_termocouples[chip].isPresent = true;

	max31856_termocouples[chip].value = readWordRegister(chip, MAX31856_LTCBH_REG);

	max31856_ambient.value = readWordRegister(chip, MAX31856_CJTH_REG) >> 4;
  }
}

static uint8_t readRegister(uint8_t chip, uint8_t regAddress)
{
  //cs 0
  csDown(chip);

  spiByteExchange (regAddress);

  uint8_t result = spiByteExchange (0xFF);

  //cs 1
  csUp(chip);

  return result;
}

static void writeRegister(uint8_t chip, uint8_t regAddress, uint8_t value)
{
  //cs 0
  csDown(chip);

  spiByteExchange (regAddress | 0x80);

  spiByteExchange (value);

  //cs 1
  csUp(chip);
}

static uint16_t readWordRegister(uint8_t chip, uint8_t regAddress)
{
  //cs 0
  csDown(chip);

  spiByteExchange (regAddress);

  uint16_t result = (spiByteExchange (0xFF) << 8) | spiByteExchange (0xFF);

  //cs 1
  csUp(chip);

  return result;
}

inline static void csDown(uint8_t chip)
{
  switch (chip)
  {
	case 0:
	  CS0_DOWN();
	  break;
	case 1:
	  CS1_DOWN();
	  break;
	case 2:
	  CS2_DOWN();
	  break;
  }
}

inline static void csUp(uint8_t chip)
{
  switch (chip)
  {
	case 0:
	  CS0_UP();
	  break;
	case 1:
	  CS1_UP();
	  break;
	case 2:
	  CS1_UP();
	  break;
  }
}

static uint8_t spiByteExchange(uint8_t data)
{
  SPI1->DR = data;

  while (!(SPI1->SR & SPI_SR_RXNE));

  return SPI1->DR;
}
