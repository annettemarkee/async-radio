#include "stdint.h"
#include "usart_lib.h"   // <-- Must be before any use of USART_CMD

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

// Helper Functions
void     SystemClock_Config(void);

// USART Command Function Declarations
void     cmd_help(void);

#endif /* __MAIN_H */