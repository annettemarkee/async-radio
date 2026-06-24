#include <stdint.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>

#ifndef ADC_LIB_H
#define ADC_LIB_H

#include "stm32f0xx_hal.h"

#define ADC_TRIGGER_OFF     (0b00)
#define ADC_TRIGGER_RISING  (0b01)
#define ADC_TRIGGER_FALLING (0b10)
#define ADC_TRIGGER_BOTH    (0b11)

typedef struct {
    uint16_t        pin_number;
    GPIO_TypeDef    *gpio_type;
    uint16_t        channel;
} ADC_Input_Pin;

static const ADC_Input_Pin ADC_In14 = {GPIO_PIN_4, GPIOC, 14}; // PC4

typedef struct {
    uint8_t  bits;
    uint8_t  code;
    uint16_t mask;
} ADC_Resolution;

static const ADC_Resolution ADC_Res_6  = {6,  0b11, 0b111111};
static const ADC_Resolution ADC_Res_8  = {8,  0b10, 0b11111111};
static const ADC_Resolution ADC_Res_10 = {10, 0b01, 0b1111111111};
static const ADC_Resolution ADC_Res_12 = {12, 0b00, 0b111111111111};

void ADC_Init(ADC_Input_Pin pin, ADC_Resolution res);

#endif