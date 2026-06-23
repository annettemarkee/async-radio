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

#define ADC_RES_6b  (0b11)
#define ADC_RES_8b  (0b10)
#define ADC_RES_10b (0b01)
#define ADC_RES_12b (0b00)

typedef struct {
    uint16_t        pin_number;
    GPIO_TypeDef    *gpio_type;
    uint16_t        channel;
} ADC_Input_Pin;

static const ADC_Input_Pin ADC_In14 = {GPIO_PIN_4, GPIOC, 14}; // PC4

void ADC_Init(ADC_Input_Pin pin, uint8_t resolution);

#endif