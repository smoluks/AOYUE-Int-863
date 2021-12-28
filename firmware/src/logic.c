#include "stm32f1xx.h"
#include "logic.h"
#include "hardwareConfig.h"
#include "config.h"
#include "string.h"
#include "gpio.h"
#include "beeper.h"
#include "output.h"
#include "systick.h"
#include "display.h"

static int32_t getPidValue(uint8_t channel);
static uint16_t normalize(int32_t rawPIDValue);
static void resetPIDState(uint8_t channel);

#define LED_SETRED() GPIOC->BSRR = BSRR_SET(BIT14) | BSRR_RESET(BIT13);
#define LED_SETGREEN() GPIOC->BSRR = BSRR_SET(BIT13) | BSRR_RESET(BIT14)
#define LED_SETORANGE() GPIOC->BSRR = BSRR_SET(BIT14) | BSRR_SET(BIT13)
#define LED_SETOFF() GPIOC->BSRR = BSRR_RESET(BIT14) | BSRR_RESET(BIT13)

#define CH1_FLAG 0x4000
#define CH2_FLAG 0x8000
#define CH_ANY_FLAG (CH1_FLAG | CH2_FLAG)

extern config_s config;

sensor_s sensors[SENSOR_COUNT];
work_mode_e work_mode;
static sensor_s sensors_old[SENSOR_COUNT];
static bool beep_enable = false;
static uint32_t pid_timestamp;
static bool timestamp_inited = false;

void initLogic() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    GPIOC->CRH = (GPIOC->CRH & 0xF00FFFFF)
            | GPIO14_CRH(GENERAL_PUSH_PULL_FAST)  //green
            | GPIO13_CRH(GENERAL_PUSH_PULL_FAST); //red
    LED_SETOFF();

    setMode(0);

    for (int i = 0; i < SENSOR_COUNT; i++) {
       resetPIDState(i);
    }
}

void processLogic() {
    //update display flag
    if (memcmp((uint8_t*) &sensors_old, (uint8_t*) &sensors, sizeof(sensors))) {
        memcpy((uint8_t*) &sensors_old, (uint8_t*) &sensors, sizeof(sensors));
        updateDisplay();
    }

    //beep
    for (int i = 0; i < SENSOR_COUNT; i++) {
        if (beep_enable && sensors[i].isPresent && (config.targets_temperature[i] & CH_ANY_FLAG) && (sensors[i].value & 0x3FF0) == (config.targets_temperature[i] & 0x3FF0)) {
            beep();
            beep_enable = false;
        }
    }

    //cross-zero timeout check
    if (timestamp_inited && isTimeout(pid_timestamp, 100)) {
        emergencyDisableAll();
        setError(ERR_NO_POWER_FREQ);
    }

    //max temp check
    for (int i = 0; i < SENSOR_COUNT; i++) {
        if(sensors[i].value >= MAXTEMP << 4)
        {
            emergencyDisableAll();
            setError(ERR_MAXTEMP);
        }
    }
}

void setMode(work_mode_e mode) {
    work_mode = mode;

    switch (mode) {
        case MODE_OFF:
            LED_SETOFF();
            break;
        case MODE_HEAT:
            LED_SETRED();
            break;
        case MODE_COLD:
            LED_SETORANGE();
            break;
    }

    beep_enable = true;

    updateDisplay();
}

void setTargetTemperature(uint8_t channel, int16_t value) {
    config.targets_temperature[channel] = value;
    resetPIDState(channel);
    beep_enable = true;
    updateDisplay();
    updateConfig();
}

output_s calculateOutput() {
    pid_timestamp = getSystime();
    timestamp_inited = true;
    output_s result;

    //temperature targets
    if (work_mode == MODE_OFF) {
        result.channel1 = result.channel2 = 0;
    } else {
        int32_t channel1_value = 0x7FFFFFFF; //max
        int32_t channel2_value = 0x7FFFFFFF;

        for (int i = 0; i < SENSOR_COUNT; i++) {
            if (!sensors[i].isPresent || !(config.targets_temperature[i] & 0xC000))
                continue;

            //first
            if(config.targets_temperature[i] & 0x4000)
            {
                int32_t value = getPidValue(i, 0);
                if(value < channel1_value)
                    channel1_value = value;
            }

            //second
            if(config.targets_temperature[i] & 0x8000)
            {
                int32_t value = getPidValue(i, 1);
                if(value < channel2_value)
                    channel2_value = value;
            }
        }

        result.channel1 = normalize(channel1_value);
        result.channel2 = normalize(channel2_value);
    }

    return result;
}

#define P 100
#define P_HEATDIFF 2
#define P_COLDDIFF 2

#define I 0
#define IMAX 250 * 256
#define IMIN -250 * 256

#define D 10

int32_t iCache[SENSOR_COUNT];
int32_t oldValue[SENSOR_COUNT];
static int32_t getPidValue(uint8_t channel)
{
    int32_t result = 0;

    //P
    int32_t targetDiff = (config.targets_temperature[channel] & 0x3FFF) - sensors[channel].value;
    result = targetDiff * P;

    //I
    iCache[channel] += targetDiff;

    if(iCache[channel] > IMAX)
        iCache[channel] = IMAX;
    if(iCache[channel] < IMIN)
        iCache[channel] = IMIN;

    result += iCache[channel] * I;

    //D
    int32_t historyDiff = sensors[channel].value - oldValue[channel];
    oldValue[channel] = sensors[channel].value;

    result -= historyDiff * D;

    //heat throttling
    if(work_mode == MODE_HEAT && config.speedLimits[channel].heat_speed)
    {
        if(historyDiff > config.speedLimits[channel].heat_speed)
        {
            result -= historyDiff * P_HEATDIFF;
        }
    }

    //cold throttling
    if(work_mode == MODE_COLD && config.speedLimits[channel].cool_speed)
    {
        if(historyDiff < -config.speedLimits[channel].cool_speed)
        {
            result -= historyDiff * P_COLDDIFF;
        }
    }

    return result;
}

//int32_t -> 0..1024
static uint16_t normalize(int32_t rawPIDValue)
{
    if(rawPIDValue == 0x7FFFFFFF)
        return 0;

    if(rawPIDValue <= 0)
        return 0;

    if(rawPIDValue >= (1024 * 256))
        return 1024;

    return rawPIDValue / 256;
}


static void resetPIDState(uint8_t channel)
{
    iCache[channel] = 0;
    oldValue[channel] = sensors[channel].value;
}
