#include <stdint.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>

#ifndef GPIO_LIB_H
#define GPIO_LIB_H

#include "stm32f0xx_hal.h"

#define PIN_RED     GPIO_PIN_6
#define PIN_BLUE    GPIO_PIN_7
#define PIN_ORANGE  GPIO_PIN_8
#define PIN_GREEN   GPIO_PIN_9

uint16_t    GetPin(uint16_t pin_bits);
void        InitGPIO(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init);
void        InitLEDs(void);

#endif