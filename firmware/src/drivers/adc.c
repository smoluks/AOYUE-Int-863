#include <sensors.h>
#include "stm32f1xx.h"
#include "config.h"
#include "lcd.h"
#include "adc.h"
#include "main.h"

void process_termocouples(int16_t values[3], int16_t zero);
void process_button(int16_t button_raw);

sensor_s analog_termocouples[3];
enum button_e current_button;

void adcInit()
{
	GPIOA->CRL = GPIOA->CRL & 0xFFF00000;
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN | RCC_APB2ENR_ADC2EN;

	ADC1->SMPR2 = 7 << (3 * 1) | 7 << (3 * 2) | 7 << (3 * 3) | 7 << (3 * 4);
	ADC1->CR2 = ADC_CR2_JEXTSEL | ADC_CR2_JEXTTRIG;
	ADC1->CR1 |= ADC_CR1_SCAN | ADC_CR1_JEOCIE;
	ADC1->JSQR = (3 << ADC_JSQR_JL_Pos) | (3 << ADC_JSQR_JSQ1_Pos) | (2 << ADC_JSQR_JSQ2_Pos) | (1 << ADC_JSQR_JSQ3_Pos) | (4 << ADC_JSQR_JSQ4_Pos);
	ADC1->CR2 |= ADC_CR2_ADON;

	ADC2->SMPR2 = 7 << (3 * 0);
	ADC2->CR2 = ADC_CR2_JEXTSEL | ADC_CR2_JEXTTRIG;
	ADC2->CR1 |= ADC_CR1_SCAN | ADC_CR1_JEOCIE;
	ADC2->JSQR = (0 << ADC_JSQR_JL_Pos) | (0 << ADC_JSQR_JSQ4_Pos);
	ADC2->CR2 |= ADC_CR2_ADON;

	//ADC calibration
	displayWriteText("Calibrating ADC", 0);
	ADC2->CR2 |= ADC_CR2_CAL;
	ADC1->CR2 |= ADC_CR2_CAL;
	while ((ADC2->CR2 & ADC_CR2_CAL) || (ADC1->CR2 & ADC_CR2_CAL));

	NVIC_EnableIRQ(ADC1_2_IRQn);
	ADC1->CR2 |= ADC_CR2_JSWSTART;
	ADC2->CR2 |= ADC_CR2_JSWSTART;
}

static uint16_t adc1_window[4];
static uint8_t adc1_count = 0;
static uint16_t button_raw = 0;
static uint8_t button_count = 0;
void ADC1_2_IRQHandler() {
	if (ADC1->SR & ADC_SR_JEOC) {

		adc1_window[0] += ADC1->JDR1;
		adc1_window[1] += ADC1->JDR2;
		adc1_window[2] += ADC1->JDR3;
		adc1_window[3] += ADC1->JDR4;

		if(++adc1_count == 16)
		{
			adc1_window[0] = adc1_window[0] >> 4;
			adc1_window[1] = adc1_window[1] >> 4;
			adc1_window[2] = adc1_window[2] >> 4;
			adc1_window[3] = adc1_window[3] >> 4;

			process_termocouples((int16_t*)adc1_window, adc1_window[3]);

			adc1_window[0] = 0;
			adc1_window[1] = 0;
			adc1_window[2] = 0;
			adc1_window[3] = 0;
			adc1_count = 0;
		}

		ADC1->SR &= ~ADC_SR_JEOC;
		ADC1->CR2 |= ADC_CR2_JSWSTART;
	}

	if (ADC2->SR & ADC_SR_JEOC) {

		button_raw += ADC2->JDR1;

		if(++button_count == 16)
		{
			process_button(button_raw >> 4);
			button_raw = 0;
			button_count = 0;
		}

		ADC2->SR &= ~ADC_SR_JEOC;
		ADC2->CR2 |= ADC_CR2_JSWSTART;
	}
}

// 16 bits, 4 floating
int16_t add_correction[3] = {0, 0, 0};
int16_t mul_correction[3] = {1, 1, 1};

void process_termocouples(int16_t values[3], int16_t zero)
{
	for(uint8_t i = 0; i < 3; i++)
	{
		if(values[i] < zero)
		{
			analog_termocouples[i].isPresent = false;
		} else {
			analog_termocouples[i].isPresent = true;
			analog_termocouples[i].value = (values[i] - zero - add_correction[i]) * mul_correction[i];
		}
	}
}

static int16_t oldbuttonvalue = 0;
void process_button(int16_t button_raw)
{
	if(button_raw > (oldbuttonvalue + 20 ) || button_raw < (oldbuttonvalue - 20))
	{
		oldbuttonvalue = button_raw;
		return;
	}

	if(button_raw > 4095 * 5 / 6)
	{
		current_button = button_none;
	} else if (button_raw > 4095 * 3 / 6)
	{
		current_button = button_up;
	} else if (button_raw > 4095 * 1 / 6)
	{
		current_button = button_enter;
	} else current_button = button_down;
}
