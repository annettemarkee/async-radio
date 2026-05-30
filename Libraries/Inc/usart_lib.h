#include <stdint.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>

#ifndef USART_LIB_H
#define USART_LIB_H

#include "stm32f0xx_hal.h"

// ------------------------------------------------------------------------------------
// USART pairs of TX & RX pins, along with their alternate functions.

typedef struct {
    GPIO_TypeDef    *port;
    uint16_t        tx;
    uint16_t        rx;
    uint8_t         alt_fn;
} USART_PAIR;

static const USART_PAIR PB10_PB11 = {GPIOB, GPIO_PIN_10, GPIO_PIN_11, GPIO_AF4_USART3};
static const USART_PAIR PC4_PC5   = {GPIOC, GPIO_PIN_4,  GPIO_PIN_5,  GPIO_AF1_USART3};
static const USART_PAIR PC10_PC11 = {GPIOC, GPIO_PIN_10, GPIO_PIN_11, GPIO_AF1_USART3};
static const USART_PAIR PD8_PD9   = {GPIOD, GPIO_PIN_8,  GPIO_PIN_9,  GPIO_AF0_USART3};

// ------------------------------------------------------------------------------------
// Bindings of terminal input strings and executable functions.

typedef void (*USART_CMD_FN)(void);

typedef struct {
    const char *name;
    USART_CMD_FN function;
    const char *description;
} USART_CMD;

void USART_CMD_REG(const USART_CMD *cmds, uint16_t count);

// ------------------------------------------------------------------------------------

void        USART_Init(USART_PAIR pair, uint32_t baud_rate);
void        USART_Tick(uint16_t *key_count, char *input);
void        USART_Execute(char *input) __attribute__((weak));
void        USART_Print(const char *format, ...);
void        USART_Prompt(void) __attribute__((weak));;
void        USART_Clear();
void        USART_CMD_HELP(void);

#endif