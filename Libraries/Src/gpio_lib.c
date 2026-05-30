#include <stdint.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>
#include "gpio_lib.h"

uint16_t GetPin(uint16_t pin_bits) {
    uint16_t pin;
    if (pin_bits != GPIO_PIN_All) {
        switch (pin_bits) {// 32bit range:
            case (GPIO_PIN_0):
                pin = 0;    // 0-1
                break;
            case (GPIO_PIN_1):
                pin = 1;    // 2-3
                break;
            case (GPIO_PIN_2):
                pin = 2;    // 4-5
                break;
            case (GPIO_PIN_3):
                pin = 3;    // 6-7
                break;
            case (GPIO_PIN_4):
                pin = 4;    // 8-9
                break;
            case (GPIO_PIN_5):
                pin = 5;    // 10-11
                break;
            case (GPIO_PIN_6):
                pin = 6;    // 12-13
                break;
            case (GPIO_PIN_7):
                pin = 7;    // 14-15
                break;
            case (GPIO_PIN_8):
                pin = 8;    // 16-17
                break;
            case (GPIO_PIN_9):
                pin = 9;    // 18-19
                break;
            case (GPIO_PIN_10):
                pin = 10;   // 20-21
                break;
            case (GPIO_PIN_11):
                pin = 11;   // etc
                break;
            case (GPIO_PIN_12):
                pin = 12;
                break;
            case (GPIO_PIN_13):
                pin = 13;
                break;
            case (GPIO_PIN_14):
                pin = 14;
                break;
            case (GPIO_PIN_15):
                pin = 15;   // 30-31
                break;
        }
    }
    else pin = 16;

    return pin;
}

void InitGPIO(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
    uint16_t pin = GetPin(GPIO_Init->Pin);
    uint16_t pin_offset = pin * 2;

    // Alternate Functions
    if ((GPIO_Init->Mode == GPIO_MODE_AF_PP) || (GPIO_Init->Mode == GPIO_MODE_AF_OD)) {
        // Set mode to AF
        GPIOx->MODER &= ~(0b11 << pin_offset);
        GPIOx->MODER |=  (0b10 << pin_offset);
    
        // Set AFR
        uint32_t afr_index = (pin < 8) ? 0 : 1;
        uint32_t afr_shift = (pin % 8) * 4;
    
        GPIOx->AFR[afr_index] &= ~(0b1111 << afr_shift);
        GPIOx->AFR[afr_index] |=  (GPIO_Init->Alternate << afr_shift);
    }
    
    // I/O Mode
    switch (GPIO_Init->Mode) {
        // 00 Input
        case (GPIO_MODE_INPUT):
            GPIOx->MODER &= ~(0b11 << pin_offset);
            break;
        // 01 General-purpose Output
        case (GPIO_MODE_OUTPUT_PP):
            GPIOx->MODER &= ~(0b11 << pin_offset);
            GPIOx->MODER |=  (0b01 << pin_offset);
            break;
        // 10 Alternate Function
        case (GPIO_MODE_AF_PP):
        case (GPIO_MODE_AF_OD):
            /* AF implemented above
            GPIOx->MODER &= ~(0b11 << pin_offset);
            GPIOx->MODER |=  (0b10 << pin_offset); */
            break;
        // 11 Analog
        case (GPIO_MODE_ANALOG):
            GPIOx->MODER |=  (0b11 << pin_offset);
            break;
    }

    // I/O Push-Pull Output Type
    if (GPIO_Init->Mode == GPIO_MODE_AF_OD) {
        GPIOx->OTYPER |=  (0b1 << pin);   // Open-Drain
    } else {
        GPIOx->OTYPER &= ~(0b1 << pin);   // Push-Pull
    }

    // I/O Pull-Up/Down
    switch (GPIO_Init->Pull) {
        // 00: No Pull-Up/Down
        case (GPIO_NOPULL):
            GPIOx->PUPDR &= ~(0b11 << pin_offset);
            break;
        // 01: Pull-Up
        case (GPIO_PULLUP):
            GPIOx->PUPDR &= ~(0b11 << pin_offset);
            GPIOx->PUPDR |=  (0b01 << pin_offset);
            break;
        // 10: Pull-Down
        case (GPIO_PULLDOWN):
            GPIOx->PUPDR &= ~(0b11 << pin_offset);
            GPIOx->PUPDR |=  (0b10 << pin_offset);
            break;
        // 11: Reserved
    }

    // I/O Output Speed
    switch (GPIO_Init->Speed) {
        // x0 Low speed
        case (GPIO_SPEED_LOW):
            GPIOx->OSPEEDR &= ~(0b11 << pin_offset);
            break;
        // 01 Medium speed
        case (GPIO_SPEED_MEDIUM):
            GPIOx->OSPEEDR &= ~(0b11 << pin_offset);
            GPIOx->OSPEEDR |=  (0b01 << pin_offset);
            break;
        // 11 High speed
        case (GPIO_SPEED_HIGH):
            GPIOx->OSPEEDR |= (0b11 << pin_offset);
            break;
    }
}

void InitLEDs() {
    GPIO_InitTypeDef initRed = {GPIO_PIN_6,
        GPIO_MODE_OUTPUT_PP,
        GPIO_NOPULL,
        GPIO_SPEED_FREQ_LOW
      };
      GPIO_InitTypeDef initBlue = {GPIO_PIN_7,
        GPIO_MODE_OUTPUT_PP,
        GPIO_NOPULL,
        GPIO_SPEED_FREQ_LOW
      };
      GPIO_InitTypeDef initOrange = {GPIO_PIN_8,
        GPIO_MODE_OUTPUT_PP,
        GPIO_NOPULL,
        GPIO_SPEED_FREQ_LOW
      };
      GPIO_InitTypeDef initGreen = {GPIO_PIN_9,
        GPIO_MODE_OUTPUT_PP,
        GPIO_NOPULL,
        GPIO_SPEED_FREQ_LOW
      };

    InitGPIO(GPIOC, &initRed);
    InitGPIO(GPIOC, &initBlue);
    InitGPIO(GPIOC, &initOrange);
    InitGPIO(GPIOC, &initGreen);
}