#pragma once

#define MAX31856_COUNT 3

void max31856Init();
void max31856Read();

//Configuration 0 Register
#define MAX31856_CR0 0x00
#define MAX31856_CR0_CMODE 0x80 //Conversion Mode
#define MAX31856_CR0_1SHOT 0x40 //One-Shot Mode
#define MAX31856_CR0_OCFAULT1 0x20 //Open-Circuit Fault Detection mode
#define MAX31856_CR0_OCFAULT0 0x10
#define MAX31856_CR0_CJ 0x08 //Cold-Junction Sensor Disable
#define MAX31856_CR0_FAULT 0x04 //Fault Mode
#define MAX31856_CR0_FAULTCLR 0x02 //Fault Status Clear
#define MAX31856_CR0_5060Hz 0x01 //50Hz/60Hz Noise Rejection Filter Selection

// Fault Mask Register
#define MAX31856_CR1 0x01
#define MAX31856_CR1_1SAMPLE 0x00
#define MAX31856_CR1_2SAMPLES 0x10
#define MAX31856_CR1_4SAMPLES 0x20
#define MAX31856_CR1_8SAMPLES 0x30
#define MAX31856_CR1_16SAMPLES 0x40
#define MAX31856_CR1_TYPE_B 0x00
#define MAX31856_CR1_TYPE_E 0x01
#define MAX31856_CR1_TYPE_J 0x02
#define MAX31856_CR1_TYPE_K 0x03
#define MAX31856_CR1_TYPE_N 0x04
#define MAX31856_CR1_TYPE_R 0x05
#define MAX31856_CR1_TYPE_S 0x06
#define MAX31856_CR1_TYPE_T 0x07
#define MAX31856_CR1_TYPE_GAIN8 0x08
#define MAX31856_CR1_TYPE_GAIN32 0x0C

// Fault Mask Register
#define MAX31856_MASK_REG 0x02
#define MAX31856_MASK_CJ_HIGH 0x20
#define MAX31856_MASK_CJ_LOW 0x10
#define MAX31856_MASK_TC_HIGH 0x08
#define MAX31856_MASK_TC_LOW 0x04
#define MAX31856_MASK_OV_UV 0x02
#define MAX31856_MASK_OPEN 0x01

//Cold-Junction High Fault Threshold Register
#define MAX31856_CJHF_REG 0x03

//Cold-Junction Low Fault Threshold Register
#define MAX31856_CJLF_REG 0x04

//Linearized Temperature High Fault Threshold Register
#define MAX31856_LTHFTH_REG 0x05
#define MAX31856_LTHFTL_REG 0x06

// Linearized Temperature Low Fault Threshold Register
#define MAX31856_LTLFTH_REG 0x07
#define MAX31856_LTLFTL_REG 0x08

//Cold-Junction Temperature Offset Register
#define MAX31856_CJTO_REG 0x09

//Cold-Junction Temperature Register
#define MAX31856_CJTH_REG 0x0A
#define MAX31856_CJTL_REG 0x0B

//Linearized TC Temperature
#define MAX31856_LTCBH_REG 0x0C
#define MAX31856_LTCBM_REG 0x0D
#define MAX31856_LTCBL_REG 0x0E

//Fault Status Register
#define MAX31856_SR_REG 0x0F
#define MAX31856_SR_CJ_RANGE 0x80
#define MAX31856_SR_TC_RANGE 0x40
#define MAX31856_SR_CJ_HIGH 0x20
#define MAX31856_SR_CJ_LOW 0x10
#define MAX31856_SR_TC_HIGH 0x08
#define MAX31856_SR_TC_LOW 0x04
#define MAX31856_SR_OVUV 0x02
#define MAX31856_SR_OPEN 0x01
