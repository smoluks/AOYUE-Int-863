#include "gpio.h"
#include "max31856.h"
#include "sensors.h"
#include "hardwareConfig.h"

static void max31856InitInternal(uint8_t chip, uint8_t type);
static void max31856ReadInternal(uint8_t chip, sensor_s* sensorData);
static sensor_error_t convertMax31856Error(uint8_t statusReg);

static uint8_t readRegister(uint8_t chip, uint8_t regAddress);
static void writeRegister(uint8_t chip, uint8_t regAddress, uint8_t value);
static uint16_t readWordRegister(uint8_t chip, uint8_t regAddress);

static uint8_t spiByteExchange(uint8_t data);

extern sensor_s sensors[SENSOR_COUNT];

#ifdef MAX31856_TAKE_AMBIENT
static sensor_s ambients[3];
#endif

void max31856Init() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_SPI1EN;
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    SPI1->CR1 = SPI_CR1_SSI | SPI_CR1_SSM | SPI_CR1_SPE | SPI_CR1_MSTR | SPI_CR1_BR_1 | SPI_CR1_BR_0 | SPI_CR1_CPOL | SPI_CR1_CPHA; // | SPI_CR1_DFF;

    //spi
    GPIOA->BSRR |= 0x00000040;
    GPIOA->CRL = (GPIOA->CRL & 0x000FFFFF)
                  | (ALT_PUSH_PULL_FAST << (5 * 4)) //mosi
                  | (INPUT_PULL << (6 * 4)) //miso
                  | (ALT_PUSH_PULL_FAST << (7 * 4)); //sck

    //cs0-2
    GPIOB->BSRR |= 0x00000007;
    GPIOB->CRL = (GPIOB->CRL & 0xFFFFF000)
                  | (GENERAL_PUSH_PULL_FAST << (0 * 4))  //cs0
                  | (GENERAL_PUSH_PULL_FAST << (1 * 4))//cs1
                  | (GENERAL_PUSH_PULL_FAST << (2 * 4));//cs2

#ifdef MAX31856_0_CHANNEL
    max31856InitInternal(0, MAX31856_0_TYPE);
#endif
#ifdef MAX31856_1_CHANNEL
    max31856InitInternal(1, MAX31856_1_TYPE);
#endif
#ifdef MAX31856_2_CHANNEL
    max31856InitInternal(2, MAX31856_2_TYPE);
#endif
}

static void max31856InitInternal(uint8_t chip, uint8_t type) {
    writeRegister(chip, MAX31856_MASK_REG, 0);
    writeRegister(chip, MAX31856_CR0, MAX31856_CR0_CMODE | MAX31856_CR0_OCFAULT0 | MAX31856_CR0_5060Hz);
    writeRegister(chip, MAX31856_CR1, type | MAX31856_CR1_16SAMPLES);
}

void max31856Read() {
#ifdef MAX31856_0_CHANNEL
    max31856ReadInternal(0, &sensors[MAX31856_0_CHANNEL]);
#endif
#ifdef MAX31856_1_CHANNEL
    max31856ReadInternal(1, &sensors[MAX31856_1_CHANNEL]);
#endif
#ifdef MAX31856_2_CHANNEL
    max31856ReadInternal(2, &sensors[MAX31856_2_CHANNEL]);
#endif
#ifdef MAX31856_TAKE_AMBIENT
    if(ambients[2].isPresent)
        max31856_termocouples[SENSOR_AMBIENT] = ambients[2];
    else if (ambients[1].isPresent)
        max31856_termocouples[SENSOR_AMBIENT] = ambients[1];
    else
        max31856_termocouples[SENSOR_AMBIENT] = ambients[0];
#endif
}

static void max31856ReadInternal(uint8_t chip, sensor_s* sensorData) {
    uint8_t statusReg = readRegister(chip, MAX31856_SR_REG);
    if (statusReg) {
        sensorData->isPresent = false;
        sensorData->error = convertMax31856Error(statusReg);

#ifdef MAX31856_TAKE_AMBIENT
        ambients->isPresent = false;
        ambients->error = sensorData->error;
#endif
    } else {
        sensorData->isPresent = true;
        sensorData->value = readWordRegister(chip, MAX31856_LTCBH_REG);
        sensorData->error = SERR_NOERROR;

#ifdef MAX31856_TAKE_AMBIENT
        ambients[chip].value = readWordRegister(port, mask, MAX31856_CJTH_REG) >> 4;
        ambients->isPresent = true;
        ambients->error = SERR_NOERROR;
#endif
    }
}

static sensor_error_t convertMax31856Error(uint8_t statusReg)
{
    if(statusReg == 0xFF)
        return SERR_NOCHIP;

    uint8_t i = SERR_OPEN;
    do
    {
        if(statusReg & 0x01)
        {
            return (sensor_error_t)i;
        }
        i++;
        statusReg = statusReg >> 1;
    }
    while(i < SERR_OPEN + 8);

    return SERR_NOERROR;
}

static uint8_t readRegister(uint8_t chip, uint8_t regAddress) {
    //cs 0
    resetIOBits(GPIOB, 1 << chip);

    spiByteExchange(regAddress);

    uint8_t result = spiByteExchange(0xFF);

    //cs 1
    setIOBits(GPIOB, 1 << chip);

    return result;
}

static void writeRegister(uint8_t chip, uint8_t regAddress, uint8_t value) {
    //cs 0
    resetIOBits(GPIOB, 1 << chip);

    spiByteExchange(regAddress | 0x80);

    spiByteExchange(value);

    //cs 1
    setIOBits(GPIOB, 1 << chip);
}

static uint16_t readWordRegister(uint8_t chip, uint8_t regAddress) {
    //cs 0
    resetIOBits(GPIOB, 1 << chip);

    spiByteExchange(regAddress);

    uint16_t result = (spiByteExchange(0xFF) << 8) | spiByteExchange(0xFF);

    //cs 1
    setIOBits(GPIOB, 1 << chip);

    return result;
}

static uint8_t spiByteExchange(uint8_t data) {
    SPI1->DR = data;

    while (!(SPI1->SR & SPI_SR_RXNE));

    return SPI1->DR;
}
