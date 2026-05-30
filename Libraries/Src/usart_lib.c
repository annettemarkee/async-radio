#include <stdint.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include "usart_lib.h"
#include "gpio_lib.h"

int input_is_command(char *input, uint16_t *key_count, char new_char);
int input_is_valid(uint16_t *key_count, char *input, char new_char);

static const USART_CMD *cmd_list = NULL;
static uint16_t cmd_count = 0;

/*
 * Configures and enables USART peripheral on the specified pins.
 * Currently only supports USART3 pins: PC4/5, PB10/11, PD8/9, PC10/11.
 * 
 * Parameters:
 * - pair     : GPIO port, pin number, and alternate function for TX + RX.
 * - baud_rate: Desired baud rate. This should generally be 115200.
 */
void USART_Init(USART_PAIR pair, uint32_t baud_rate)
{
    __HAL_RCC_USART3_CLK_ENABLE();
    
    GPIO_InitTypeDef initTX = {pair.tx,
        GPIO_MODE_AF_PP,
        GPIO_PULLUP,
        GPIO_SPEED_FREQ_LOW,
        pair.alt_fn
    };
    
    GPIO_InitTypeDef initRX = {pair.rx,
        GPIO_MODE_AF_PP,
        GPIO_PULLUP,
        GPIO_SPEED_FREQ_LOW,
        pair.alt_fn
    };
    
    InitGPIO(pair.port, &initTX);
    InitGPIO(pair.port, &initRX);

    // BRR = f_clk / baud rate
    USART3->BRR = HAL_RCC_GetHCLKFreq() / baud_rate;

    USART3->CR1 |= (0b1 << 3); // Transmitter enable
    USART3->CR1 |= (0b1 << 2); // Receiver enable
    USART3->CR1 |= (0b1 << 5); // RX interrupt enable (USART)
    USART3->CR1 |=  0b1;       // USART enable, p747

    NVIC_EnableIRQ(USART3_4_IRQn);
    NVIC_SetPriority(USART3_4_IRQn, 1); // RX interrupt enable (NVIC)

    USART_Print("USART connection established.");
    USART_Prompt();
}

/*
 * Prints the provided character to terminal.
 *
 * Parameters:
 * - input   : The character to be printed.
 */
void USART_printc(char input)
{
    // Check USART flag indicating transmit register is empty
    while ((USART3->ISR & USART_ISR_TXE) == 0) { }

    // Write character into transmit data register
    USART3->TDR = input;
}

/*
 * Prints the provided string to terminal.
 *
 * Parameters:
 * - input   : The string to be printed.
 */
void USART_printstr(const char *input)
{
    while (*input) {
        USART_printc(*input++);
    }
}

/*
 * Prints the provided string to terminal on a new line.
 *
 * Parameters:
 * - input   : The string to be printed.
 */
void USART_println(const char *input)
{
    USART_printstr("\r\n");
    USART_printstr(input);
}

/*
 * Prints formatted string to terminal. 

 * Parameters:
 * - format  : Literal text and format specifiers.
 * - ...     : Data to fill format specifiers.
 * 
 * Example: USART_Print("String %d: %s", 1, "str") prints "String 1: str".
 */
void USART_Print(const char *format, ...)
{
    va_list args;
    char output[256]; // Note: May pose an issue when printing long strings.

    va_start(args, format);
    vsnprintf(output, sizeof(output), format, args);
    va_end(args);

    USART_println(output);
}

/*
 * Prompts the user for input with a terminal label.
 */
__attribute__((weak)) void USART_Prompt()
{
    USART_Print(">| ");
}

/*
 * Clears terminal display via escape sequence.
 */
void USART_Clear()
{
    USART_printstr("\033"); // ASCII ESC
    USART_printstr("\[2J"); // Clears visible screen
    USART_printstr("\033"); // ASCII ESC
    USART_printstr("\[H");  // Moves cursor to top left
}

/*
 * On terminal command entry, reads newest character from RDR register
 * and proceeds to input validation. This function should be called in
 * the relevant USART IRQHandler.
 * 
 * Parameters:
 * - key_count: Total length of current input string.
 * - input    : Current input string.
 */
