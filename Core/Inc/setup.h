#ifndef __SETUP_H
#define __SETUP_H

#include "stm32f767xx.h"
#include "stm32f7xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Enables all required clock sources.
void Setup_RCC (void);

/// Configures all the GPIO's.
void Setup_GPIO (void);

/// Configures all the UART's.
void Setup_UART (void);

/// Configures the steppers.
void Setup_Steppers (void);

/// Configures the TIMs.
void Setup_TIMs (void);

#ifdef __cplusplus
}
#endif

#endif
