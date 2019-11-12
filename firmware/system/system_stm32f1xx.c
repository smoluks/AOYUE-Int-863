#include "stm32f1xx.h"
#include "delay.h"
#include "config.h"

void __attribute__ ((weak)) _init(void) {}
void systick_init();
uint8_t SystemCoreClockUpdate(void);

extern struct ramconfig_s ramconfig;

void SystemInit(void)
{
	RCC->APB1ENR = RCC_APB1ENR_PWREN | RCC_APB1ENR_USART2EN | RCC_APB1ENR_TIM3EN | RCC_APB1ENR_TIM4EN;// | RCC_APB1ENR_SPI2EN | RCC_APB1ENR_PWREN;
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_SPI1EN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_ADC1EN | RCC_APB2ENR_ADC2EN | RCC_APB2ENR_TIM1EN;//
	AFIO->MAPR = AFIO_MAPR_SPI1_REMAP | AFIO_MAPR_SWJ_CFG_1;
	//porta
	GPIOA->CRH = 0x28844444;
	GPIOA->CRL = 0x00000000;
	GPIOA->ODR = 0x00002000;
	//portb
	GPIOB->CRH = 0x33384433;
	GPIOB->CRL = 0x33B8B337;
	GPIOB->ODR = 0x00000055;
	//portc
	GPIOC->CRH = 0x88800000;
	GPIOC->CRL = 0x00000000;
	GPIOC->ODR = 0x0000E000;
	//spi
	SPI1->CR1 = SPI_CR1_SSI | SPI_CR1_SSM | SPI_CR1_SPE | SPI_CR1_MSTR | SPI_CR1_BR_1;
	//----ADC1-----
	ADC1->SMPR2 = 0x0002F000;
	ADC1->CR2 |= ADC_CR2_JEXTSEL;
	ADC1->CR2 |= ADC_CR2_JEXTTRIG;
	//ADC1->CR2 |= ADC_CR2_CONT;
	ADC1->CR1 |= ADC_CR1_SCAN  | ADC_CR1_JEOCIE;
	ADC1->JSQR = (1 << ADC_JSQR_JL_Pos) | (4<<ADC_JSQR_JSQ3_Pos) | (5<<ADC_JSQR_JSQ4_Pos);
	ADC1->CR2 |= ADC_CR2_ADON;
	NVIC_EnableIRQ(ADC1_2_IRQn);
	//-----ADC2-----
	ADC2->SMPR2 = 0x00FC0000;
	ADC2->CR2 |= ADC_CR2_JEXTSEL;
	ADC2->CR2 |= ADC_CR2_JEXTTRIG;
	//ADC2->CR2 |= ADC_CR2_CONT;
	ADC2->CR1 |= ADC_CR1_SCAN;
	ADC2->JSQR = (1 << ADC_JSQR_JL_Pos) | (6<<ADC_JSQR_JSQ3_Pos) | (7<<ADC_JSQR_JSQ4_Pos);
	ADC2->CR2 |= ADC_CR2_ADON;
	//прогрев АЦП
	delay(1);
	//Запуск калибровки обоих АЦП
	ADC2->CR2 |= ADC_CR2_CAL;
	ADC1->CR2 |= ADC_CR2_CAL;
		while ((ADC2->CR2 & ADC_CR2_CAL) || (ADC1->CR2 & ADC_CR2_CAL)){};
	//EINT
	AFIO->EXTICR[1] = 1 << AFIO_EXTICR2_EXTI4_Pos;
	EXTI->IMR = 0x00000010;
	EXTI->RTSR = 0x00000010;
	EXTI->FTSR = 0x00000010;
	NVIC_EnableIRQ(EXTI4_IRQn);
	//TIM4 - задержка перед ADC
	TIM4->DIER = TIM_DIER_UIE;
	TIM4->PSC = 720-1;
	TIM4->ARR = 993;
	TIM4->CR1 = TIM_CR1_ARPE | TIM_CR1_OPM;
	NVIC_EnableIRQ(TIM4_IRQn);
	//
	ramconfig.mainerror = SystemCoreClockUpdate();
	//
	systick_init();
	//
	delay_init();
	//
	__enable_irq();
}

void StartUart()
{
	GPIOA->CRL |= 0x00008BB8;
	GPIOA->BSRR = 0x00000009;
	//UART2
	USART2->BRR = 938;
	USART2->CR2 = 0x00000000;
	USART2->CR3 = 0x00000000;
	USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
	NVIC_EnableIRQ(USART2_IRQn);
}

uint8_t SystemCoreClockUpdate()
{
	//включаем внешний кварц
	RCC->CR = RCC_CR_HSEON;
	uint32_t timeout = 0;
	while (!(RCC->CR & RCC_CR_HSERDY) && timeout++ != 300000) //ждем готовность внешнего кварца
	{
	}
	if (!(RCC->CR & RCC_CR_HSERDY))
		return ERR_CRYSTAL;

	FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_1; //делитель частоты для FLASH памяти
	RCC->CFGR = RCC_CFGR_PLLMULL9 | RCC_CFGR_PLLSRC | RCC_CFGR_ADCPRE_DIV6 | RCC_CFGR_PPRE2_DIV1 | RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_HPRE_DIV1;

	RCC->CR |= RCC_CR_PLLON; //стартуем PLL
	timeout = 0;
	while (!(RCC->CR & RCC_CR_PLLRDY) && timeout++ != 300000) //ждем готовности PLL
	{
	}
	if (!(RCC->CR & RCC_CR_PLLRDY))
		return ERR_PLL;

	RCC->CFGR = RCC->CFGR | RCC_CFGR_SW_PLL; //переключаемся на pll
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) //ждем переключения на pll
	{
	}

	return ERR_NOERROR;
}

#define F_CPU 		72000000UL	// Тактовая у нас 72МГЦ
#define TimerTick  	F_CPU/1000-1	// Нам нужен килогерц

void systick_init()
{
	SysTick->LOAD = TimerTick;		// Загрузка значения
	SysTick->VAL = TimerTick;	// Обнуляем таймеры и флаги. Записью, помните?

	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
	SysTick_CTRL_TICKINT_Msk |
	SysTick_CTRL_ENABLE_Msk;

}