void USART_Tick(uint16_t *key_count, char *input)
{
    // If new character is received
    if ((USART3->ISR & USART_ISR_RXNE) != 0) {

        // Read character from register and store it
        char new_char = USART3 -> RDR;

        if (input_is_valid(key_count, input, new_char) == 0) {
            if (input_is_command(input, key_count, new_char) == 0) {
                USART_Execute(input);
            }
        }
    }
}

//---------------------------------------------------------------------------------
// USART_Tick Helpers

/*
 * Processes special terminal input characters and reports if input
 * is executable.
 * 
 * Parameters:
 * - key_count: Total length of current input string.
 * - input    : Current input string.
 * - new_char : Character to be validated.
 * 
 * Outputs:
 * - 0: Character is executable.
 * - 1: Character is not executable.
 */
int input_is_valid(uint16_t *key_count, char *input, char new_char)
{
    if ((*key_count) == 0) { // First character of string

        // Initial backspaces: Ignored
        if (new_char == 0x7f) return 1;

        else return 0;
    }
    else {
        if (new_char == 0x7f) { // Backspace
            input[*key_count] = '\0'; // Replace final char with terminator
            (*key_count)--;
            USART_printstr("\b \b"); // Erase character from terminal
            return 1;
        }
        else return 0;
    }
}

/*
 * Checks if terminal input string is a command, and cleans leading and
 * trailing spaces of commands.
 * 
 * Parameters:
 * - key_count: Total length of current input string.
 * - input    : Current input string.
 * - new_char : Character to be validated.
 * 
 * Outputs:
 * - 0: String is executable.
 * - 1: String is not executable.
 */
int input_is_command(char *input, uint16_t *key_count, char new_char)
{
    input[*key_count] = new_char;
    USART_printc(new_char);

    if (new_char == '\r') { // End of command
        // Eliminate leading spaces
        if(input[0] == 0x20) {
            // Find the first non-space character
            char *first_non_space = input; // Start at input start
            uint16_t input_end = *key_count;
            
            for (int i = 0; i < input_end; i++) {
                if (input[i] == 0x20) {
                    first_non_space++; // Every time it's a space, move the pointer
                    (*key_count)--;
                }
                else break; // Non-space char found, keep pointer set where it is
            }

            // Note: All-space commands result in an empty string. This is probably fine, but if freak stuff happens later, here's why!

            memmove(input, first_non_space, *key_count);
        }

        // Eliminate trailing spaces
        while (input[*key_count - 1] == 0x20) {
            input[*key_count] = '\0';
            (*key_count)--;
        }

        input[*key_count] = '\0'; // Add terminator
        *key_count = 0;
        USART_Print(input);
        return 0;
    }
    else {
        (*key_count)++;
        return 1;
    }
}

// End of USART_Tick Helpers
//---------------------------------------------------------------------------------

/*
 * Processes and attempts to execute terminal commands.
 * 
 * Parameters:
 * - input   : String potentially containing a command.
 */
__attribute__((weak)) void USART_Execute(char *input)
{
    for (uint16_t i = 0; i < cmd_count; i++) {
        if (strcmp(input, cmd_list[i].name) == 0) { // Valid command
            cmd_list[i].function();

            memset(input, 0, sizeof(input));
            USART_Prompt();
            return;
        }
    }

    USART_Print("Invalid command: %s", input);
    memset(input, 0, sizeof(input));
    USART_Prompt();
}

/*
 * Binds terminal command strings to functions at compile time.
 * 
 * Parameters:
 * - cmds    : List of commands, the functions they execute, and a brief description.
 * - count   : Number of commands in cmds.
 */
void USART_CMD_REG(const USART_CMD *cmds, uint16_t count)
{
    cmd_list  = cmds;
    cmd_count = count;
}

/*
 * Prints all valid terminal commands to terminal.
 */
void USART_CMD_HELP()
{
    USART_Print("Command - Description");
    USART_Print("=====================");

    for (uint16_t i = 0; i < cmd_count; i++) {
        USART_Print("%-7s - %s", cmd_list[i].name, cmd_list[i].description);
    }
}