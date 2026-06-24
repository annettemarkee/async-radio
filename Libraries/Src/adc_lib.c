#include <stdint.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>
#include "adc_lib.h"
#include "gpio_lib.h"
#include "usart_lib.h"

/*
 * Initializes the ADC and its input pin.
 * 
 * Parameters:
 * - pin       : ADC input pin to be initialized.
 * - resolution: Data resolution of conversion (6, 8, 10, or 12 bits).
 */
void ADC_Init(ADC_Input_Pin pin, uint8_t resolution) {
    GPIO_InitTypeDef adcInput = {pin.pin_number,
        GPIO_MODE_ANALOG,
        GPIO_NOPULL,
        GPIO_SPEED_FREQ_LOW
    };

    InitGPIO(pin.gpio_type, &pin.pin_number);

    __HAL_RCC_ADC1_CLK_ENABLE();

    uint8_t res;
    switch (resolution) {
        case 6:
            res = 0b11;
            break;
        case 8:
            res = 0b10;
            break;
        case 10:
            res = 0b01;
            break;
        case 12:
            res = 0b00;
            break;
        default:
            USART_Print("Invalid ADC resolution: %d. Defaulting to 6-bit.", resolution);
            res = 0b11;   
    }

    ADC1->CFGR1 |=  (res  << 3);  // Data resolution
    ADC1->CFGR1 |=  (0b00 << 10); // External trigger disable
    ADC1->CFGR1 &= ~(0b1  << 16); // Continuous conversion mode

    ADC1->CHSELR |= (0b1 << pin.channel); // Select channel of pin

    // Calibration
    ADC1->CR |= (0b1 << 31); // Start calibration
    while ((ADC1->CR & ADC_CR_ADCAL) != 0) { } // Wait for calibration

    // Peripheral enable
    ADC1->CR |= (0b1 << 0); // ADC enable
    ADC1->CR |= (0b1 << 2); // ADC start

    // ADC Conversion
    // After conversion, transfer data from ADC_DR to other location
    // EOC indicates end of conversion
        // EOCIE causes interrupt
    // EOS indicates end of sequence
        // EOSIE causes interrupt
}
