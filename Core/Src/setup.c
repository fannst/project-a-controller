#include "setup.h"
#include "pins.h"
#include "main.h"
#include "stepper.h"

//////////////////////////////////////////////////////////////////////////
// Static Variables
//////////////////////////////////////////////////////////////////////////

//// UARTs

UART_HandleTypeDef s_DebugUARTHandle = {
  .Instance = USART1,
  .Init.BaudRate = 460800,
  .Init.WordLength = UART_WORDLENGTH_8B,
  .Init.StopBits = UART_STOPBITS_1,
  .Init.Parity = UART_PARITY_NONE,
  .Init.HwFlowCtl = UART_HWCONTROL_NONE,
  .Init.Mode = UART_MODE_TX_RX
};

/// Steppers

Stepper_t g_Stepper0 = STEPPER_NEW (200, 4000, 2, 0);
Stepper_t g_Stepper1 = STEPPER_NEW (200, 8000, 2, 0);
Stepper_t g_Stepper2 = STEPPER_NEW (200, 8000, 2, 0);
Stepper_t g_Stepper3 = STEPPER_NEW (200, 8000, 2, 1);
Stepper_t g_Stepper4 = STEPPER_NEW (200, 8000, 2, 1);
Stepper_t g_Stepper5 = STEPPER_NEW (200, 8000, 2, 1);

uint32_t g_StepperCount = 6;
Stepper_t *g_Steppers[6] = {
  &g_Stepper0,
  &g_Stepper1,
  &g_Stepper2,
  &g_Stepper3,
  &g_Stepper4,
  &g_Stepper5
};

//////////////////////////////////////////////////////////////////////////
// Setup Methods
//////////////////////////////////////////////////////////////////////////

/// Enables all required clock sources.
void Setup_RCC (void) {
    ////
    //    GPIO's
    ////

    __GPIOB_CLK_ENABLE ();
    __GPIOA_CLK_ENABLE ();

    ////
    //    UART's
    ////

    __USART1_CLK_ENABLE ();
}

/// Configures all the GPIO's.
void Setup_GPIO (void) {
    GPIO_InitTypeDef GPIO_InitStruct;
  
  ////
  //    User LED's
  ////
  
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;

  GPIO_InitStruct.Pin = LD1_PIN | LD2_PIN | LD3_PIN;
  HAL_GPIO_Init (LD123_GPIO, &GPIO_InitStruct);

  ////
  //    USART1 RX/TX
  ////

  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Alternate = 7;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_InitStruct.Pin = USART1_TX_PIN | USART1_RX_PIN;
  HAL_GPIO_Init (USART1_RX_TX_GPIO, &GPIO_InitStruct);
}

/// Configures all the UART's.
void Setup_UART (void) {
  ////
  //    USART1
  ////

  if (HAL_UART_Init (&s_DebugUARTHandle) != HAL_OK)
    Error_Handler ();
}
