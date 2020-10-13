#include "stm32f1xx.h"
#include "logic.h"
#include "config.h"

extern config_s config;

static uint32_t incremental[3];
static uint16_t incrementalCountAdc1;
static uint16_t incrementalCountAdc2;

static uint16_t zeroValue = 0;

void ADC1_2_IRQHandler() {
	if (ADC1->SR & ADC_SR_JEOC) {
		incremental[1] += ADC1->JDR2 - zeroValue;
		incremental[2] += ADC1->JDR1 - zeroValue;

		incrementalCountAdc1++;

		if (incrementalCountAdc1 == 0x800) {
			process_sensor(1, ((incremental[1] >> 7) + config.sensorCorrections[1].additive) * config.sensorCorrections[1].multiplicative);

			process_sensor(2, ((incremental[2] >> 7) + config.sensorCorrections[2].additive) * config.sensorCorrections[2].multiplicative);

			incremental[1] = 0;
			incremental[2] = 0;
			incrementalCountAdc1 = 0;
		}

		ADC1->SR &= ~ADC_SR_JEOC;
		ADC1->CR2 |= ADC_CR2_JSWSTART;
	}

	if (ADC2->SR & ADC_SR_JEOC) {
		incremental[0] += ADC2->JDR2 - zeroValue;

		incrementalCountAdc2++;

		if (incrementalCountAdc2 == 0x800) {
			process_sensor(0, ((incremental[0] >> 7) + config.sensorCorrections[0].additive) * config.sensorCorrections[0].multiplicative);

			incremental[0] = 0;
			incrementalCountAdc2 = 0;
		}

		zeroValue = ADC2->JDR1;

		ADC2->SR &= ~ADC_SR_JEOC;
		ADC2->CR2 |= ADC_CR2_JSWSTART;
	}
}
