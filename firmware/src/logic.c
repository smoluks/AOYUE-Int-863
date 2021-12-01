#include "stm32f1xx.h"
#include "logic.h"
#include "hardwareConfig.h"
#include "config.h"
#include "string.h"
#include "gpio.h"
#include "beeper.h"

static bool isTemperatureLow(uint8_t channel);
static bool isHeatThrotting(uint8_t channel);
static bool isColdThrottling(uint8_t channel);

#define LED_SETRED() GPIOC->BSRR = BSRR_SET(BIT14) | BSRR_RESET(BIT13);
#define LED_SETGREEN() GPIOC->BSRR = BSRR_SET(BIT13) | BSRR_RESET(BIT14)
#define LED_SETORANGE() GPIOC->BSRR = BSRR_SET(BIT14) | BSRR_SET(BIT13)
#define LED_SETOFF() GPIOC->BSRR = BSRR_RESET(BIT14) | BSRR_RESET(BIT13)

#define OUT1_ON() GPIOB->BSRR = BSRR_SET(BIT5)
#define OUT1_OFF() GPIOB->BSRR = BSRR_RESET(BIT5)

#define OUT2_ON() GPIOB->BSRR = BSRR_RESET(BIT4)
#define OUT2_OFF() GPIOB->BSRR = BSRR_SET(BIT4)

#define BOTH_OFF() GPIOB->BSRR = BSRR_SET(BIT4) | BSRR_RESET(BIT5)

extern bool need_update_display;
extern config_s config;

sensor_s sensors[SENSOR_COUNT];
work_mode_e work_mode;
static sensor_s sensors_old[SENSOR_COUNT];
static bool makeBeep[SENSOR_COUNT];
static uint32_t timestamp;
static bool timestamp_inited = false;

void initLogic() {
    OUT1_OFF();
    OUT2_OFF();
    LED_SETOFF();

    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;

    GPIOB->CRL = (GPIOB->CRL & 0xFF00FFFF) | GPIO5_CRL(GENERAL_PUSH_PULL_FAST)  //ch1
            | GPIO4_CRL(GENERAL_PUSH_PULL_FAST); //ch2

    GPIOC->CRH = (GPIOC->CRH & 0xF00FFFFF) | GPIO14_CRH(GENERAL_PUSH_PULL_FAST)  //green
            | GPIO13_CRH(GENERAL_PUSH_PULL_FAST); //red

    setMode(0);
}

void processLogic() {
    //update display flag
    if (memcmp((uint8_t*) &sensors_old, (uint8_t*) &sensors, sizeof(sensors))) {
        memcpy((uint8_t*) &sensors_old, (uint8_t*) &sensors, sizeof(sensors));
        need_update_display = true;
    }

    //beep
    for (int i = 0; i < SENSOR_COUNT; i++) {
        if (makeBeep[i] && (sensors[i].value & 0x3FF0) == (config.targets_temperature[i] & 0x3FF0)) {
            beep();
            makeBeep[i] = false;
        }
    }

    //cross-zero check
    if (timestamp_inited && isTimeout(timestamp, 100)) {
        BOTH_OFF();
        setError(ERR_NO_POWER_FREQ);
    }
}

void setMode(work_mode_e mode) {
    work_mode = mode;
    switch (mode) {
        case MODE_OFF:
            LED_SETOFF();
            break;
        case MODE_HEAT:
            LED_SETRED()
            ;
            break;
        case MODE_COLD:
            LED_SETORANGE();
            break;
    }

    need_update_display = true;
}

void setTargetTemperature(uint8_t channel, int16_t value) {
    config.targets_temperature[channel] = value;
    makeBeep[channel] = true;
    need_update_display = true;
    updateConfig();
}

void MakeOutput() {
    timestamp = getSystime();
    timestamp_inited = true;

    uint8_t first_enables = 0;
    uint8_t first_disables = 0;

    uint8_t second_enables = 0;
    uint8_t second_disables = 0;

    //temperature targets
    if (work_mode == MODE_OFF) {
        BOTH_OFF();
    } else {
        for (int i = 0; i < SENSOR_COUNT; i++) {
            if (!sensors[i].isPresent)
                continue;

            if(sensors[i].value >= MAXTEMP << 4)
            {
                BOTH_OFF();
                beep();
            }

            if (config.targets_temperature[i] & 0xC000) {
                if (isColdThrottling(i) || (isTemperatureLow(i) && !isHeatThrotting(i))) {
                    //enable
                    if (config.targets_temperature[i] & 0x4000) //ch1
                        first_enables++;
                    if (config.targets_temperature[i] & 0x8000) //ch2
                        second_enables++;
                } else {
                    //disable
                    if (config.targets_temperature[i] & 0x4000) //ch1
                        first_disables++;
                    if (config.targets_temperature[i] & 0x8000) //ch2
                        second_disables++;
                }
            }
        }

        if (first_enables)
            OUT1_ON();
        else
            OUT1_OFF();

        if (second_enables)
            OUT2_ON();
        else
            OUT2_OFF();
    }
}

bool isTemperatureLow(uint8_t channel) {
    return sensors[channel].value < (config.targets_temperature[channel] & 0x3FFF);
}

bool isHeatThrotting(uint8_t channel) {
    return work_mode == MODE_HEAT && config.speedLimits[channel].heat && sensors[channel].isPresent && sensors_old[channel].isPresent && (sensors[channel].value > (sensors_old[channel].value + config.speedLimits[channel].heat));
}

bool isColdThrottling(uint8_t channel) {
    return work_mode == MODE_COLD && config.speedLimits[channel].cold && sensors[channel].isPresent && sensors_old[channel].isPresent && (sensors[channel].value < (sensors_old[channel].value - config.speedLimits[channel].cold));
}
