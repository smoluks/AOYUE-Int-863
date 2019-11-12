#pragma once

#define GPIOA_FANOFF   		1 << 15

#define GPIOB_1WIRE  		1 << 0
#define GPIOB_BTRESET 		1 << 1
#define GPIOB_OLED_RESET    1 << 6
#define GPIOB_OLED_DC       1 << 8
#define GPIOB_LED_RED       1 << 7
#define GPIOB_LED_GREEN     1 << 9
#define GPIOB_BTLED2 		1 << 11
#define GPIOB_BTMODE  		1 << 12
#define GPIOB_OUT1		  	1 << 13
#define GPIOB_OUT2  		1 << 14
#define GPIOB_USB_EN 		1 << 15

#define GPIOC_BUTTON1		1 << 15
#define GPIOC_BUTTON2		1 << 14
#define GPIOC_BUTTON3		1 << 13

#define BSRR_SET(i) i
#define BSRR_RESET(i) i<<16
