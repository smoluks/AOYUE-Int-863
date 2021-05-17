#pragma once

enum button_e {
    button_down,
	button_enter,
	button_up,
	button_none,
};

void adcInit();
void ADC1_2_IRQHandler();

