#include "config.h"
#include "regul.h"
#include "stm32f1xx.h"

extern struct ramconfig_s ramconfig;
extern struct romconfig_s romconfig;

void adc_start()
{
	ADC1->CR2 |= ADC_CR2_JSWSTART; //Запуск преобразований
	ADC2->CR2 |= ADC_CR2_JSWSTART;
}


void ADC1_2_IRQHandler()
{
	//подождем второй АЦП
	while (!(ADC2->SR & ADC_SR_JEOC)){}

	ADC1->SR &= ~ADC_SR_JEOC;
	ADC2->SR &= ~ADC_SR_JEOC;

	ramconfig.temperatures[1] = (((ADC2->JDR2 << 1) * romconfig.sensor_corrections[1].mul_corr) >> 8) + romconfig.sensor_corrections[1].add_corr + ramconfig.temperatures[0];
	ramconfig.temperatures[2] = (((ADC1->JDR2 << 1) * romconfig.sensor_corrections[2].mul_corr) >> 8) + romconfig.sensor_corrections[2].add_corr + ramconfig.temperatures[0];
	ramconfig.temperatures[3] = (((ADC1->JDR1 << 1) * romconfig.sensor_corrections[3].mul_corr) >> 8) + romconfig.sensor_corrections[3].add_corr + ramconfig.temperatures[0];
	ramconfig.temperatures[4] = (((ADC2->JDR1 << 1) * romconfig.sensor_corrections[4].mul_corr) >> 8) + romconfig.sensor_corrections[4].add_corr + ramconfig.temperatures[0];

	ADC_Complete();
}
