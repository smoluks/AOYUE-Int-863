#include "gpio.h"
#include "stm32f1xx.h"

inline void setIOBits(GPIO_TypeDef* port, uint16_t mask)
{
	port->BSRR = mask;
}

inline void resetIOBits(GPIO_TypeDef* port, uint16_t mask)
{
	port->BSRR = (mask << 16);
}
