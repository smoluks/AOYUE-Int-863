#include "stm32f1xx.h"
#include "gpio.h"
#include "systick.h"
#include "pt.h"

static PT_THREAD(processBeeperInternal(struct pt *pt));

static struct pt beeper_pt;
static bool makeBeep = false;

#define BEEP_ON() GPIOA->BSRR = BSRR_RESET(BIT15)
#define BEEP_OFF() GPIOA->BSRR = BSRR_SET(BIT15)

void initBeeper() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->ODR |= 0x00008000;
    GPIOA->CRH = (GPIOA->CRH & 0x0FFFFFFF) | GPIO15_CRH(GENERAL_PUSH_PULL_FAST);

    PT_INIT(&beeper_pt);
}

void processBeeper() {
    processBeeperInternal(&beeper_pt);
}

void beep() {
    makeBeep = true;
}

static uint32_t timestamp;
static PT_THREAD(processBeeperInternal(struct pt *pt)) {
    PT_BEGIN(pt)
    ;

    PT_WAIT_UNTIL(pt, makeBeep);
    makeBeep = false;

    BEEP_ON();

    timestamp = getSystime();
    PT_WAIT_UNTIL(pt, isTimeout(timestamp, 500));

    BEEP_OFF();

    timestamp = getSystime();
    PT_WAIT_UNTIL(pt, isTimeout(timestamp, 500));

    BEEP_ON();

    timestamp = getSystime();
    PT_WAIT_UNTIL(pt, isTimeout(timestamp, 500));

    BEEP_OFF();

    timestamp = getSystime();
    PT_WAIT_UNTIL(pt, isTimeout(timestamp, 500));

    BEEP_ON();

    timestamp = getSystime();
    PT_WAIT_UNTIL(pt, isTimeout(timestamp, 500));

    BEEP_OFF();

    timestamp = getSystime();
    PT_WAIT_UNTIL(pt, isTimeout(timestamp, 500));

PT_END(pt);
}
