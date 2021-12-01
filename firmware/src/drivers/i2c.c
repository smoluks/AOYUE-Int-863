#include <i2c.h>
#include "stm32f1xx.h"
#include "gpio.h"
#include "error.h"

static void makeStartWhileNotReady();
static void makeStart();
static bool sendAddress(bool write);
static bool sendData(uint8_t data);
static uint8_t receiveData(bool ack, bool stop);
static void makeStop();

void i2cInit()
{
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;

    I2C2->CR1 &= ~I2C_CR1_PE;

    GPIOB->BSRR = 0x00000C00;
    GPIOB->CRH = (GPIOB->CRH & 0xFFFF00FF)
                              | GPIO11_CRH(GENERAL_OPEN_DRAIN_FAST)  //sda
                              | GPIO10_CRH(GENERAL_OPEN_DRAIN_FAST); //scl
    while((GPIOB->IDR & 0x0C00) != 0x0C00);

    GPIOB->BSRR = 0x04000000; // scl low
    while((GPIOB->IDR & 0x0C00) != 0x0800);

    GPIOB->BSRR = 0x08000000; // sda low
    while((GPIOB->IDR & 0x0C00) != 0x0000);

    GPIOB->BSRR = 0x00000800; // sda high
    while((GPIOB->IDR & 0x0C00) != 0x0800);

    GPIOB->BSRR = 0x00000400; // scl high
    while((GPIOB->IDR & 0x0C00) != 0x0C00);

    GPIOB->CRH = (GPIOB->CRH & 0xFFFF00FF)
                              | GPIO11_CRH(ALT_OPEN_DRAIN_FAST)  //sda
                              | GPIO10_CRH(ALT_OPEN_DRAIN_FAST); //scl

    I2C2->CR1 = I2C_CR1_SWRST;
    I2C2->CR1 = 0;

    I2C2->CCR = 180; //45;
    I2C2->TRISE = 37; //11;
    I2C2->CR2 = 36;
    I2C2->CR1 = I2C_CR1_PE;
}

uint8_t i2cRead(uint8_t address)
{
    makeStart();
    sendAddress(true);
    sendData(address);
    makeStop();

    makeStart();
    sendAddress(false);
    return receiveData(false, true);
}

void i2cWrite(uint8_t address, uint8_t data)
{
    makeStartWhileNotReady();
    if(!sendData(address))
    {
        setError(ERR_I2C);
    }

    if(!sendData(data))
    {
        setError(ERR_I2C);
    }

    makeStop();
}

void i2cReadBuffer(uint8_t address, uint8_t* data, uint8_t count)
{
    makeStart();
    sendAddress(true);
    sendData(address);
    makeStop();

    makeStart();
    sendAddress(false);
    while(count--)
    {
        *(data++) = receiveData(count, !count);
        address++;
    }
}

void i2cWriteBuffer(uint8_t address, uint8_t* data, uint8_t count)
{
    makeStartWhileNotReady();
    sendData(address);

    uint8_t previousPage = address & 0xF0;
    while(count--)
    {
        if(!sendData(*(data++)))
        {
            setError(ERR_I2C);
        }

        address++;

        if(count && ((address & 0xF8) != previousPage))
        {
            previousPage = address & 0xF8;

            makeStop();
            makeStartWhileNotReady();
            sendData(address);
        }
    }

    makeStop();
}

//---Routine---
void makeStartWhileNotReady()
{
    while(1)
    {
        makeStart();
        if(sendAddress(true))
            return;

        makeStop();
    }
}


void makeStart()
{
    I2C2->CR1 |= I2C_CR1_START;
    while (!(I2C2->SR1 & I2C_SR1_SB));

    (void) I2C2->SR1;
}

uint16_t d;

bool sendAddress(bool write)
{
    I2C2->SR1 &= ~I2C_SR1_AF;

    uint8_t address =  write ? I2CADDR : I2CADDR | 0x01;
    I2C2->DR = address;
    while (!(I2C2->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF)))
    {
        d = I2C2->SR1;
    }

    if(I2C2->SR1 & I2C_SR1_AF)
    {
        return false;
    }

    (void) I2C2->SR1;
    (void) I2C2->SR2;

    return true;
}

bool sendData(uint8_t data)
{
    I2C2->SR1 &= ~I2C_SR1_AF;

    I2C2->DR = data;

    while (!(I2C2->SR1 & (I2C_SR1_TXE | I2C_SR1_AF)));
    return (!(I2C2->SR1 & I2C_SR1_AF));
}

void makeStop()
{
    I2C2->CR1 |= I2C_CR1_STOP;
}

uint8_t receiveData(bool ack, bool stop)
{
    if(ack)
        I2C2->CR1 |= I2C_CR1_ACK;
    else
        I2C2->CR1 &= ~I2C_CR1_ACK;

    if(stop)
        I2C2->CR1 |= I2C_CR1_STOP;

    while (!(I2C2->SR1 & I2C_SR1_RXNE));

    return I2C2->DR;
}

