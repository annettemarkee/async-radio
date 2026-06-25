#include <stdint.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>
#include "adc_lib.h"
#include "gpio_lib.h"
#include "usart_lib.h"

static uint16_t calibration_factor;
static uint16_t conversion_data;
static ADC_Resolution chosen_resolution;

/*
 * Initializes the ADC and its input pin.
 * 
 * Parameters:
 * - pin     : ADC input pin to be initialized.
 * - res     : Data resolution struct, ADC_Res_N.
 */
void ADC_Init(ADC_Input_Pin pin, ADC_Resolution res) {

    chosen_resolution = res;

    // Initialize ADC input pin
    {
        GPIO_InitTypeDef adcInput = {pin.pin_number,
            GPIO_MODE_ANALOG,
            GPIO_NOPULL,
            GPIO_SPEED_FREQ_LOW
        };
    
        InitGPIO(pin.gpio_type, &pin.pin_number);
    }

    __HAL_RCC_ADC1_CLK_ENABLE();

    // Configure ADC
    {
        ADC1->CFGR1 |=  (res.code << 3);  // Data resolution
        ADC1->CFGR1 &= ~(0b1      << 5);  // Data is right-aligned
        ADC1->CFGR1 |=  (0b00     << 10); // External trigger disable
        ADC1->CFGR1 &= ~(0b1      << 16); // Continuous conversion mode

        ADC1->CHSELR |= (0b1 << pin.channel); // Select channel of pin
    }

    // Calibrate ADC
    ADC1->CR |= (0b1 << 31); // Start calibration
    while ((ADC1->CR & ADC_CR_ADCAL) != 0) { } // Wait for calibration
    calibration_factor = ((ADC1->DR) & res.mask);

    // Enable interrupts
    ADC1->IER |= (0b1 << 2); // Enable EOC interrupts
    NVIC_EnableIRQ(ADC1_COMP_IRQn);
    NVIC_SetPriority(ADC1_COMP_IRQn, 2);

    // Start ADC
    ADC1->CR |= (0b1 << 0); // ADC enable
    ADC1->CR |= (0b1 << 2); // ADC start
}

void ADC_Save_Data() {
    conversion_data = ((ADC1->DR) & chosen_resolution.mask);
}

uint16_t ADC_Get_Data() {
    return conversion_data;
}